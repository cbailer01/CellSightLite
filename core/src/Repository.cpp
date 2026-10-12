#include "cellsight/Repository.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace cs {

namespace {

using Row = std::map<std::string, std::string>;

std::vector<std::string> splitFields(const std::string& line, char sep) {
    std::vector<std::string> fields;
    std::string field;
    std::istringstream in(line);
    while (std::getline(in, field, sep)) {
        fields.push_back(field);
    }
    if (!fields.empty() && !fields.back().empty() && fields.back().back() == '\r') {
        fields.back().pop_back();
    }
    return fields;
}

std::ifstream openOrThrow(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Impossible d'ouvrir " + path);
    }
    return in;
}

// Lit un tableau ';' dont la première ligne utile est l'en-tête.
// Les lignes commençant par '#' sont ignorées.
std::vector<Row> readTable(const std::string& path) {
    std::ifstream in = openOrThrow(path);
    std::vector<Row> rows;
    std::vector<std::string> header;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#' || line == "\r") {
            continue;
        }
        std::vector<std::string> fields = splitFields(line, ';');
        if (header.empty()) {
            header = fields;
            continue;
        }
        if (fields.size() != header.size()) {
            throw std::runtime_error(path + " : ligne mal formee : " + line);
        }
        Row row;
        for (std::size_t i = 0; i < header.size(); ++i) {
            row[header[i]] = fields[i];
        }
        rows.push_back(row);
    }
    return rows;
}

const std::string& field(const Row& row, const std::string& name) {
    auto it = row.find(name);
    if (it == row.end()) {
        throw std::runtime_error("Champ absent : " + name);
    }
    return it->second;
}

double number(const Row& row, const std::string& name) {
    return std::stod(field(row, name));
}

// Plan de test : métadonnées en lignes « # cle;valeur », puis un tableau d'étapes.
std::shared_ptr<TestPlan> readPlan(const std::string& path) {
    std::ifstream in = openOrThrow(path);
    std::map<std::string, std::string> meta;
    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("# ", 0) == 0) {
            std::vector<std::string> kv = splitFields(line.substr(2), ';');
            if (kv.size() == 2) {
                meta[kv[0]] = kv[1];
            }
        }
    }
    auto plan = std::make_shared<TestPlan>(meta.at("id"), meta.at("nom"),
                                           std::stoi(meta.at("repetitions")));
    for (const Row& r : readTable(path)) {
        Step s;
        s.type = stepTypeFromString(field(r, "type"));
        s.cRate = number(r, "c_rate");
        s.voltage_V = number(r, "voltage_V");
        s.cutoffCRate = number(r, "cutoff_c_rate");
        s.duration_s = number(r, "duration_s");
        plan->addStep(s);
    }
    return plan;
}

}  // namespace

CsvRepository::CsvRepository(std::string dataDirectory)
    : dataDirectory_(std::move(dataDirectory)) {
    const std::string dir = dataDirectory_ + "/";

    for (const Row& r : readTable(dir + "datasheets.csv")) {
        OperatingLimits lim;
        lim.minVoltage_V = number(r, "v_min_V");
        lim.maxVoltage_V = number(r, "v_max_V");
        lim.maxChargeCurrent_A = number(r, "max_charge_A");
        lim.maxDischargeCurrent_A = number(r, "max_discharge_A");
        lim.minTemperature_C = number(r, "t_min_C");
        lim.maxTemperature_C = number(r, "t_max_C");
        auto ds = std::make_shared<const Datasheet>(
            field(r, "id"), field(r, "manufacturer"), field(r, "model"),
            chemistryFromString(field(r, "chemistry")), field(r, "form_factor"),
            number(r, "nominal_capacity_Ah"), number(r, "nominal_voltage_V"), lim,
            dir + field(r, "pdf"));
        datasheets_[ds->id()] = ds;
    }

    for (const Row& r : readTable(dir + "benches.csv")) {
        benches_[field(r, "id")] = makeBench(field(r, "vendor"), field(r, "id"),
                                             field(r, "model"), field(r, "location"),
                                             std::stoi(field(r, "channels")));
    }

    for (const auto& entry : std::filesystem::directory_iterator(dir + "plans")) {
        if (entry.path().extension() == ".csv") {
            std::shared_ptr<TestPlan> plan = readPlan(entry.path().string());
            plans_[plan->id()] = plan;
        }
    }

    for (const Row& r : readTable(dir + "cells.csv")) {
        cells_.push_back(std::make_shared<Cell>(
            field(r, "id"), findDatasheet(field(r, "datasheet_id")),
            benches_.count(field(r, "bench_id")) ? benches_.at(field(r, "bench_id")) : nullptr,
            std::stoi(field(r, "channel")), findPlan(field(r, "plan_id")), field(r, "location"),
            field(r, "start_date"), field(r, "data_file")));
    }

    for (const Row& r : readTable(dir + "users.csv")) {
        users_.emplace_back(field(r, "login"), field(r, "name"), roleFromString(field(r, "role")));
    }
}

std::shared_ptr<Cell> CsvRepository::findCell(const std::string& id) const {
    for (const auto& cell : cells_) {
        if (cell->id() == id) {
            return cell;
        }
    }
    return nullptr;
}

std::shared_ptr<const TestPlan> CsvRepository::findPlan(const std::string& id) const {
    auto it = plans_.find(id);
    return it == plans_.end() ? nullptr : it->second;
}

std::shared_ptr<const Datasheet> CsvRepository::findDatasheet(const std::string& id) const {
    auto it = datasheets_.find(id);
    return it == datasheets_.end() ? nullptr : it->second;
}

}  // namespace cs
