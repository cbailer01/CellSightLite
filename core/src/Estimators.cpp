#include "cellsight/Estimators.hpp"

#include <algorithm>
#include <cmath>

#include "cellsight/NotImplemented.hpp"

namespace cs {

std::vector<double> CoulombCountingSoc::estimate(const TimeSeries& series,
                                                 const Datasheet& ds) const {
    // TODO Étape 6 — CoulombCountingSoc::estimate : à écrire (voir le sujet).
    throw NotImplemented(6, "CoulombCountingSoc::estimate");
}

RulEstimate LinearRulEstimator::estimate(const std::vector<int>& cycles,
                                         const std::vector<double>& soh_pct) const {
    // TODO Étape 9 — LinearRulEstimator::estimate : à écrire (voir le sujet).
    throw NotImplemented(9, "LinearRulEstimator::estimate");
}

}  // namespace cs
