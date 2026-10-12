#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "cellsight/Cell.hpp"
#include "cellsight/Datasheet.hpp"
#include "cellsight/TestBench.hpp"
#include "cellsight/TestPlan.hpp"
#include "cellsight/User.hpp"

namespace cs {

// Où sont rangées les cellules. L'interface graphique ne connaît QUE cette
// interface : dans CellSight, une autre classe dérivée interroge un serveur
// (API REST) au lieu de lire des fichiers, sans rien changer à l'interface.
class CellRepository {
public:
    virtual ~CellRepository() = default;
    virtual std::vector<std::shared_ptr<Cell>> cells() const = 0;
    virtual std::shared_ptr<Cell> findCell(const std::string& id) const = 0;  // nullptr si absent
    virtual std::shared_ptr<const TestPlan> findPlan(const std::string& id) const = 0;
    virtual std::shared_ptr<const Datasheet> findDatasheet(const std::string& id) const = 0;
    virtual std::vector<User> users() const = 0;
    virtual std::string dataDirectory() const = 0;
};

// Fourni — lit le catalogue du laboratoire dans des fichiers ';' :
// datasheets.csv, benches.csv, plans/*.csv, cells.csv, users.csv.
class CsvRepository : public CellRepository {
public:
    explicit CsvRepository(std::string dataDirectory);  // lève std::runtime_error

    std::vector<std::shared_ptr<Cell>> cells() const override { return cells_; }
    std::shared_ptr<Cell> findCell(const std::string& id) const override;
    std::shared_ptr<const TestPlan> findPlan(const std::string& id) const override;
    std::shared_ptr<const Datasheet> findDatasheet(const std::string& id) const override;
    std::vector<User> users() const override { return users_; }
    std::string dataDirectory() const override { return dataDirectory_; }

private:
    std::string dataDirectory_;
    std::map<std::string, std::shared_ptr<const Datasheet>> datasheets_;
    std::map<std::string, std::shared_ptr<const TestBench>> benches_;
    std::map<std::string, std::shared_ptr<const TestPlan>> plans_;
    std::vector<std::shared_ptr<Cell>> cells_;
    std::vector<User> users_;
};

}  // namespace cs
