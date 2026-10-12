#pragma once

#include <vector>

#include "cellsight/TimeSeries.hpp"

namespace cs {

enum class Direction { Charge, Discharge };

// Étape 5 — intégrale de y(x) par la méthode des trapèzes.
// 0 si moins de deux points ; lève std::invalid_argument si x et y n'ont pas la même taille.
double trapezoid(const std::vector<double>& x, const std::vector<double>& y);

// Étape 5 — charge échangée dans un sens, en Ah : intégrale de max(0, +I) en charge,
// de max(0, -I) en décharge, divisée par 3600.
double charge_Ah(const TimeSeries& series, Direction direction);

// Étape 5 — énergie échangée dans un sens, en Wh : même principe avec V * I.
double energy_Wh(const TimeSeries& series, Direction direction);

}  // namespace cs
