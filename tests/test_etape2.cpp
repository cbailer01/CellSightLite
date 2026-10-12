// Étape 2 — TimeSeries : découpage par cycle, durée, extrema.
#include "cellsight/TimeSeries.hpp"
#include "helpers.hpp"
#include "minitest.hpp"

using namespace cs;
using helpers::point;

namespace {

// Trois cycles : 1 (t = 0, 10, 20), 2 (t = 30, 40), 3 (t = 50).
TimeSeries threeCycles() {
    TimeSeries s;
    s.add(point(0, 1.0, 3.60, 25.0, 1));
    s.add(point(10, 1.0, 3.90, 25.5, 1));
    s.add(point(20, -2.0, 3.40, 27.0, 1));
    s.add(point(30, 1.0, 3.70, 26.0, 2));
    s.add(point(40, 0.0, 4.10, 25.2, 2));
    s.add(point(50, -1.5, 3.20, 25.8, 3));
    return s;
}

}  // namespace

TEST(2, cycles_distincts_et_tries) {
    CHECK(threeCycles().cycles() == (std::vector<int>{1, 2, 3}));

    TimeSeries unordered;  // ne pas supposer que les cycles arrivent dans l'ordre
    for (int c : {3, 1, 3, 2, 1}) {
        unordered.add(point(0, 0, 3.6, 25, c));
    }
    CHECK(unordered.cycles() == (std::vector<int>{1, 2, 3}));
    CHECK(TimeSeries().cycles().empty());
}

TEST(2, extraction_d_un_cycle) {
    const TimeSeries c2 = threeCycles().cycle(2);
    CHECK(c2.size() == 2);
    CHECK_NEAR(c2[0].time_s, 30.0, 1e-12);
    CHECK_NEAR(c2[1].time_s, 40.0, 1e-12);
    for (const Measurement& m : c2) {
        CHECK(m.cycle == 2);
    }
}

TEST(2, cycle_absent_donne_une_serie_vide) {
    CHECK(threeCycles().cycle(42).empty());
}

TEST(2, plage_de_cycles) {
    CHECK(threeCycles().cycleRange(2, 3).size() == 3);
    CHECK(threeCycles().cycleRange(1, 3).size() == 6);
    CHECK(threeCycles().cycleRange(3, 2).empty());
}

TEST(2, duree) {
    CHECK_NEAR(threeCycles().duration_s(), 50.0, 1e-12);
    CHECK_NEAR(threeCycles().cycle(2).duration_s(), 10.0, 1e-12);
    CHECK_NEAR(TimeSeries().duration_s(), 0.0, 1e-12);
}

TEST(2, minimum_et_maximum) {
    const TimeSeries s = threeCycles();
    CHECK_NEAR(s.max(Quantity::Voltage), 4.10, 1e-12);
    CHECK_NEAR(s.min(Quantity::Voltage), 3.20, 1e-12);
    CHECK_NEAR(s.min(Quantity::Current), -2.0, 1e-12);
    CHECK_NEAR(s.max(Quantity::Temperature), 27.0, 1e-12);
}

TEST(2, min_max_sur_serie_vide_levent_logic_error) {
    CHECK_THROWS(TimeSeries().max(Quantity::Voltage), std::logic_error);
    CHECK_THROWS(TimeSeries().min(Quantity::Voltage), std::logic_error);
}

TEST(2, donnees_reelles_cs_nmc_001) {
    const TimeSeries s = helpers::cellData("CS-NMC-001");
    const std::vector<int> cycles = s.cycles();
    CHECK(cycles.size() == helpers::truth("CS-NMC-001").size());
    CHECK(cycles.front() == 1);
    CHECK(s.max(Quantity::Voltage) < 4.21);
    CHECK(s.min(Quantity::Voltage) > 2.49);
    CHECK(s.cycle(10).duration_s() > 3600.0);  // un cycle dure plusieurs heures
}
