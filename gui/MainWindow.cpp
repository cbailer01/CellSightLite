#include "MainWindow.hpp"

#include <QAction>
#include <QApplication>
#include <QChartView>
#include <QComboBox>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QSplitter>
#include <QStatusBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QUrl>
#include <QVBoxLayout>
#include <algorithm>
#include <cmath>
#include <functional>

#include "Charts.hpp"
#include "cellsight/Estimators.hpp"
#include "cellsight/Exporter.hpp"
#include "cellsight/NotImplemented.hpp"

namespace {

QString qs(const std::string& s) {
    return QString::fromStdString(s);
}

QString number(double value, int decimals) {
    return QString::number(value, 'f', decimals);
}

// Chemin utilisable par std::ofstream. Sous Windows, un chemin accentué doit être
// converti dans le jeu de caractères du système, pas en UTF-8.
std::string nativePath(const QString& path) {
    return QFile::encodeName(path).toStdString();
}

QString todo(const cs::NotImplemented& e) {
    return QString("Étape %1 à implémenter : %2").arg(e.etape()).arg(qs(e.fonction()));
}

// Exécute f ; si le code appelé n'est pas encore écrit, renvoie un texte d'attente.
QString orTodo(const std::function<std::string()>& f) {
    try {
        return qs(f());
    } catch (const cs::NotImplemented& e) {
        return QString("(étape %1 à implémenter)").arg(e.etape());
    }
}

enum MeasureChoice { kVoltage, kCurrent, kTemperature, kSoc };

}  // namespace

MainWindow::MainWindow(cs::CellRepository& repository, cs::User user, QWidget* parent)
    : QMainWindow(parent),
      repository_(repository),
      user_(std::move(user)),
      cells_(repository.cells()),
      analyzer_(cs::CycleAnalyzer::withDefaultMetrics()) {
    setWindowTitle(QString("CellSight Lite — %1 (%2)")
                       .arg(qs(user_.fullName()), qs(cs::toString(user_.role()))));
    resize(1200, 760);

    auto* tabs = new QTabWidget;
    tabs->addTab(buildInfoTab(), "Fiche cellule");
    tabs->addTab(buildMeasuresTab(), "Mesures");
    tabs->addTab(buildMetricsTab(), "Indicateurs");

    auto* splitter = new QSplitter;
    splitter->addWidget(buildCellList());
    splitter->addWidget(tabs);
    splitter->setStretchFactor(1, 1);
    setCentralWidget(splitter);

    buildMenus();
    buildConnections();
    applyPermissions();

    statusBar()->showMessage(QString("%1 cellules — données : %2")
                                 .arg(cells_.size())
                                 .arg(qs(repository_.dataDirectory())));
    if (!cells_.empty()) {
        cellList_->setCurrentRow(0);
    }
}

// --- Construction de l'interface ---------------------------------------------

QWidget* MainWindow::buildCellList() {
    filter_ = new QLineEdit;
    filter_->setPlaceholderText("Filtrer : NMC, Arbin, CS-LFP...");
    cellList_ = new QListWidget;
    for (const auto& cell : cells_) {
        cellList_->addItem(QString("%1   %2   %3")
                               .arg(qs(cell->id()))
                               .arg(qs(cs::toString(cell->datasheet().chemistry())))
                               .arg(orTodo([&cell] { return cell->bench().vendor(); })));
    }

    auto* box = new QWidget;
    auto* layout = new QVBoxLayout(box);
    layout->addWidget(new QLabel("<b>Cellules</b>"));
    layout->addWidget(filter_);
    layout->addWidget(cellList_);
    return box;
}

