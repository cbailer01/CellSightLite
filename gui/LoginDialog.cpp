#include "LoginDialog.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

LoginDialog::LoginDialog(std::vector<cs::User> users, QWidget* parent)
    : QDialog(parent), users_(std::move(users)), combo_(new QComboBox) {
    setWindowTitle("CellSight Lite — connexion");

    for (const cs::User& u : users_) {
        combo_->addItem(QString::fromStdString(u.fullName() + "  (" + u.login() + ", " +
                                               cs::toString(u.role()) + ")"));
    }

    auto* form = new QFormLayout;
    form->addRow("Utilisateur :", combo_);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("<b>CellSight Lite</b> — données de cyclage du laboratoire"));
    layout->addLayout(form);
    layout->addWidget(buttons);
}

cs::User LoginDialog::selectedUser() const {
    return users_.at(static_cast<std::size_t>(combo_->currentIndex()));
}
