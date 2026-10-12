// Bonus (étape 9) — durée de vie restante par régression linéaire du SOH.
#include "cellsight/CycleAnalyzer.hpp"
#include "cellsight/Estimators.hpp"
#include "helpers.hpp"
#include "minitest.hpp"

using namespace cs;

TEST(9, rul_sur_une_droite_exacte) {
    std::vector<int> cycles;
    std::vector<double> soh;
    for (int c = 1; c <= 50; ++c) {
        cycles.push_back(c);
        soh.push_back(100.0 - 0.2 * c);  // 80 % atteint au cycle 100
    }
    const RulEstimate r = LinearRulEstimator(80.0, 30).estimate(cycles, soh);
    CHECK(r.valid);
    CHECK_NEAR(r.slope_pctPerCycle, -0.2, 1e-9);
    CHECK_NEAR(r.endOfLifeCycle, 100.0, 1e-6);
    CHECK_NEAR(r.remainingCycles, 50.0, 1e-6);
}

TEST(9, rul_invalide_si_le_soh_ne_baisse_pas) {
    const RulEstimate r = LinearRulEstimator().estimate({1, 2, 3, 4}, {90, 91, 92, 93});
    CHECK(!r.valid);
}

TEST(9, rul_invalide_avec_moins_de_trois_points) {
    CHECK(!LinearRulEstimator().estimate({1, 2}, {95, 94}).valid);
}

TEST(9, rul_de_la_cellule_vieillie_a_45_degres) {
    const Datasheet& ds = *helpers::repository().findDatasheet("DS-VC18650-25N");
    const MetricTable t =
        CycleAnalyzer::withDefaultMetrics().analyze(helpers::cellData("CS-NMC-002"), ds);
    const std::vector<double>& soh = t.values.at(static_cast<std::size_t>(t.indexOf("SOH")));
    const RulEstimate r = LinearRulEstimator(80.0, 20).estimate(t.cycles, soh);
    CHECK(r.valid);
    CHECK(r.remainingCycles >= 0.0 && r.remainingCycles < 30.0);  // proche de la fin de vie
}
