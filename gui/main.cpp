#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <clocale>
#include <exception>
#include <memory>

#include "LoginDialog.hpp"
#include "MainWindow.hpp"
#include "cellsight/Repository.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("CellSight Lite");

    // Qt adopte la locale du système. Sous Linux en français, std::stod lirait
    // alors « 3,6 » et refuserait « 3.6 » : or les fichiers du cycleur utilisent
    // le point décimal. On rétablit la locale « C » pour les nombres.
    std::setlocale(LC_NUMERIC, "C");

    // Dossier de données : premier argument, sinon celui du projet.
    const QString dataDir = argc > 1 ? QString::fromLocal8Bit(argv[1]) : CELLSIGHT_DATA_DIR;

    std::unique_ptr<cs::CellRepository> repository;
    try {
        repository = std::make_unique<cs::CsvRepository>(QFile::encodeName(dataDir).toStdString());
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "CellSight Lite",
                              "Impossible de charger le catalogue de " + dataDir + " :\n" +
                                  QString::fromStdString(e.what()));
        return 1;
    }

    LoginDialog login(repository->users());
    if (login.exec() != QDialog::Accepted) {
        return 0;
    }

    MainWindow window(*repository, login.selectedUser());
    window.show();
    return app.exec();
}
