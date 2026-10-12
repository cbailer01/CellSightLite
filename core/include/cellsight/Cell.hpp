#pragma once

#include <memory>
#include <string>

#include "cellsight/Datasheet.hpp"
#include "cellsight/TestBench.hpp"
#include "cellsight/TestPlan.hpp"
#include "cellsight/TimeSeries.hpp"

namespace cs {

// Une cellule physique du laboratoire, identifiée par son numéro.
//
// Fiche technique, banc et plan de test sont PARTAGÉS entre plusieurs cellules :
// la cellule les référence par std::shared_ptr<const ...> au lieu de les copier.
class Cell {
public:
    // Lève std::invalid_argument si les données sont incohérentes (étape 1).
    Cell(std::string id, std::shared_ptr<const Datasheet> datasheet,
         std::shared_ptr<const TestBench> bench, int channel,
         std::shared_ptr<const TestPlan> plan, std::string location, std::string startDate,
         std::string dataFile);

    const std::string& id() const { return id_; }
    const Datasheet& datasheet() const { return *datasheet_; }
    const TestBench& bench() const { return *bench_; }
    int channel() const { return channel_; }
    const TestPlan& plan() const { return *plan_; }
    const std::string& location() const { return location_; }
    const std::string& startDate() const { return startDate_; }
    const std::string& dataFile() const { return dataFile_; }  // relatif au dossier de données

    // Étape 4 — charge les mesures avec l'importeur que fournit le banc.
    void loadData(const std::string& dataDirectory);
    bool hasData() const { return loaded_; }
    const TimeSeries& data() const;  // lève std::logic_error si rien n'est chargé

private:
    void validate() const;  // Étape 1

    std::string id_;
    std::shared_ptr<const Datasheet> datasheet_;
    std::shared_ptr<const TestBench> bench_;
    int channel_;
    std::shared_ptr<const TestPlan> plan_;
    std::string location_;
    std::string startDate_;
    std::string dataFile_;
    TimeSeries data_;
    bool loaded_ = false;
};

}  // namespace cs
