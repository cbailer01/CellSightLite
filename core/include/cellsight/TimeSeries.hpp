#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace cs {

// Un point de mesure du cycleur, après conversion dans les unités SI.
struct Measurement {
    double time_s = 0.0;         // temps depuis le début de l'essai
    double current_A = 0.0;      // > 0 en charge, < 0 en décharge
    double voltage_V = 0.0;
    double temperature_C = 0.0;
    int cycle = 0;               // numéro de cycle (à partir de 1)
    int step = 0;                // numéro d'étape dans le plan de test
};

enum class Quantity { Time, Current, Voltage, Temperature };

std::string toString(Quantity q);  // "Tension", ...
std::string unitOf(Quantity q);    // "V", ...
double valueOf(const Measurement& m, Quantity q);

// Suite chronologique de mesures d'une cellule.
class TimeSeries {
public:
    using const_iterator = std::vector<Measurement>::const_iterator;

    void add(const Measurement& m);
    std::size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    const Measurement& operator[](std::size_t i) const { return data_[i]; }
    const_iterator begin() const { return data_.begin(); }
    const_iterator end() const { return data_.end(); }

    // Toutes les valeurs d'une grandeur, dans l'ordre (pour tracer une courbe).
    std::vector<double> values(Quantity q) const;

    // Étape 2
    std::vector<int> cycles() const;               // numéros distincts, triés
    TimeSeries cycle(int number) const;            // mesures de ce cycle (vide si absent)
    TimeSeries cycleRange(int first, int last) const;  // cycles first..last inclus
    double duration_s() const;                     // dernier temps - premier temps (0 si vide)
    double min(Quantity q) const;                  // lève std::logic_error si vide
    double max(Quantity q) const;                  // lève std::logic_error si vide

private:
    std::vector<Measurement> data_;
};

}  // namespace cs
