// Étape 7 — export CSV.
#include <filesystem>
#include <fstream>
#include <sstream>

#include "cellsight/Exporter.hpp"
#include "helpers.hpp"
#include "minitest.hpp"

using namespace cs;
using helpers::point;

namespace {

std::vector<std::string> readLines(const std::string& path) {
    std::ifstream in(path);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) {
        lines.push_back(line);
    }
    return lines;
}

}  // namespace

TEST(7, nom_et_extension) {
    CHECK(!CsvExporter().name().empty());
    CHECK(CsvExporter().extension() == "csv");
}

TEST(7, ecriture_puis_relecture) {
    TimeSeries s;
    s.add(point(1.0, 1.25, 3.4567, 25.03, 1, 1));
    s.add(point(61.0, -2.5, 3.1234, 26.5, 2, 4));
    const std::string path = helpers::tempPath("export.csv");
    CsvExporter().write(s, path);

    const std::vector<std::string> lines = readLines(path);
    CHECK(lines.size() == 3);
    CHECK(lines[0] == "time_s,current_A,voltage_V,temperature_C,cycle,step");

    std::istringstream row(lines[2]);
    double t, i, v, temp;
    int cycle, step;
    char comma;
    row >> t >> comma >> i >> comma >> v >> comma >> temp >> comma >> cycle >> comma >> step;
    CHECK_NEAR(t, 61.0, 1e-9);
    CHECK_NEAR(i, -2.5, 1e-9);
    CHECK_NEAR(v, 3.1234, 1e-9);
    CHECK_NEAR(temp, 26.5, 1e-9);
    CHECK(cycle == 2);
    CHECK(step == 4);
}

TEST(7, export_d_un_cycle_reel) {
    const TimeSeries cycle = helpers::cellData("CS-NMC-001").cycle(10);
    const std::string path = helpers::tempPath("cycle10.csv");
    CsvExporter().write(cycle, path);
    CHECK(readLines(path).size() == cycle.size() + 1);
}

TEST(7, dossier_inexistant_leve_runtime_error) {
    const std::string path = helpers::tempPath("dossier_inexistant/x.csv");
    std::filesystem::remove_all(helpers::tempPath("dossier_inexistant"));
    CHECK_THROWS(CsvExporter().write(TimeSeries(), path), std::runtime_error);
}
