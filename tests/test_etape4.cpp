// Étape 4 — hiérarchie des bancs, méthode de fabrique, chargement polymorphe.
#include <memory>

#include "cellsight/Cell.hpp"
#include "cellsight/Repository.hpp"
#include "cellsight/TestBench.hpp"
#include "helpers.hpp"
#include "minitest.hpp"

using namespace cs;

TEST(4, banc_basytec_se_presente) {
    std::unique_ptr<TestBench> bench = makeBench("BaSyTec", "B-2", "CTS", "Salle B-014", 8);
    CHECK(bench->vendor() == "BaSyTec");
    CHECK(bench->describe().rfind("BaSyTec CTS", 0) == 0);  // describe() appelle vendor()
}

TEST(4, chaque_banc_fabrique_son_importeur) {
    const ArbinBench arbin("B-1", "BT-2000", "", 16);
    const BasytecBench basytec("B-2", "CTS", "", 8);
    std::unique_ptr<DataImporter> a = arbin.createImporter();
    std::unique_ptr<DataImporter> b = basytec.createImporter();
    CHECK(dynamic_cast<ArbinImporter*>(a.get()) != nullptr);
    CHECK(dynamic_cast<BasytecImporter*>(b.get()) != nullptr);
}

TEST(4, mesures_absentes_avant_chargement) {
    std::shared_ptr<Cell> cell = CsvRepository(helpers::dataDir()).findCell("CS-NMC-001");
    CHECK(cell != nullptr);
    CHECK(!cell->hasData());
    CHECK_THROWS(cell->data(), std::logic_error);
}

TEST(4, chargement_polymorphe_de_toutes_les_cellules) {
    // Une seule boucle pour tous les bancs : c'est tout l'intérêt du polymorphisme.
    CsvRepository repo(helpers::dataDir());
    CHECK(repo.cells().size() == 4);
    for (const std::shared_ptr<Cell>& cell : repo.cells()) {
        cell->loadData(repo.dataDirectory());
        CHECK(cell->hasData());
        CHECK(cell->data().cycles().size() == helpers::truth(cell->id()).size());
    }
}
