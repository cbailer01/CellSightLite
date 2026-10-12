#include "cellsight/CycleAnalyzer.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <stdexcept>

#include "cellsight/NotImplemented.hpp"

namespace cs {

int MetricTable::indexOf(const std::string& name) const {
    for (std::size_t i = 0; i < names.size(); ++i) {
        if (names[i] == name) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void MetricTable::writeCsv(const std::string& path) const {
    std::ofstream out(path);
    if (!out.is_open()) {
        throw std::runtime_error("Impossible d'ecrire " + path);
    }
    out << "cycle";
    for (std::size_t m = 0; m < names.size(); ++m) {
        out << ',' << names[m] << " (" << units[m] << ')';
    }
    out << '\n';
    for (std::size_t k = 0; k < cycles.size(); ++k) {
        out << cycles[k];
        for (const std::vector<double>& column : values) {
            out << ',';
            if (!std::isnan(column[k])) {
                out << column[k];
            }
        }
        out << '\n';
    }
}

void CycleAnalyzer::add(std::unique_ptr<Metric> metric) {
    metrics_.push_back(std::move(metric));
}

CycleAnalyzer CycleAnalyzer::withDefaultMetrics() {
    CycleAnalyzer analyzer;
    analyzer.add(std::make_unique<DischargeCapacity>());
    analyzer.add(std::make_unique<ChargeCapacity>());
    analyzer.add(std::make_unique<DischargeEnergy>());
    analyzer.add(std::make_unique<CoulombicEfficiency>());
    analyzer.add(std::make_unique<StateOfHealth>());
    analyzer.add(std::make_unique<InternalResistance>());
    analyzer.add(std::make_unique<MaxTemperature>());
    return analyzer;
}

std::vector<std::string> CycleAnalyzer::metricNames() const {
    std::vector<std::string> names;
    for (const auto& m : metrics_) {
        names.push_back(m->name());
    }
    return names;
}

MetricTable CycleAnalyzer::analyze(const TimeSeries& series, const Datasheet& ds) const {
    MetricTable table;
    table.cycles = series.cycles();

    // Découper une seule fois, et non une fois par indicateur.
    std::vector<TimeSeries> perCycle;
    perCycle.reserve(table.cycles.size());
    for (int c : table.cycles) {
        perCycle.push_back(series.cycle(c));
    }

    for (const auto& metric : metrics_) {
        std::vector<double> column(table.cycles.size(), std::numeric_limits<double>::quiet_NaN());
        try {
            for (std::size_t k = 0; k < perCycle.size(); ++k) {
                column[k] = metric->compute(perCycle[k], ds);
            }
        } catch (const NotImplemented& e) {
            std::fill(column.begin(), column.end(), std::numeric_limits<double>::quiet_NaN());
            table.warnings.push_back(metric->name() + " : " + e.what());
        }
        table.names.push_back(metric->name());
        table.units.push_back(metric->unit());
        table.values.push_back(std::move(column));
    }
    return table;
}

}  // namespace cs
