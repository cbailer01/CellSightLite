#include "cellsight/Cell.hpp"

#include <stdexcept>
#include <utility>

#include "cellsight/NotImplemented.hpp"

namespace cs {

Cell::Cell(std::string id, std::shared_ptr<const Datasheet> datasheet,
           std::shared_ptr<const TestBench> bench, int channel,
           std::shared_ptr<const TestPlan> plan, std::string location, std::string startDate,
           std::string dataFile)
    : id_(std::move(id)),
      datasheet_(std::move(datasheet)),
      bench_(std::move(bench)),
      channel_(channel),
      plan_(std::move(plan)),
      location_(std::move(location)),
      startDate_(std::move(startDate)),
      dataFile_(std::move(dataFile)) {
    validate();
}

void Cell::validate() const {
    // TODO Étape 1 — Cell::validate : à écrire (voir le sujet).
}

void Cell::loadData(const std::string& dataDirectory) {
    // TODO Étape 4 — Cell::loadData : à écrire (voir le sujet).
    throw NotImplemented(4, "Cell::loadData");
}

const TimeSeries& Cell::data() const {
    if (!loaded_) {
        throw std::logic_error("Cellule " + id_ + " : mesures non chargees (appeler loadData)");
    }
    return data_;
}

}  // namespace cs
