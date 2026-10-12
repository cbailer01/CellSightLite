#include "cellsight/TestPlan.hpp"

#include <cstdio>
#include <stdexcept>
#include <utility>

#include "cellsight/NotImplemented.hpp"

namespace cs {

std::string toString(StepType t) {
    switch (t) {
        case StepType::ChargeCC: return "CHARGE_CC";
        case StepType::ChargeCV: return "CHARGE_CV";
        case StepType::DischargeCC: return "DISCHARGE_CC";
        case StepType::Rest: return "REST";
    }
    return "?";
}

StepType stepTypeFromString(const std::string& text) {
    for (StepType t : {StepType::ChargeCC, StepType::ChargeCV, StepType::DischargeCC,
                       StepType::Rest}) {
        if (toString(t) == text) {
            return t;
        }
    }
    throw std::invalid_argument("Type d'etape inconnu : " + text);
}

// Mise en forme printf : plus simple qu'un ostringstream pour fixer les décimales.
static std::string format(const char* pattern, double a, double b = 0.0) {
    char buffer[128];
    std::snprintf(buffer, sizeof buffer, pattern, a, b);
    return buffer;
}

std::string Step::describe() const {
    switch (type) {
        case StepType::ChargeCC: return format("Charge CC %.2gC jusqu'à %.2f V", cRate, voltage_V);
        case StepType::ChargeCV:
            return format("Charge CV à %.2f V jusqu'à %.2gC", voltage_V, cutoffCRate);
        case StepType::DischargeCC:
            return format("Décharge CC %.2gC jusqu'à %.2f V", cRate, voltage_V);
        case StepType::Rest: return format("Repos %.0f min", duration_s / 60.0);
    }
    return "?";
}

TestPlan::TestPlan(std::string id, std::string name, int repetitions)
    : id_(std::move(id)), name_(std::move(name)), repetitions_(repetitions) {}

void TestPlan::addStep(const Step& step) {
    steps_.push_back(step);
}

std::vector<std::string> TestPlan::checkAgainst(const Datasheet& ds) const {
    // TODO Étape 6 — TestPlan::checkAgainst : à écrire (voir le sujet).
    throw NotImplemented(6, "TestPlan::checkAgainst");
}

}  // namespace cs
