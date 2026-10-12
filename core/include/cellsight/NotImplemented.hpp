#pragma once

#include <stdexcept>
#include <string>

namespace cs {

// Levée par toute fonction que le kit étudiant laisse à écrire.
// Les tests et l'interface graphique l'interceptent pour afficher
// « étape N à implémenter » au lieu de planter.
class NotImplemented : public std::logic_error {
public:
    NotImplemented(int etape, const std::string& fonction)
        : std::logic_error("Etape " + std::to_string(etape) + " a implementer : " + fonction),
          etape_(etape),
          fonction_(fonction) {}

    int etape() const noexcept { return etape_; }
    const std::string& fonction() const noexcept { return fonction_; }

private:
    int etape_;
    std::string fonction_;
};

}  // namespace cs
