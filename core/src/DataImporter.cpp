#include "cellsight/DataImporter.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include "cellsight/NotImplemented.hpp"

namespace cs {

// --- Outils communs (fournis) ----------------------------------------------

std::vector<std::string> DataImporter::split(const std::string& line, char separator) {
    std::vector<std::string> fields;
    std::string field;
    std::istringstream in(line);
    while (std::getline(in, field, separator)) {
        fields.push_back(field);
    }
    // Fichier édité sous Windows : getline laisse le '\r' de fin de ligne.
    if (!fields.empty() && !fields.back().empty() && fields.back().back() == '\r') {
        fields.back().pop_back();
    }
    return fields;
}

double DataImporter::toDouble(const std::string& field) {
    try {
        std::size_t used = 0;
        double value = std::stod(field, &used);
        if (used != field.size()) {
            throw std::invalid_argument(field);
        }
        return value;
    } catch (const std::exception&) {
        throw std::runtime_error("Nombre invalide : '" + field + "'");
    }
}

int DataImporter::toInt(const std::string& field) {
    try {
        std::size_t used = 0;
        int value = std::stoi(field, &used);
        if (used != field.size()) {
            throw std::invalid_argument(field);
        }
        return value;
    } catch (const std::exception&) {
        throw std::runtime_error("Entier invalide : '" + field + "'");
    }
}

std::size_t DataImporter::columnIndex(const std::vector<std::string>& header,
                                      const std::string& name) {
    for (std::size_t i = 0; i < header.size(); ++i) {
        if (header[i] == name) {
            return i;
        }
    }
    throw std::runtime_error("Colonne absente : " + name);
}

// --- Arbin (fourni : le modèle à suivre pour BaSyTec) ----------------------

std::string ArbinImporter::formatName() const {
    return "Arbin CSV";
}

TimeSeries ArbinImporter::load(const std::string& path) const {
    std::ifstream in(path);
    // Sur MinGW, if (!in) ne détecte pas un fichier absent : tester is_open().
    if (!in.is_open()) {
        throw std::runtime_error("Impossible d'ouvrir " + path);
    }

    std::string line;
    if (!std::getline(in, line)) {
        throw std::runtime_error("Fichier vide : " + path);
    }
    // Les colonnes sont repérées par leur NOM : l'ordre peut changer.
    const std::vector<std::string> header = split(line, ',');
    const std::size_t iTime = columnIndex(header, "Test_Time(s)");
    const std::size_t iStep = columnIndex(header, "Step_Index");
    const std::size_t iCycle = columnIndex(header, "Cycle_Index");
    const std::size_t iCurrent = columnIndex(header, "Current(A)");
    const std::size_t iVoltage = columnIndex(header, "Voltage(V)");
    const std::size_t iTemp = columnIndex(header, "Aux_Temperature_1(C)");

    TimeSeries series;
    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        const std::vector<std::string> f = split(line, ',');
        if (f.size() != header.size()) {
            throw std::runtime_error("Ligne mal formee : " + line);
        }
        Measurement m;
        m.time_s = toDouble(f[iTime]);
        m.step = toInt(f[iStep]);
        m.cycle = toInt(f[iCycle]);
        m.current_A = toDouble(f[iCurrent]);  // Arbin : > 0 en charge, comme nous
        m.voltage_V = toDouble(f[iVoltage]);
        m.temperature_C = toDouble(f[iTemp]);
        series.add(m);
    }
    return series;
}

// --- BaSyTec (Étape 3) -----------------------------------------------------

std::string BasytecImporter::formatName() const {
    // TODO Étape 3 — BasytecImporter::formatName : à écrire (voir le sujet).
    throw NotImplemented(3, "BasytecImporter::formatName");
}

TimeSeries BasytecImporter::load(const std::string& path) const {
    // TODO Étape 3 — BasytecImporter::load : à écrire (voir le sujet).
    throw NotImplemented(3, "BasytecImporter::load");
}

}  // namespace cs
