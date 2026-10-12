#include "cellsight/Exporter.hpp"

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "cellsight/NotImplemented.hpp"

namespace cs {

// --- CSV (Étape 7) -----------------------------------------------------------

std::string CsvExporter::name() const {
    // TODO Étape 7 — CsvExporter::name : à écrire (voir le sujet).
    throw NotImplemented(7, "CsvExporter::name");
}

std::string CsvExporter::extension() const {
    // TODO Étape 7 — CsvExporter::extension : à écrire (voir le sujet).
    throw NotImplemented(7, "CsvExporter::extension");
}

void CsvExporter::write(const TimeSeries& series, const std::string& path) const {
    // TODO Étape 7 — CsvExporter::write : à écrire (voir le sujet).
    throw NotImplemented(7, "CsvExporter::write");
}

// --- MAT niveau 4 (fourni) ---------------------------------------------------
// Pour chaque variable : un en-tête de 5 entiers 32 bits
//   type (0 = double, petit-boutiste), lignes, colonnes, imaginaire (0), longueur du nom + 1
// puis le nom terminé par '\0', puis les valeurs en double, colonne par colonne.

static void writeMatVariable(std::ofstream& out, const std::string& name,
                             const std::vector<double>& column) {
    const std::int32_t header[5] = {0, static_cast<std::int32_t>(column.size()), 1, 0,
                                    static_cast<std::int32_t>(name.size() + 1)};
    out.write(reinterpret_cast<const char*>(header), sizeof header);
    out.write(name.c_str(), static_cast<std::streamsize>(name.size() + 1));
    out.write(reinterpret_cast<const char*>(column.data()),
              static_cast<std::streamsize>(column.size() * sizeof(double)));
}

void MatExporter::write(const TimeSeries& series, const std::string& path) const {
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) {
        throw std::runtime_error("Impossible d'ecrire " + path);
    }
    std::vector<double> cycle, step;
    for (const Measurement& m : series) {
        cycle.push_back(m.cycle);
        step.push_back(m.step);
    }
    writeMatVariable(out, "time_s", series.values(Quantity::Time));
    writeMatVariable(out, "current_A", series.values(Quantity::Current));
    writeMatVariable(out, "voltage_V", series.values(Quantity::Voltage));
    writeMatVariable(out, "temperature_C", series.values(Quantity::Temperature));
    writeMatVariable(out, "cycle", cycle);
    writeMatVariable(out, "step", step);
}

}  // namespace cs
