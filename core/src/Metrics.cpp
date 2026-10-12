#include <cmath>
#include <limits>

#include "cellsight/Integration.hpp"
#include "cellsight/Metric.hpp"
#include "cellsight/NotImplemented.hpp"

namespace cs {

static const double kNaN = std::numeric_limits<double>::quiet_NaN();

// --- Fournis ---------------------------------------------------------------

double ChargeCapacity::compute(const TimeSeries& cycle, const Datasheet&) const {
    return charge_Ah(cycle, Direction::Charge);
}

double MaxTemperature::compute(const TimeSeries& cycle, const Datasheet&) const {
    if (cycle.empty()) {
        return kNaN;
    }
    return cycle.max(Quantity::Temperature);
}

// --- Étape 5 ---------------------------------------------------------------

double DischargeCapacity::compute(const TimeSeries& cycle, const Datasheet& ds) const {
    // TODO Étape 5 — DischargeCapacity::compute : à écrire (voir le sujet).
    throw NotImplemented(5, "DischargeCapacity::compute");
}

double DischargeEnergy::compute(const TimeSeries& cycle, const Datasheet& ds) const {
    // TODO Étape 5 — DischargeEnergy::compute : à écrire (voir le sujet).
    throw NotImplemented(5, "DischargeEnergy::compute");
}

double CoulombicEfficiency::compute(const TimeSeries& cycle, const Datasheet& ds) const {
    // TODO Étape 5 — CoulombicEfficiency::compute : à écrire (voir le sujet).
    throw NotImplemented(5, "CoulombicEfficiency::compute");
}

double StateOfHealth::compute(const TimeSeries& cycle, const Datasheet& ds) const {
    // TODO Étape 5 — StateOfHealth::compute : à écrire (voir le sujet).
    throw NotImplemented(5, "StateOfHealth::compute");
}

double InternalResistance::compute(const TimeSeries& cycle, const Datasheet& ds) const {
    // TODO Étape 5 — InternalResistance::compute : à écrire (voir le sujet).
    throw NotImplemented(5, "InternalResistance::compute");
}

}  // namespace cs
