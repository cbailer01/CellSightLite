// Étape 6 — état de charge par comptage coulombien, conformité des plans de test.
#include <algorithm>

#include "cellsight/Estimators.hpp"
#include "cellsight/TestPlan.hpp"
#include "helpers.hpp"
#include "minitest.hpp"

using namespace cs;

namespace {

const Datasheet& datasheet(const std::string& id) {
    return *helpers::repository().findDatasheet(id);
}

const TestPlan& plan(const std::string& id) {
    return *helpers::repository().findPlan(id);
}

// Indice du DERNIER point d'une étape donnée d'un cycle donné.
std::size_t lastIndexOf(const TimeSeries& s, int cycle, int step) {
    std::size_t found = s.size();
    for (std::size_t k = 0; k < s.size(); ++k) {
        if (s[k].cycle == cycle && s[k].step == step) {
            found = k;
        }
    }
    return found;
}

}  // namespace

TEST(6, soc_une_valeur_par_mesure_entre_0_et_100) {
    const TimeSeries s = helpers::cellData("CS-NMC-001").cycleRange(1, 5);
    const std::vector<double> soc = CoulombCountingSoc(30.0).estimate(s, datasheet("DS-VC18650-25N"));
    CHECK(soc.size() == s.size());
    CHECK(std::all_of(soc.begin(), soc.end(), [](double v) { return v >= 0.0 && v <= 100.0; }));
    CHECK_NEAR(soc.front(), 30.0, 1e-9);
}

TEST(6, soc_recale_a_100_en_fin_de_charge) {
    const TimeSeries s = helpers::cellData("CS-NMC-001").cycleRange(59, 60);
    const std::vector<double> soc = CoulombCountingSoc().estimate(s, datasheet("DS-VC18650-25N"));
    const std::size_t endOfCharge = lastIndexOf(s, 60, 2);  // fin de l'étape CV
    CHECK(endOfCharge < s.size());
    CHECK_NEAR(soc[endOfCharge], 100.0, 1e-6);
}

TEST(6, soc_en_fin_de_decharge) {
    const TimeSeries s = helpers::cellData("CS-NMC-001").cycleRange(59, 60);
    const std::vector<double> soc = CoulombCountingSoc().estimate(s, datasheet("DS-VC18650-25N"));
    const double discharged = helpers::truthAt("CS-NMC-001", 60).qDischarge_Ah;
    const double expected = std::max(0.0, 100.0 - 100.0 * discharged / 2.5);
    CHECK_NEAR(soc[lastIndexOf(s, 60, 4)], expected, 1.0);
}

TEST(6, soc_serie_vide) {
    CHECK(CoulombCountingSoc().estimate(TimeSeries(), datasheet("DS-VC18650-25N")).empty());
}

TEST(6, plans_du_laboratoire_conformes) {
    CHECK(plan("TP-CYC-NMC-1C").checkAgainst(datasheet("DS-VC18650-25N")).empty());
    CHECK(plan("TP-CYC-LFP-1C").checkAgainst(datasheet("DS-FL26650-30")).empty());
    CHECK(plan("TP-CYC-LFP-2C").checkAgainst(datasheet("DS-FL26650-30")).empty());
}

TEST(6, plan_de_charge_rapide_refuse) {
    // 3C = 7.5 A > 2.5 A en charge, et 4.3 V > 4.2 V aux étapes CC et CV.
    CHECK(plan("TP-FAST-NMC-3C").checkAgainst(datasheet("DS-VC18650-25N")).size() == 3);
}

TEST(6, plan_nmc_sur_cellule_lfp_refuse) {
    // 4.2 V > 3.65 V, en CC comme en CV.
    CHECK(plan("TP-CYC-NMC-1C").checkAgainst(datasheet("DS-FL26650-30")).size() == 2);
}

TEST(6, repos_de_duree_nulle_refuse) {
    TestPlan p("TP-T", "Repos nul", 1);
    Step rest;
    rest.type = StepType::Rest;
    rest.duration_s = 0.0;
    p.addStep(rest);
    CHECK(p.checkAgainst(datasheet("DS-VC18650-25N")).size() == 1);
}
