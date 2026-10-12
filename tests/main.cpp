// Lance les tests et affiche un bilan par étape.
//   cellsight_tests             toutes les étapes
//   cellsight_tests --etape 3   uniquement l'étape 3
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <map>
#include <string>

#include "minitest.hpp"

namespace {

const std::map<int, std::string> kTitles = {
    {1, "Classes metier"},        {2, "Series temporelles"}, {3, "Import BaSyTec"},
    {4, "Bancs et polymorphisme"}, {5, "Indicateurs"},        {6, "SOC et securite"},
    {7, "Export CSV"},            {9, "Bonus : RUL"},
};

struct Tally {
    int ok = 0;
    int failed = 0;
    int todo = 0;
};

}  // namespace

int main(int argc, char** argv) {
    int only = 0;
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::strcmp(argv[i], "--etape") == 0) {
            only = std::atoi(argv[i + 1]);
        }
    }

    std::vector<minitest::TestCase> tests = minitest::registry();
    std::stable_sort(tests.begin(), tests.end(),
                     [](const auto& a, const auto& b) { return a.etape < b.etape; });

    std::map<int, Tally> tally;
    for (const minitest::TestCase& t : tests) {
        if (only != 0 && t.etape != only) {
            continue;
        }
        std::string status = "OK     ";
        std::string detail;
        try {
            t.body();
            ++tally[t.etape].ok;
        } catch (const cs::NotImplemented& e) {
            status = "A FAIRE";
            detail = e.what();
            ++tally[t.etape].todo;
        } catch (const minitest::Failure& e) {
            status = "ECHEC  ";
            detail = e.what();
            ++tally[t.etape].failed;
        } catch (const std::exception& e) {
            status = "ECHEC  ";
            detail = std::string("exception inattendue : ") + e.what();
            ++tally[t.etape].failed;
        }
        std::printf("[%s] etape %d  %s\n", status.c_str(), t.etape, t.name.c_str());
        if (!detail.empty()) {
            std::printf("            -> %s\n", detail.c_str());
        }
    }

    std::printf("\n%-34s %8s\n", "Bilan", "reussis");
    bool allGood = true;
    for (const auto& [etape, n] : tally) {
        const int total = n.ok + n.failed + n.todo;
        const char* verdict = n.failed > 0 ? "ECHEC" : (n.todo > 0 ? "a faire" : "OK");
        const auto title = kTitles.find(etape);
        std::printf("Etape %d  %-25s %3d / %-3d %s\n", etape,
                    title == kTitles.end() ? "" : title->second.c_str(), n.ok, total, verdict);
        allGood = allGood && n.failed == 0 && n.todo == 0;
    }
    return allGood ? 0 : 1;
}
