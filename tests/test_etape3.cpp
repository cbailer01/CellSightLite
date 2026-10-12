// Étape 3 — importeur BaSyTec.
#include <fstream>

#include "cellsight/DataImporter.hpp"
#include "helpers.hpp"
#include "minitest.hpp"

using namespace cs;

namespace {

const std::string kFile = "/raw/basytec/CS-NMC-002.txt";

// Nombre de lignes de données (ni vides, ni '~') : compté indépendamment de l'importeur.
std::size_t countDataLines(const std::string& path) {
    std::ifstream in(path);
    std::size_t n = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line[0] != '~') {
            ++n;
        }
    }
    return n;
}

}  // namespace

TEST(3, nom_du_format) {
    CHECK(!BasytecImporter().formatName().empty());
    CHECK(BasytecImporter().formatName() != ArbinImporter().formatName());
}

TEST(3, fichier_absent_leve_runtime_error) {
    CHECK_THROWS(BasytecImporter().load(helpers::dataDir() + "/raw/basytec/absent.txt"),
                 std::runtime_error);
}

TEST(3, une_mesure_par_ligne_de_donnees) {
    const std::string path = helpers::dataDir() + kFile;
    CHECK(BasytecImporter().load(path).size() == countDataLines(path));
}

TEST(3, premiere_mesure_convertie_en_unites_si) {
    const TimeSeries s = BasytecImporter().load(helpers::dataDir() + kFile);
    CHECK(!s.empty());
    const Measurement& m = s[0];
    CHECK_NEAR(m.time_s, 1.0, 0.01);       // 1 s, et non 0.000278 h
    CHECK_NEAR(m.current_A, 1.25, 0.01);   // charge à C/2 : courant positif
    CHECK(m.voltage_V > 2.5 && m.voltage_V < 4.2);
    CHECK_NEAR(m.temperature_C, 45.0, 1.0);  // cellule en enceinte à 45 °C
    CHECK(m.cycle == 1);
    CHECK(m.step == 1);
}

TEST(3, temps_croissant) {
    const TimeSeries s = BasytecImporter().load(helpers::dataDir() + kFile);
    for (std::size_t k = 1; k < s.size(); ++k) {
        CHECK(s[k].time_s > s[k - 1].time_s);
    }
}

TEST(3, colonnes_reperees_par_leur_nom) {
    // Colonnes dans un autre ordre, fins de ligne Windows, et deux lignes '~' qui
    // ressemblent à un en-tête : seule la DERNIÈRE donne les noms de colonnes.
    const std::string path = helpers::tempPath("basytec_reordonne.txt");
    {
        std::ofstream out(path, std::ios::binary);
        out << "~Resultfile from CTS\r\n"
               "~Time[h];DataSet;Command;U[V];I[A];T1[degC];Cyc-Count;Line\r\n";
        out << "~Line;Cyc-Count;I[A];U[V];T1[degC];Time[h];Command;DataSet\r\n"
               "4;7;-2.5000;3.6500;26.10;2.0000000;Discharge;1\r\n"
               "5;8;0.0000;3.7000;25.00;3.5000000;Pause;2\r\n";
    }
    const TimeSeries s = BasytecImporter().load(path);
    CHECK(s.size() == 2);
    CHECK_NEAR(s[0].time_s, 7200.0, 1e-6);
    CHECK_NEAR(s[0].current_A, -2.5, 1e-9);
    CHECK_NEAR(s[0].voltage_V, 3.65, 1e-9);
    CHECK_NEAR(s[0].temperature_C, 26.1, 1e-9);
    CHECK(s[0].cycle == 7);
    CHECK(s[0].step == 4);
    CHECK_NEAR(s[1].time_s, 12600.0, 1e-6);
}

TEST(3, autant_de_cycles_que_la_verite_terrain) {
    const TimeSeries s = BasytecImporter().load(helpers::dataDir() + kFile);
    CHECK(s.cycles().size() == helpers::truth("CS-NMC-002").size());
}
