#include "cellsight/Integration.hpp"

#include <algorithm>
#include <stdexcept>

#include "cellsight/NotImplemented.hpp"

namespace cs {

double trapezoid(const std::vector<double>& x, const std::vector<double>& y) {
    // TODO Étape 5 — trapezoid : à écrire (voir le sujet).
    throw NotImplemented(5, "trapezoid");
}

double charge_Ah(const TimeSeries& series, Direction direction) {
    // TODO Étape 5 — charge_Ah : à écrire (voir le sujet).
    throw NotImplemented(5, "charge_Ah");
}

double energy_Wh(const TimeSeries& series, Direction direction) {
    // TODO Étape 5 — energy_Wh : à écrire (voir le sujet).
    throw NotImplemented(5, "energy_Wh");
}

}  // namespace cs
