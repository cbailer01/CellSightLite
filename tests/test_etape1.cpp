// Étape 1 — classes métier : Datasheet, Cell, User.
#include <memory>

#include "cellsight/Cell.hpp"
#include "cellsight/Datasheet.hpp"
#include "cellsight/User.hpp"
#include "minitest.hpp"

using namespace cs;

namespace {

OperatingLimits nmcLimits() {
    OperatingLimits lim;
    lim.minVoltage_V = 2.5;
    lim.maxVoltage_V = 4.2;
    lim.maxChargeCurrent_A = 2.5;
    lim.maxDischargeCurrent_A = 10.0;
    lim.minTemperature_C = 0.0;
    lim.maxTemperature_C = 60.0;
    return lim;
}

Datasheet makeDatasheet(double capacity = 2.5, OperatingLimits lim = nmcLimits(),
                        const std::string& id = "DS-TEST") {
    return Datasheet(id, "Voltacell", "VC-TEST", Chemistry::NMC, "18650", capacity, 3.6, lim,
                     "datasheet.pdf");
}

std::shared_ptr<const TestBench> bench16() {
    return std::make_shared<ArbinBench>("BENCH-T", "BT-2000", "Salle test", 16);
}

std::shared_ptr<const TestPlan> plan() {
    return std::make_shared<TestPlan>("TP-T", "Plan de test", 1);
}

}  // namespace

TEST(1, datasheet_conserve_ses_valeurs) {
    const Datasheet ds = makeDatasheet();
    CHECK(ds.id() == "DS-TEST");
    CHECK(ds.chemistry() == Chemistry::NMC);
    CHECK_NEAR(ds.nominalCapacity_Ah(), 2.5, 1e-12);
    CHECK_NEAR(ds.limits().maxVoltage_V, 4.2, 1e-12);
}

TEST(1, datasheet_refuse_une_capacite_nulle_ou_negative) {
    CHECK_THROWS(makeDatasheet(0.0), std::invalid_argument);
    CHECK_THROWS(makeDatasheet(-1.0), std::invalid_argument);
}

TEST(1, datasheet_refuse_une_fenetre_de_tension_inversee) {
    OperatingLimits lim = nmcLimits();
    lim.minVoltage_V = 4.3;
    CHECK_THROWS(makeDatasheet(2.5, lim), std::invalid_argument);
}

TEST(1, datasheet_refuse_un_courant_max_nul) {
    OperatingLimits lim = nmcLimits();
    lim.maxChargeCurrent_A = 0.0;
    CHECK_THROWS(makeDatasheet(2.5, lim), std::invalid_argument);
}

TEST(1, datasheet_refuse_un_identifiant_vide) {
    CHECK_THROWS(makeDatasheet(2.5, nmcLimits(), ""), std::invalid_argument);
}

TEST(1, tension_dans_la_plage_bornes_incluses) {
    const Datasheet ds = makeDatasheet();
    CHECK(ds.voltageInRange(3.7));
    CHECK(ds.voltageInRange(2.5));
    CHECK(ds.voltageInRange(4.2));
    CHECK(!ds.voltageInRange(4.21));
    CHECK(!ds.voltageInRange(2.49));
}

TEST(1, regime_c_vers_courant) {
    const Datasheet ds = makeDatasheet();
    CHECK_NEAR(ds.cRateToCurrent(0.5), 1.25, 1e-12);
    CHECK_NEAR(ds.cRateToCurrent(2.0), 5.0, 1e-12);
}

TEST(1, cellule_valide) {
    auto ds = std::make_shared<const Datasheet>(makeDatasheet());
    const Cell cell("CS-T-001", ds, bench16(), 3, plan(), "EC-1", "2026-01-12", "raw/x.csv");
    CHECK(cell.id() == "CS-T-001");
    CHECK(cell.channel() == 3);
    CHECK(&cell.datasheet() == ds.get());  // partagée, pas copiée
}

TEST(1, cellule_refuse_une_voie_absente_du_banc) {
    auto ds = std::make_shared<const Datasheet>(makeDatasheet());
    CHECK_THROWS(Cell("CS-T", ds, bench16(), 17, plan(), "", "", ""), std::invalid_argument);
    CHECK_THROWS(Cell("CS-T", ds, bench16(), 0, plan(), "", "", ""), std::invalid_argument);
}

TEST(1, cellule_refuse_une_fiche_absente) {
    CHECK_THROWS(Cell("CS-T", nullptr, bench16(), 1, plan(), "", "", ""), std::invalid_argument);
}

TEST(1, cellule_refuse_un_identifiant_vide) {
    auto ds = std::make_shared<const Datasheet>(makeDatasheet());
    CHECK_THROWS(Cell("", ds, bench16(), 1, plan(), "", "", ""), std::invalid_argument);
}

TEST(1, permissions_selon_le_role) {
    const User viewer("v", "Visiteur", Role::Viewer);
    const User researcher("r", "Doctorant", Role::Researcher);
    const User admin("a", "Responsable", Role::Admin);

    CHECK(viewer.can(Permission::ViewData));
    CHECK(!viewer.can(Permission::ExportData));
    CHECK(!viewer.can(Permission::EditCatalog));

    CHECK(researcher.can(Permission::ViewData));
    CHECK(researcher.can(Permission::ExportData));
    CHECK(!researcher.can(Permission::EditCatalog));

    CHECK(admin.can(Permission::ViewData));
    CHECK(admin.can(Permission::ExportData));
    CHECK(admin.can(Permission::EditCatalog));
}
