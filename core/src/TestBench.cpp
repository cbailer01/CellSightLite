#include "cellsight/TestBench.hpp"

#include <stdexcept>
#include <utility>

#include "cellsight/NotImplemented.hpp"

namespace cs {

TestBench::TestBench(std::string id, std::string model, std::string location, int channels)
    : id_(std::move(id)), model_(std::move(model)), location_(std::move(location)),
      channels_(channels) {
    if (channels_ <= 0) {
        throw std::invalid_argument("Banc " + id_ + " : nombre de voies <= 0");
    }
}

std::string TestBench::describe() const {
    // vendor() est virtuelle : ici, c'est la version de la classe dérivée qui s'exécute.
    return vendor() + " " + model_ + " (" + std::to_string(channels_) + " voies, " + location_ +
           ")";
}

std::string ArbinBench::vendor() const {
    return "Arbin";
}

std::unique_ptr<DataImporter> ArbinBench::createImporter() const {
    return std::make_unique<ArbinImporter>();
}

std::string BasytecBench::vendor() const {
    // TODO Étape 4 — BasytecBench::vendor : à écrire (voir le sujet).
    throw NotImplemented(4, "BasytecBench::vendor");
}

std::unique_ptr<DataImporter> BasytecBench::createImporter() const {
    // TODO Étape 4 — BasytecBench::createImporter : à écrire (voir le sujet).
    throw NotImplemented(4, "BasytecBench::createImporter");
}

std::unique_ptr<TestBench> makeBench(const std::string& vendor, const std::string& id,
                                     const std::string& model, const std::string& location,
                                     int channels) {
    if (vendor == "Arbin") {
        return std::make_unique<ArbinBench>(id, model, location, channels);
    }
    if (vendor == "BaSyTec") {
        return std::make_unique<BasytecBench>(id, model, location, channels);
    }
    throw std::invalid_argument("Constructeur de banc inconnu : " + vendor);
}

}  // namespace cs
