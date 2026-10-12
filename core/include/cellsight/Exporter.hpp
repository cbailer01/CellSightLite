#pragma once

#include <string>

#include "cellsight/TimeSeries.hpp"

namespace cs {

// Écrit une série de mesures dans un fichier. Une classe dérivée par format.
class Exporter {
public:
    virtual ~Exporter() = default;
    virtual std::string name() const = 0;       // pour l'interface : « CSV », ...
    virtual std::string extension() const = 0;  // sans le point : "csv", "mat"
    // Lève std::runtime_error si le fichier ne peut pas être écrit.
    virtual void write(const TimeSeries& series, const std::string& path) const = 0;
};

// Étape 7 — séparateur ',', point décimal, une ligne d'en-tête :
//   time_s,current_A,voltage_V,temperature_C,cycle,step
class CsvExporter : public Exporter {
public:
    std::string name() const override;
    std::string extension() const override;
    void write(const TimeSeries& series, const std::string& path) const override;
};

// Fourni — fichier MATLAB niveau 4 (lisible par load() dans MATLAB et
// par scipy.io.loadmat en Python) : un vecteur colonne par grandeur.
class MatExporter : public Exporter {
public:
    std::string name() const override { return "MATLAB"; }
    std::string extension() const override { return "mat"; }
    void write(const TimeSeries& series, const std::string& path) const override;
};

}  // namespace cs
