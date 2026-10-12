#pragma once

#include <string>
#include <vector>

#include "cellsight/Datasheet.hpp"

namespace cs {

enum class StepType { ChargeCC, ChargeCV, DischargeCC, Rest };

std::string toString(StepType t);                    // "CHARGE_CC", ...
StepType stepTypeFromString(const std::string& text);  // lève std::invalid_argument

// Une étape du plan de test, telle que programmée sur le cycleur.
struct Step {
    StepType type = StepType::Rest;
    double cRate = 0.0;        // CC : régime de courant (multiple de la capacité nominale)
    double voltage_V = 0.0;    // CC : tension d'arrêt ; CV : tension maintenue
    double cutoffCRate = 0.0;  // CV : arrêt quand le courant passe sous ce régime
    double duration_s = 0.0;   // Rest : durée

    std::string describe() const;  // « Charge CC 0.5C jusqu'à 4.20 V »
};

class TestPlan {
public:
    TestPlan(std::string id, std::string name, int repetitions);

    void addStep(const Step& step);

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    int repetitions() const { return repetitions_; }
    const std::vector<Step>& steps() const { return steps_; }

    // Étape 6 — Vérifie le plan contre les limites de la fiche technique.
    // Renvoie un message par violation ; un vecteur vide signifie « plan conforme ».
    std::vector<std::string> checkAgainst(const Datasheet& ds) const;

private:
    std::string id_;
    std::string name_;
    int repetitions_;
    std::vector<Step> steps_;
};

}  // namespace cs
