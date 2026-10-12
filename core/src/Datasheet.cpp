#include "cellsight/Datasheet.hpp"

#include <cstdio>
#include <stdexcept>
#include <utility>

#include "cellsight/NotImplemented.hpp"

namespace cs {

std::string toString(Chemistry c) {
    switch (c) {
        case Chemistry::NMC: return "NMC";
        case Chemistry::NCA: return "NCA";
        case Chemistry::LFP: return "LFP";
        case Chemistry::LCO: return "LCO";
        case Chemistry::LTO: return "LTO";
    }
    return "?";
}

Chemistry chemistryFromString(const std::string& text) {
    for (Chemistry c : {Chemistry::NMC, Chemistry::NCA, Chemistry::LFP, Chemistry::LCO,
                        Chemistry::LTO}) {
        if (toString(c) == text) {
            return c;
        }
    }
    throw std::invalid_argument("Chimie inconnue : " + text);
}

Datasheet::Datasheet(std::string id, std::string manufacturer, std::string model,
                     Chemistry chemistry, std::string formFactor, double nominalCapacity_Ah,
                     double nominalVoltage_V, OperatingLimits limits, std::string pdfPath)
    : id_(std::move(id)),
      manufacturer_(std::move(manufacturer)),
      model_(std::move(model)),
      chemistry_(chemistry),
      formFactor_(std::move(formFactor)),
      nominalCapacity_Ah_(nominalCapacity_Ah),
      nominalVoltage_V_(nominalVoltage_V),
      limits_(limits),
      pdfPath_(std::move(pdfPath)) {
    validate();
}

std::string Datasheet::displayName() const {
    char capacity[32];
    std::snprintf(capacity, sizeof capacity, "%.2f Ah", nominalCapacity_Ah_);
    return manufacturer_ + " " + model_ + " (" + toString(chemistry_) + ", " + capacity + ")";
}

void Datasheet::validate() const {
    // TODO Étape 1 — Datasheet::validate : à écrire (voir le sujet).
}

bool Datasheet::voltageInRange(double voltage_V) const {
    // TODO Étape 1 — Datasheet::voltageInRange : à écrire (voir le sujet).
    throw NotImplemented(1, "Datasheet::voltageInRange");
}

double Datasheet::cRateToCurrent(double cRate) const {
    // TODO Étape 1 — Datasheet::cRateToCurrent : à écrire (voir le sujet).
    throw NotImplemented(1, "Datasheet::cRateToCurrent");
}

}  // namespace cs
