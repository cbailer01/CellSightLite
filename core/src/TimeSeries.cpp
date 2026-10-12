#include "cellsight/TimeSeries.hpp"

#include <algorithm>
#include <iterator>
#include <set>
#include <stdexcept>

#include "cellsight/NotImplemented.hpp"

namespace cs {

std::string toString(Quantity q) {
    switch (q) {
        case Quantity::Time: return "Temps";
        case Quantity::Current: return "Courant";
        case Quantity::Voltage: return "Tension";
        case Quantity::Temperature: return "Temperature";
    }
    return "?";
}

std::string unitOf(Quantity q) {
    switch (q) {
        case Quantity::Time: return "s";
        case Quantity::Current: return "A";
        case Quantity::Voltage: return "V";
        case Quantity::Temperature: return "degC";
    }
    return "";
}

double valueOf(const Measurement& m, Quantity q) {
    switch (q) {
        case Quantity::Time: return m.time_s;
        case Quantity::Current: return m.current_A;
        case Quantity::Voltage: return m.voltage_V;
        case Quantity::Temperature: return m.temperature_C;
    }
    return 0.0;
}

void TimeSeries::add(const Measurement& m) {
    data_.push_back(m);
}

std::vector<double> TimeSeries::values(Quantity q) const {
    std::vector<double> out;
    out.reserve(data_.size());
    for (const Measurement& m : data_) {
        out.push_back(valueOf(m, q));
    }
    return out;
}

std::vector<int> TimeSeries::cycles() const {
    // TODO Étape 2 — TimeSeries::cycles : à écrire (voir le sujet).
    throw NotImplemented(2, "TimeSeries::cycles");
}

TimeSeries TimeSeries::cycle(int number) const {
    // TODO Étape 2 — TimeSeries::cycle : à écrire (voir le sujet).
    throw NotImplemented(2, "TimeSeries::cycle");
}

TimeSeries TimeSeries::cycleRange(int first, int last) const {
    // TODO Étape 2 — TimeSeries::cycleRange : à écrire (voir le sujet).
    throw NotImplemented(2, "TimeSeries::cycleRange");
}

double TimeSeries::duration_s() const {
    // TODO Étape 2 — TimeSeries::duration_s : à écrire (voir le sujet).
    throw NotImplemented(2, "TimeSeries::duration_s");
}

double TimeSeries::min(Quantity q) const {
    // TODO Étape 2 — TimeSeries::min : à écrire (voir le sujet).
    throw NotImplemented(2, "TimeSeries::min");
}

double TimeSeries::max(Quantity q) const {
    // TODO Étape 2 — TimeSeries::max : à écrire (voir le sujet).
    throw NotImplemented(2, "TimeSeries::max");
}

}  // namespace cs
