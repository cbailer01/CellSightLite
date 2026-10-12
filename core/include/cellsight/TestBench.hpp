#pragma once

#include <memory>
#include <string>

#include "cellsight/DataImporter.hpp"

namespace cs {

// Banc de cyclage. Classe ABSTRAITE : chaque constructeur de banc exporte ses
// données dans son propre format, et c'est le banc qui sait quel importeur
// fabriquer (méthode de fabrique createImporter).
class TestBench {
public:
    TestBench(std::string id, std::string model, std::string location, int channels);
    virtual ~TestBench() = default;

    const std::string& id() const { return id_; }
    const std::string& model() const { return model_; }
    const std::string& location() const { return location_; }
    int channels() const { return channels_; }
    bool hasChannel(int channel) const { return channel >= 1 && channel <= channels_; }

    virtual std::string vendor() const = 0;
    virtual std::unique_ptr<DataImporter> createImporter() const = 0;

    // « Arbin BT-2000 (16 voies, Salle B-012) »
    std::string describe() const;

private:
    std::string id_;
    std::string model_;
    std::string location_;
    int channels_;
};

class ArbinBench : public TestBench {
public:
    using TestBench::TestBench;  // hérite du constructeur
    std::string vendor() const override;
    std::unique_ptr<DataImporter> createImporter() const override;
};

// Étape 4
class BasytecBench : public TestBench {
public:
    using TestBench::TestBench;
    std::string vendor() const override;
    std::unique_ptr<DataImporter> createImporter() const override;
};

// Fabrique un banc à partir du nom de son constructeur ("Arbin", "BaSyTec").
std::unique_ptr<TestBench> makeBench(const std::string& vendor, const std::string& id,
                                     const std::string& model, const std::string& location,
                                     int channels);

}  // namespace cs
