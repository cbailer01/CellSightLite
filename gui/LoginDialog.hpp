#pragma once

#include <QDialog>
#include <vector>

#include "cellsight/User.hpp"

class QComboBox;

// Choix de l'utilisateur au démarrage (pas de mot de passe : outil de laboratoire).
class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(std::vector<cs::User> users, QWidget* parent = nullptr);
    cs::User selectedUser() const;

private:
    std::vector<cs::User> users_;
    QComboBox* combo_;
};
