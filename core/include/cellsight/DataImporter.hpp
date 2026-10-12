#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "cellsight/TimeSeries.hpp"

namespace cs {

// Lit un fichier brut de cycleur et le convertit en TimeSeries (unités SI).
// Une classe dérivée par format de fichier.
class DataImporter {
public:
    virtual ~DataImporter() = default;

    virtual std::string formatName() const = 0;
    // Lève std::runtime_error si le fichier est absent ou mal formé.
    virtual TimeSeries load(const std::string& path) const = 0;

protected:
    // Outils communs aux importeurs (fournis).
    static std::vector<std::string> split(const std::string& line, char separator);
    static double toDouble(const std::string& field);
    static int toInt(const std::string& field);
    // Position d'une colonne dans l'en-tête ; lève std::runtime_error si absente.
    static std::size_t columnIndex(const std::vector<std::string>& header,
                                   const std::string& name);
};

// Export « type Arbin » : séparateur ',', temps en secondes. FOURNI : c'est le modèle.
class ArbinImporter : public DataImporter {
public:
    std::string formatName() const override;
    TimeSeries load(const std::string& path) const override;
};

// Export « type BaSyTec » : en-tête '~', séparateur ';', temps en HEURES. Étape 3.
class BasytecImporter : public DataImporter {
public:
    std::string formatName() const override;
    TimeSeries load(const std::string& path) const override;
};

}  // namespace cs
