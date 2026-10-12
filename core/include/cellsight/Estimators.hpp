#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "cellsight/Datasheet.hpp"
#include "cellsight/TimeSeries.hpp"

namespace cs {

// Estimateur d'état de charge : une valeur de SOC (en %) par point de mesure.
// Dans CellSight, d'autres classes dérivées appellent des modèles d'IA distants.
class SocEstimator {
public:
    virtual ~SocEstimator() = default;
    virtual std::string name() const = 0;
    virtual std::vector<double> estimate(const TimeSeries& series, const Datasheet& ds) const = 0;
};

// Étape 6 — comptage coulombien :
//   SOC(k) = SOC(k-1) + 100 * intégrale de I dt / Qnominale,
// recalé à 100 % en fin de charge CV (tension proche de Vmax et courant faible),
// et borné à [0, 100].
class CoulombCountingSoc : public SocEstimator {
public:
    explicit CoulombCountingSoc(double initialSoc_pct = 0.0) : initialSoc_pct_(initialSoc_pct) {}
    std::string name() const override { return "Comptage coulombien"; }
    std::vector<double> estimate(const TimeSeries& series, const Datasheet& ds) const override;

private:
    double initialSoc_pct_;
};

// Bonus — durée de vie restante (RUL) par régression linéaire du SOH.
struct RulEstimate {
    bool valid = false;               // faux si moins de 3 points ou SOH non décroissant
    double slope_pctPerCycle = 0.0;
    double endOfLifeCycle = 0.0;      // cycle où le SOH atteint le seuil
    double remainingCycles = 0.0;     // endOfLifeCycle - dernier cycle (>= 0)
};

class LinearRulEstimator {
public:
    explicit LinearRulEstimator(double threshold_pct = 80.0, std::size_t window = 30)
        : threshold_pct_(threshold_pct), window_(window) {}
    // Ajuste une droite sur les `window` derniers points valides (NaN ignorés).
    RulEstimate estimate(const std::vector<int>& cycles, const std::vector<double>& soh_pct) const;

private:
    double threshold_pct_;
    std::size_t window_;
};

}  // namespace cs
