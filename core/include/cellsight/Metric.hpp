#pragma once

#include <string>

#include "cellsight/Datasheet.hpp"
#include "cellsight/TimeSeries.hpp"

namespace cs {

// Un indicateur calculé cycle par cycle (patron Stratégie).
// Ajouter un indicateur = écrire une classe dérivée et l'enregistrer dans
// CycleAnalyzer::withDefaultMetrics() : l'interface graphique le propose alors
// automatiquement, sans autre modification.
class Metric {
public:
    virtual ~Metric() = default;
    virtual std::string name() const = 0;
    virtual std::string unit() const = 0;
    // Valeur sur UN cycle ; NaN si elle n'est pas calculable.
    virtual double compute(const TimeSeries& cycle, const Datasheet& ds) const = 0;
};

// --- Fournis : les modèles à suivre ------------------------------------------

class ChargeCapacity : public Metric {
public:
    std::string name() const override { return "Capacité chargée"; }
    std::string unit() const override { return "Ah"; }
    double compute(const TimeSeries& cycle, const Datasheet& ds) const override;
};

class MaxTemperature : public Metric {
public:
    std::string name() const override { return "Température max"; }
    std::string unit() const override { return "°C"; }
    double compute(const TimeSeries& cycle, const Datasheet& ds) const override;
};

// --- Étape 5 -------------------------------------------------------------------

class DischargeCapacity : public Metric {
public:
    std::string name() const override { return "Capacité déchargée"; }
    std::string unit() const override { return "Ah"; }
    double compute(const TimeSeries& cycle, const Datasheet& ds) const override;
};

class DischargeEnergy : public Metric {
public:
    std::string name() const override { return "Énergie déchargée"; }
    std::string unit() const override { return "Wh"; }
    double compute(const TimeSeries& cycle, const Datasheet& ds) const override;
};

// 100 * Qdécharge / Qcharge, sur le même cycle.
class CoulombicEfficiency : public Metric {
public:
    std::string name() const override { return "Rendement coulombique"; }
    std::string unit() const override { return "%"; }
    double compute(const TimeSeries& cycle, const Datasheet& ds) const override;
};

// État de santé : 100 * Qdécharge / Qnominale (fiche technique).
class StateOfHealth : public Metric {
public:
    std::string name() const override { return "SOH"; }
    std::string unit() const override { return "%"; }
    double compute(const TimeSeries& cycle, const Datasheet& ds) const override;
};

// Résistance interne : saut de tension au passage repos -> décharge, divisé par
// le saut de courant (R = dV / dI), en milliohms.
class InternalResistance : public Metric {
public:
    std::string name() const override { return "Résistance interne"; }
    std::string unit() const override { return "mOhm"; }
    double compute(const TimeSeries& cycle, const Datasheet& ds) const override;
};

}  // namespace cs
