#pragma once

#include <string>

namespace cs {

enum class Role { Viewer, Researcher, Admin };
enum class Permission { ViewData, ExportData, EditCatalog };

std::string toString(Role r);
Role roleFromString(const std::string& text);  // "VIEWER", "RESEARCHER", "ADMIN"

class User {
public:
    User(std::string login, std::string fullName, Role role);

    const std::string& login() const { return login_; }
    const std::string& fullName() const { return fullName_; }
    Role role() const { return role_; }

    // Étape 1 — Viewer : consulter ; Researcher : + exporter ; Admin : tout.
    bool can(Permission p) const;

private:
    std::string login_;
    std::string fullName_;
    Role role_;
};

}  // namespace cs
