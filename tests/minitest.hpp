#pragma once
// Mini-cadre de test, sans dépendance : TEST, CHECK, CHECK_NEAR, CHECK_REL, CHECK_THROWS.
// Un test qui rencontre cs::NotImplemented est compté « À FAIRE », pas « ÉCHEC ».

#include <cmath>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "cellsight/NotImplemented.hpp"

namespace minitest {

struct Failure : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct TestCase {
    int etape;
    std::string name;
    std::function<void()> body;
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> tests;
    return tests;
}

struct Registrar {
    Registrar(int etape, const char* name, std::function<void()> body) {
        registry().push_back({etape, name, std::move(body)});
    }
};

inline std::string where(const char* file, int line) {
    std::string path(file);
    const std::size_t slash = path.find_last_of("/\\");
    return (slash == std::string::npos ? path : path.substr(slash + 1)) + ":" +
           std::to_string(line);
}

}  // namespace minitest

#define TEST(etape, name)                                                   \
    static void name();                                                     \
    static ::minitest::Registrar name##_registrar(etape, #name, name);      \
    static void name()

#define CHECK(cond)                                                                       \
    do {                                                                                  \
        if (!(cond)) {                                                                    \
            throw ::minitest::Failure(::minitest::where(__FILE__, __LINE__) + " : " #cond); \
        }                                                                                 \
    } while (0)

#define CHECK_NEAR(actual, expected, tolerance)                                            \
    do {                                                                                   \
        const double a_ = (actual), e_ = (expected), t_ = (tolerance);                     \
        if (!(std::fabs(a_ - e_) <= t_)) {                                                 \
            std::ostringstream o_;                                                         \
            o_ << ::minitest::where(__FILE__, __LINE__) << " : " #actual " = " << a_       \
               << ", attendu " << e_ << " +/- " << t_;                                     \
            throw ::minitest::Failure(o_.str());                                           \
        }                                                                                  \
    } while (0)

// Tolérance relative : CHECK_REL(x, 2.5, 0.01) accepte 2.5 +/- 1 %.
#define CHECK_REL(actual, expected, relative) \
    CHECK_NEAR(actual, expected, std::fabs(expected) * (relative))

// NotImplemented est relancée : un « throw NotImplemented » ne doit pas passer
// pour l'exception attendue (c'est aussi une std::logic_error).
#define CHECK_THROWS(expr, ExceptionType)                                                 \
    do {                                                                                  \
        bool thrown_ = false;                                                             \
        try {                                                                             \
            (void)(expr);                                                                 \
        } catch (const ::cs::NotImplemented&) {                                           \
            throw;                                                                        \
        } catch (const ExceptionType&) {                                                  \
            thrown_ = true;                                                               \
        }                                                                                 \
        if (!thrown_) {                                                                   \
            throw ::minitest::Failure(::minitest::where(__FILE__, __LINE__) +             \
                                      " : " #expr " aurait du lever " #ExceptionType);    \
        }                                                                                 \
    } while (0)
