#pragma once

#include <string>

namespace cs {

enum class Chemistry { NMC, NCA, LFP, LCO, LTO };

std::string toString(Chemistry c);
Chemistry chemistryFromString(const std::string& text);  // lève std::invalid_argument

// Limites de fonctionnement données par le fabricant.
struct OperatingLimits {
    double minVoltage_V = 0.0;
    double maxVoltage_V = 0.0;
    double maxChargeCurrent_A = 0.0;
    double maxDischargeCurrent_A = 0.0;
    double minTemperature_C = 0.0;
    double maxTemperature_C = 0.0;
};

// Fiche technique d'un modèle de cellule. Plusieurs cellules du même modèle
// partagent la même fiche (voir Cell : std::shared_ptr<const Datasheet>).
class Datasheet {
public:
    // Lève std::invalid_argument si les caractéristiques sont incohérentes.
    Datasheet(std::string id, std::string manufacturer, std::string model, Chemistry chemistry,
              std::string formFactor, double nominalCapacity_Ah, double nominalVoltage_V,
              OperatingLimits limits, std::string pdfPath);

    const std::string& id() const { return id_; }
    const std::string& manufacturer() const { return manufacturer_; }
    const std::string& model() const { return model_; }
    Chemistry chemistry() const { return chemistry_; }
    const std::string& formFactor() const { return formFactor_; }
    double nominalCapacity_Ah() const { return nominalCapacity_Ah_; }
    double nominalVoltage_V() const { return nominalVoltage_V_; }
    const OperatingLimits& limits() const { return limits_; }
    const std::string& pdfPath() const { return pdfPath_; }

    // « Voltacell VC-18650-25N (NMC, 2.50 Ah) »
    std::string displayName() const;

    // Étape 1
    bool voltageInRange(double voltage_V) const;  // bornes incluses
    double cRateToCurrent(double cRate) const;    // 1C = capacité nominale en A

private:
    void validate() const;  // Étape 1

    std::string id_;
    std::string manufacturer_;
    std::string model_;
    Chemistry chemistry_;
    std::string formFactor_;
    double nominalCapacity_Ah_;
    double nominalVoltage_V_;
    OperatingLimits limits_;
    std::string pdfPath_;
};

}  // namespace cs
