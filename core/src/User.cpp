#include "cellsight/User.hpp"

#include <stdexcept>
#include <utility>

#include "cellsight/NotImplemented.hpp"

namespace cs {

std::string toString(Role r) {
    switch (r) {
        case Role::Viewer: return "VIEWER";
        case Role::Researcher: return "RESEARCHER";
        case Role::Admin: return "ADMIN";
    }
    return "?";
}

Role roleFromString(const std::string& text) {
    for (Role r : {Role::Viewer, Role::Researcher, Role::Admin}) {
        if (toString(r) == text) {
            return r;
        }
    }
    throw std::invalid_argument("Role inconnu : " + text);
}

User::User(std::string login, std::string fullName, Role role)
    : login_(std::move(login)), fullName_(std::move(fullName)), role_(role) {}

bool User::can(Permission p) const {
    // TODO Étape 1 — User::can : à écrire (voir le sujet).
    throw NotImplemented(1, "User::can");
}

}  // namespace cs
