#pragma once

#include <memory>
#include <string>
#include <vector>

#include "cellsight/Datasheet.hpp"
#include "cellsight/Metric.hpp"
#include "cellsight/TimeSeries.hpp"

namespace cs {

// Résultat de l'analyse : un tableau « indicateur x cycle ».
struct MetricTable {
    std::vector<int> cycles;
    std::vector<std::string> names;
    std::vector<std::string> units;
    std::vector<std::vector<double>> values;  // values[indicateur][indice du cycle]
    std::vector<std::string> warnings;        // indicateurs non disponibles, et pourquoi

    int indexOf(const std::string& name) const;  // -1 si absent
    void writeCsv(const std::string& path) const;
};

// Applique une liste d'indicateurs à chaque cycle d'une série.
class CycleAnalyzer {
public:
    void add(std::unique_ptr<Metric> metric);
    static CycleAnalyzer withDefaultMetrics();

    std::vector<std::string> metricNames() const;
    // Un indicateur pas encore implémenté (NotImplemented) donne une colonne de NaN
    // et un avertissement, sans empêcher le calcul des autres.
    MetricTable analyze(const TimeSeries& series, const Datasheet& ds) const;

private:
    std::vector<std::unique_ptr<Metric>> metrics_;
};

}  // namespace cs
