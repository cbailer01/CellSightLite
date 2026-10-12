// Étape 5 — intégration et indicateurs, comparés à la vérité terrain du générateur.
#include <cmath>

#include "cellsight/CycleAnalyzer.hpp"
#include "cellsight/Integration.hpp"
#include "cellsight/Metric.hpp"
#include "helpers.hpp"
#include "minitest.hpp"

using namespace cs;
using helpers::point;

namespace {

const std::string kCell = "CS-NMC-001";

const Datasheet& nmcDatasheet() {
    return *helpers::repository().findDatasheet("DS-VC18650-25N");
}

TimeSeries cycleOf(const std::string& id, int cycle) {
    return helpers::cellData(id).cycle(cycle);
}

}  // namespace

TEST(5, trapezes_cas_simples) {
    CHECK_NEAR(trapezoid({0, 1, 2, 3}, {2, 2, 2, 2}), 6.0, 1e-12);  // rectangle
    CHECK_NEAR(trapezoid({0, 2}, {0, 2}), 2.0, 1e-12);              // triangle
    CHECK_NEAR(trapezoid({0, 1, 3}, {1, 3, 3}), 8.0, 1e-12);        // pas irrégulier
    CHECK_NEAR(trapezoid({5}, {1}), 0.0, 1e-12);
    CHECK_NEAR(trapezoid({}, {}), 0.0, 1e-12);
}

TEST(5, trapezes_tailles_differentes) {
    CHECK_THROWS(trapezoid({0, 1, 2}, {1, 2}), std::invalid_argument);
}

TEST(5, charge_et_decharge_comptees_separement) {
    TimeSeries s;  // 1 h de charge à 1 A, puis 30 min de décharge à 2 A
    s.add(point(0, 1.0, 3.5, 25, 1));
    s.add(point(3600, 1.0, 4.1, 25, 1));
    s.add(point(3601, -2.0, 4.0, 25, 1));
    s.add(point(5401, -2.0, 3.0, 25, 1));
    CHECK_NEAR(charge_Ah(s, Direction::Charge), 1.0, 1e-3);
    CHECK_NEAR(charge_Ah(s, Direction::Discharge), 1.0, 1e-3);
}

TEST(5, energie_tension_constante) {
    TimeSeries s;  // 2 A sous 4 V pendant 1 h = 8 Wh
    s.add(point(0, -2.0, 4.0, 25, 1));
    s.add(point(3600, -2.0, 4.0, 25, 1));
    CHECK_NEAR(energy_Wh(s, Direction::Discharge), 8.0, 1e-9);
    CHECK_NEAR(energy_Wh(s, Direction::Charge), 0.0, 1e-9);
}

TEST(5, capacite_dechargee_vs_verite_terrain) {
    for (int c : {1, 10, 60, 120}) {
        const double expected = helpers::truthAt(kCell, c).qDischarge_Ah;
        CHECK_REL(DischargeCapacity().compute(cycleOf(kCell, c), nmcDatasheet()), expected, 0.005);
    }
}

TEST(5, energie_dechargee_vs_verite_terrain) {
    for (int c : {10, 60, 120}) {
        const double expected = helpers::truthAt(kCell, c).eDischarge_Wh;
        CHECK_REL(DischargeEnergy().compute(cycleOf(kCell, c), nmcDatasheet()), expected, 0.005);
    }
}

TEST(5, rendement_coulombique) {
    for (int c : {10, 60}) {
        const helpers::Truth t = helpers::truthAt(kCell, c);
        CHECK_NEAR(CoulombicEfficiency().compute(cycleOf(kCell, c), nmcDatasheet()),
                   100.0 * t.qDischarge_Ah / t.qCharge_Ah, 0.3);
    }
}

TEST(5, soh_rapporte_a_la_capacite_nominale) {
    const double expected = 100.0 * helpers::truthAt(kCell, 60).qDischarge_Ah / 2.5;
    CHECK_REL(StateOfHealth().compute(cycleOf(kCell, 60), nmcDatasheet()), expected, 0.005);
}

TEST(5, resistance_interne_vs_verite_terrain) {
    for (int c : {10, 60, 120}) {
        const double expected = helpers::truthAt(kCell, c).rDc_mOhm;
        CHECK_REL(InternalResistance().compute(cycleOf(kCell, c), nmcDatasheet()), expected, 0.05);
    }
}

TEST(5, resistance_interne_croit_avec_le_vieillissement) {
    const InternalResistance r;
    CHECK(r.compute(cycleOf(kCell, 120), nmcDatasheet()) >
          r.compute(cycleOf(kCell, 1), nmcDatasheet()));
}

TEST(5, resistance_nan_sans_passage_repos_decharge) {
    TimeSeries s;
    s.add(point(0, 1.0, 3.6, 25, 1));
    s.add(point(60, 1.0, 3.7, 25, 1));
    CHECK(std::isnan(InternalResistance().compute(s, nmcDatasheet())));
}

TEST(5, indicateurs_sur_fichier_basytec) {
    const Datasheet& lfp = *helpers::repository().findDatasheet("DS-FL26650-30");
    const double expected = helpers::truthAt("CS-LFP-002", 50).qDischarge_Ah;
    CHECK_REL(DischargeCapacity().compute(cycleOf("CS-LFP-002", 50), lfp), expected, 0.005);
}

TEST(5, analyse_complete_sans_avertissement) {
    const MetricTable table =
        CycleAnalyzer::withDefaultMetrics().analyze(helpers::cellData(kCell), nmcDatasheet());
    CHECK(table.warnings.empty());
    CHECK(table.cycles.size() == helpers::truth(kCell).size());
    CHECK(table.indexOf("SOH") >= 0);
}