QWidget* MainWindow::buildInfoTab() {
    auto makeLabel = [] {
        auto* label = new QLabel("—");
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        return label;
    };
    idLabel_ = makeLabel();
    datasheetLabel_ = makeLabel();
    chemistryLabel_ = makeLabel();
    capacityLabel_ = makeLabel();
    voltageLabel_ = makeLabel();
    benchLabel_ = makeLabel();
    channelLabel_ = makeLabel();
    locationLabel_ = makeLabel();
    startLabel_ = makeLabel();

    auto* form = new QFormLayout;
    form->addRow("Identifiant :", idLabel_);
    form->addRow("Modèle :", datasheetLabel_);
    form->addRow("Chimie :", chemistryLabel_);
    form->addRow("Capacité nominale :", capacityLabel_);
    form->addRow("Plage de tension :", voltageLabel_);
    form->addRow("Banc de cyclage :", benchLabel_);
    form->addRow("Voie :", channelLabel_);
    form->addRow("Emplacement :", locationLabel_);
    form->addRow("Début de l'essai :", startLabel_);
    auto* identity = new QGroupBox("Identité de la cellule");
    identity->setLayout(form);

    datasheetButton_ = new QPushButton("Ouvrir la fiche technique (PDF)");

    planLabel_ = makeLabel();
    planCheckLabel_ = makeLabel();
    planCheckLabel_->setWordWrap(true);
    planTable_ = new QTableWidget(0, 1);
    planTable_->setHorizontalHeaderLabels({"Étapes du plan"});
    planTable_->horizontalHeader()->setStretchLastSection(true);
    planTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auto* planBox = new QGroupBox("Plan de test");
    auto* planLayout = new QVBoxLayout(planBox);
    planLayout->addWidget(planLabel_);
    planLayout->addWidget(planTable_);
    planLayout->addWidget(planCheckLabel_);

    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->addWidget(identity);
    layout->addWidget(datasheetButton_, 0, Qt::AlignLeft);
    layout->addWidget(planBox, 1);
    return page;
}

QWidget* MainWindow::buildMeasuresTab() {
    quantityCombo_ = new QComboBox;
    quantityCombo_->addItems(
        {"Tension (V)", "Courant (A)", "Température (°C)", "État de charge SOC (%)"});
    firstCycle_ = new QSpinBox;
    lastCycle_ = new QSpinBox;
    measureView_ = new QChartView(charts::message("Sélectionnez une cellule"));
    measureView_->setRenderHint(QPainter::Antialiasing);

    auto* controls = new QHBoxLayout;
    controls->addWidget(new QLabel("Grandeur :"));
    controls->addWidget(quantityCombo_);
    controls->addSpacing(20);
    controls->addWidget(new QLabel("Cycles de"));
    controls->addWidget(firstCycle_);
    controls->addWidget(new QLabel("à"));
    controls->addWidget(lastCycle_);
    controls->addStretch();

    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->addLayout(controls);
    layout->addWidget(measureView_, 1);
    return page;
}

QWidget* MainWindow::buildMetricsTab() {
    metricCombo_ = new QComboBox;
    for (const std::string& name : analyzer_.metricNames()) {
        metricCombo_->addItem(qs(name));
    }
    metricView_ = new QChartView(charts::message("Sélectionnez une cellule"));
    metricView_->setRenderHint(QPainter::Antialiasing);
    metricTable_ = new QTableWidget;
    metricTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    metricTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    rulLabel_ = new QLabel;

    auto* controls = new QHBoxLayout;
    controls->addWidget(new QLabel("Indicateur :"));
    controls->addWidget(metricCombo_);
    controls->addStretch();

    auto* split = new QSplitter(Qt::Vertical);
    split->addWidget(metricView_);
    split->addWidget(metricTable_);
    split->setStretchFactor(0, 3);
    split->setStretchFactor(1, 2);

    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->addLayout(controls);
    layout->addWidget(split, 1);
    layout->addWidget(rulLabel_);
    return page;
}

