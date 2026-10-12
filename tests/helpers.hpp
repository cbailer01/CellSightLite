#pragma once
// Outils communs aux tests : accès aux données synthétiques et à la vérité terrain.

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "cellsight/DataImporter.hpp"
#include "cellsight/Repository.hpp"
#include "cellsight/TimeSeries.hpp"

namespace helpers {

inline std::string dataDir() {
    return CELLSIGHT_DATA_DIR;
}

inline cs::CsvRepository& repository() {
    static cs::CsvRepository repo(dataDir());
    return repo;
}

// Mesures d'une cellule, lues UNE fois puis gardées en cache. On appelle
// l'importeur directement (et non Cell::loadData, étape 4) pour que les tests
// des étapes 2 et 5 ne dépendent pas de l'étape 4.
inline const cs::TimeSeries& cellData(const std::string& id) {
    static std::map<std::string, cs::TimeSeries> cache;
    auto it = cache.find(id);
    if (it != cache.end()) {
        return it->second;
    }
    std::shared_ptr<cs::Cell> cell = repository().findCell(id);
    if (!cell) {
        throw std::runtime_error("Cellule inconnue : " + id);
    }
    const std::string path = dataDir() + "/" + cell->dataFile();
    cs::TimeSeries series = cell->dataFile().find("arbin") != std::string::npos
                                ? cs::ArbinImporter().load(path)
                                : cs::BasytecImporter().load(path);
    return cache.emplace(id, std::move(series)).first->second;
}

// Vérité terrain écrite par le générateur (valeurs sans bruit de mesure).
struct Truth {
    int cycle = 0;
    double qCharge_Ah = 0.0;
    double qDischarge_Ah = 0.0;
    double eDischarge_Wh = 0.0;
    double r0_mOhm = 0.0;
    double rDc_mOhm = 0.0;  // ce que mesure dV/dI au début de la décharge
    double capacity_Ah = 0.0;
};

inline std::vector<Truth> truth(const std::string& id) {
    std::ifstream in(dataDir() + "/truth/" + id + ".csv");
    if (!in.is_open()) {
        throw std::runtime_error("Verite terrain absente pour " + id);
    }
    std::vector<Truth> rows;
    std::string line;
    std::getline(in, line);  // en-tête
    while (std::getline(in, line)) {
        std::replace(line.begin(), line.end(), ';', ' ');
        std::istringstream fields(line);
        Truth t;
        fields >> t.cycle >> t.qCharge_Ah >> t.qDischarge_Ah >> t.eDischarge_Wh >> t.r0_mOhm >>
            t.rDc_mOhm >> t.capacity_Ah;
        rows.push_back(t);
    }
    return rows;
}

inline Truth truthAt(const std::string& id, int cycle) {
    return truth(id).at(static_cast<std::size_t>(cycle - 1));
}

inline cs::Measurement point(double t, double current, double voltage, double temperature,
                             int cycle, int step = 1) {
    cs::Measurement m;
    m.time_s = t;
    m.current_A = current;
    m.voltage_V = voltage;
    m.temperature_C = temperature;
    m.cycle = cycle;
    m.step = step;
    return m;
}

inline std::string tempPath(const std::string& name) {
    return (std::filesystem::temp_directory_path() / ("cellsight_" + name)).string();
}

}  // namespace helpers