void MainWindow::buildMenus() {
    QMenu* file = menuBar()->addMenu("&Fichier");
    exportCsvAction_ = file->addAction("Exporter les cycles affichés en CSV…");
    exportMatAction_ = file->addAction("Exporter toutes les mesures en MATLAB (.mat)…");
    exportMetricsAction_ = file->addAction("Exporter les indicateurs en CSV…");
    exportPngAction_ = file->addAction("Exporter le graphique en PNG…");
    file->addSeparator();
    QAction* quit = file->addAction("Quitter");
    connect(quit, &QAction::triggered, this, &QWidget::close);

    QMenu* help = menuBar()->addMenu("&Aide");
    QAction* about = help->addAction("À propos de CellSight Lite");
    connect(about, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::buildConnections() {
    connect(filter_, &QLineEdit::textChanged, this, &MainWindow::onFilterChanged);
    connect(cellList_, &QListWidget::currentRowChanged, this, &MainWindow::onCellSelected);
    connect(quantityCombo_, &QComboBox::currentIndexChanged, this,
            &MainWindow::updateMeasureChart);
    connect(firstCycle_, &QSpinBox::valueChanged, this, &MainWindow::updateMeasureChart);
    connect(lastCycle_, &QSpinBox::valueChanged, this, &MainWindow::updateMeasureChart);
    connect(exportMatAction_, &QAction::triggered, this, &MainWindow::onExportMat);
    connect(exportMetricsAction_, &QAction::triggered, this, &MainWindow::onExportMetrics);
    connect(exportPngAction_, &QAction::triggered, this, &MainWindow::onExportChartPng);

    // TODO Étape 8 — MainWindow::buildConnections : à écrire (voir le sujet).
    // Indice : Sur le modèle des lignes ci-dessus, connecter :
    //            - le choix d'indicateur (metricCombo_)      -> updateMetricChart
    //            - l'action « Exporter en CSV » (exportCsvAction_) -> onExportCsv
    //            - le bouton de la fiche (datasheetButton_)   -> onOpenDatasheet
}

void MainWindow::applyPermissions() {
    bool canExport = true;
    try {
        canExport = user_.can(cs::Permission::ExportData);
    } catch (const cs::NotImplemented&) {
        // Étape 1 pas encore faite : on laisse tout ouvert.
    }
    for (QAction* action : {exportCsvAction_, exportMatAction_, exportMetricsAction_,
                            exportPngAction_}) {
        action->setEnabled(canExport);
    }
}

// --- Sélection d'une cellule -------------------------------------------------

void MainWindow::onFilterChanged(const QString& text) {
    for (int i = 0; i < cellList_->count(); ++i) {
        QListWidgetItem* item = cellList_->item(i);
        item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
    }
}

void MainWindow::onCellSelected(int row) {
    if (row < 0 || row >= static_cast<int>(cells_.size())) {
        return;
    }
    current_ = cells_[static_cast<std::size_t>(row)];
    showCellInfo(*current_);
    showPlan(*current_);
    metrics_ = cs::MetricTable();

    if (!ensureLoaded(*current_)) {
        showMetricTable();
        return;
    }

    try {
        const std::vector<int> cycles = current_->data().cycles();
        if (!cycles.empty()) {
            // Bloquer les signaux : sinon chaque setRange/setValue redessine le graphique.
            const QSignalBlocker blockFirst(firstCycle_);
            const QSignalBlocker blockLast(lastCycle_);
            firstCycle_->setRange(cycles.front(), cycles.back());
            lastCycle_->setRange(cycles.front(), cycles.back());
            firstCycle_->setValue(cycles.front());
            lastCycle_->setValue(std::min(cycles.front() + 2, cycles.back()));
        }
        metrics_ = analyzer_.analyze(current_->data(), current_->datasheet());
    } catch (const cs::NotImplemented& e) {
        metrics_.warnings.push_back(e.what());
        statusBar()->showMessage(todo(e));
    }

    updateMeasureChart();
    updateMetricChart();
    showMetricTable();
}

bool MainWindow::ensureLoaded(cs::Cell& cell) {
    if (cell.hasData()) {
        return true;
    }
    QString error;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    try {
        cell.loadData(repository_.dataDirectory());
    } catch (const cs::NotImplemented& e) {
        error = todo(e);
    } catch (const std::exception& e) {
        error = "Lecture impossible : " + qs(e.what());
    }
    QApplication::restoreOverrideCursor();

    if (!error.isEmpty()) {
        charts::replace(measureView_, charts::message(error));
        charts::replace(metricView_, charts::message(error));
        statusBar()->showMessage(error);
        return false;
    }
    statusBar()->showMessage(
        QString("%1 : %2 mesures chargées").arg(qs(cell.id())).arg(cell.data().size()));
    return true;
}

void MainWindow::showCellInfo(const cs::Cell& cell) {
    const cs::Datasheet& ds = cell.datasheet();
    idLabel_->setText(qs(cell.id()));
    datasheetLabel_->setText(qs(ds.displayName()));
    capacityLabel_->setText(number(ds.nominalCapacity_Ah(), 2) + " Ah");
    startLabel_->setText(qs(cell.startDate()));

    // TODO Étape 8 — MainWindow::showCellInfo : à écrire (voir le sujet).
    // Indice : Compléter chemistryLabel_, voltageLabel_ (« 2.50 V – 4.20 V »), benchLabel_
    //          (TestBench::describe), channelLabel_ et locationLabel_, comme ci-dessus.
}

void MainWindow::showPlan(const cs::Cell& cell) {
    const cs::TestPlan& plan = cell.plan();
    planLabel_->setText(QString("<b>%1</b> — %2 (%3 répétitions)")
                            .arg(qs(plan.id()), qs(plan.name()))
                            .arg(plan.repetitions()));
    planTable_->setRowCount(static_cast<int>(plan.steps().size()));
    for (std::size_t i = 0; i < plan.steps().size(); ++i) {
        planTable_->setItem(static_cast<int>(i), 0,
                            new QTableWidgetItem(qs(plan.steps()[i].describe())));
    }

    try {
        const std::vector<std::string> violations = plan.checkAgainst(cell.datasheet());
        if (violations.empty()) {
            planCheckLabel_->setText(
                "<span style='color:#1a7f37'>✔ Plan conforme à la fiche technique</span>");
        } else {
            QString text = "<span style='color:#c62828'>✘ Plan NON conforme :</span><ul>";
            for (const std::string& v : violations) {
                text += "<li>" + qs(v).toHtmlEscaped() + "</li>";
            }
            planCheckLabel_->setText(text + "</ul>");
        }
    } catch (const cs::NotImplemented& e) {
        planCheckLabel_->setText(
            QString("Vérification du plan : étape %1 à implémenter").arg(e.etape()));
    }
}

// --- Graphiques et tableau ---------------------------------------------------

void MainWindow::updateMeasureChart() {
    if (!current_ || !current_->hasData()) {
        return;
    }
    try {
        const cs::TimeSeries range =
            current_->data().cycleRange(firstCycle_->value(), lastCycle_->value());
        if (range.empty()) {
            charts::replace(measureView_, charts::message("Aucune mesure pour ces cycles"));
            return;
        }
        std::vector<double> hours = range.values(cs::Quantity::Time);
        const double t0 = hours.front();
        for (double& t : hours) {
            t = (t - t0) / 3600.0;
        }

        std::vector<double> y;
        switch (quantityCombo_->currentIndex()) {
            case kVoltage: y = range.values(cs::Quantity::Voltage); break;
            case kCurrent: y = range.values(cs::Quantity::Current); break;
            case kTemperature: y = range.values(cs::Quantity::Temperature); break;
            default: y = cs::CoulombCountingSoc().estimate(range, current_->datasheet()); break;
        }
        const QString title = QString("%1 — cycles %2 à %3")
                                  .arg(qs(current_->id()))
                                  .arg(firstCycle_->value())
                                  .arg(lastCycle_->value());
        charts::replace(measureView_,
                        charts::line(hours, y, title, "Temps depuis le début de la sélection (h)",
                                     quantityCombo_->currentText()));
    } catch (const cs::NotImplemented& e) {
        charts::replace(measureView_, charts::message(todo(e)));
    }
}

void MainWindow::updateMetricChart() {
    const int m = metricCombo_->currentIndex();
    if (!current_ || m < 0 || m >= static_cast<int>(metrics_.names.size())) {
        charts::replace(metricView_,
                        charts::message(metrics_.warnings.empty()
                                            ? QString("Aucun indicateur")
                                            : qs(metrics_.warnings.front())));
        return;
    }
    const std::vector<double>& y = metrics_.values[static_cast<std::size_t>(m)];
    const QString name = qs(metrics_.names[static_cast<std::size_t>(m)]);
    const QString unit = qs(metrics_.units[static_cast<std::size_t>(m)]);

    if (std::all_of(y.begin(), y.end(), [](double v) { return std::isnan(v); })) {
        QString reason = name + " : non disponible";
        for (const std::string& w : metrics_.warnings) {
            if (qs(w).startsWith(name)) {
                reason = qs(w);
            }
        }
        charts::replace(metricView_, charts::message(reason));
        return;
    }
    const std::vector<double> x(metrics_.cycles.begin(), metrics_.cycles.end());
    charts::replace(metricView_, charts::line(x, y, name + " — " + qs(current_->id()), "Cycle",
                                              name + " (" + unit + ")", true));
}

void MainWindow::showMetricTable() {
    metricTable_->clear();
    metricTable_->setColumnCount(static_cast<int>(metrics_.names.size()));
    metricTable_->setRowCount(static_cast<int>(metrics_.cycles.size()));
    QStringList headers;
    for (std::size_t m = 0; m < metrics_.names.size(); ++m) {
        headers << qs(metrics_.names[m]) + "\n(" + qs(metrics_.units[m]) + ")";
    }
    metricTable_->setHorizontalHeaderLabels(headers);

    QStringList rows;
    for (std::size_t k = 0; k < metrics_.cycles.size(); ++k) {
        rows << QString("Cycle %1").arg(metrics_.cycles[k]);
        for (std::size_t m = 0; m < metrics_.names.size(); ++m) {
            const double v = metrics_.values[m][k];
            metricTable_->setItem(static_cast<int>(k), static_cast<int>(m),
                                  new QTableWidgetItem(std::isnan(v) ? "—" : number(v, 3)));
        }
    }
    metricTable_->setVerticalHeaderLabels(rows);

    // Durée de vie restante (bonus) à partir de la colonne SOH.
    const int soh = metrics_.indexOf("SOH");
    if (soh < 0 || metrics_.cycles.empty()) {
        rulLabel_->clear();
        return;
    }
    try {
        const cs::RulEstimate r = cs::LinearRulEstimator(80.0, 20).estimate(
            metrics_.cycles, metrics_.values[static_cast<std::size_t>(soh)]);
        rulLabel_->setText(
            r.valid ? QString("Durée de vie restante estimée (SOH 80 %, tendance des 20 derniers "
                              "cycles) : <b>%1 cycles</b> — fin de vie vers le cycle %2")
                          .arg(qRound(r.remainingCycles))
                          .arg(qRound(r.endOfLifeCycle))
                    : QString("Durée de vie restante : non estimable (SOH non décroissant)"));
    } catch (const cs::NotImplemented&) {
        rulLabel_->setText("Durée de vie restante : bonus à implémenter (LinearRulEstimator)");
    }
}

// --- Exports et fiche technique ----------------------------------------------

void MainWindow::onExportMat() {
    if (!current_ || !current_->hasData()) {
        QMessageBox::information(this, "Export", "Aucune mesure chargée.");
        return;
    }
    const QString path = QFileDialog::getSaveFileName(
        this, "Exporter en MATLAB", qs(current_->id()) + ".mat", "Fichiers MATLAB (*.mat)");
    if (path.isEmpty()) {
        return;  // l'utilisateur a annulé
    }
    try {
        cs::MatExporter().write(current_->data(), nativePath(path));
        statusBar()->showMessage("Mesures exportées dans " + path);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Export", "Échec de l'export :\n" + qs(e.what()));
    }
}

void MainWindow::onExportCsv() {
    // TODO Étape 8 — MainWindow::onExportCsv : à écrire (voir le sujet).
    // Indice : Comme onExportMat, mais en n'exportant QUE les cycles affichés dans l'onglet
    //          « Mesures » : cycleRange(firstCycle_->value(), lastCycle_->value()).
}

void MainWindow::onExportMetrics() {
    if (metrics_.cycles.empty()) {
        QMessageBox::information(this, "Export", "Aucun indicateur calculé.");
        return;
    }
    const QString path =
        QFileDialog::getSaveFileName(this, "Exporter les indicateurs",
                                     qs(current_->id()) + "_indicateurs.csv", "Fichiers CSV (*.csv)");
    if (path.isEmpty()) {
        return;
    }
    try {
        metrics_.writeCsv(nativePath(path));
        statusBar()->showMessage("Indicateurs exportés dans " + path);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Export", "Échec de l'export :\n" + qs(e.what()));
    }
}

void MainWindow::onOpenDatasheet() {
    // TODO Étape 8 — MainWindow::onOpenDatasheet : à écrire (voir le sujet).
    // Indice : Vérifier que le fichier existe (QFileInfo::exists), sinon QMessageBox::warning ;
    //          puis QDesktopServices::openUrl(QUrl::fromLocalFile(...)).
}

void MainWindow::onExportChartPng() {
    // TODO Étape 9 — MainWindow::onExportChartPng : à écrire (voir le sujet).
    // Indice : Bonus : QWidget::grab() renvoie une image (QPixmap) du widget ;
    //          QPixmap::save(chemin, "PNG") l'enregistre.
}

void MainWindow::onAbout() {
    QMessageBox::about(
        this, "À propos de CellSight Lite",
        "<h3>CellSight Lite</h3>"
        "<p>Visualisation des données de cyclage de cellules lithium-ion.</p>"
        "<p>Ce logiciel est la <b>partie C++</b> du projet <b>CellSight</b>. La plateforme "
        "complète du laboratoire y ajoute une base de données centrale, des modèles d'IA "
        "(SOC, SOH, RUL, détection d'anomalies), une ontologie du domaine et un assistant "
        "conversationnel.</p>"
        "<p><i>Données synthétiques — fabricants fictifs.</i></p>");
}
