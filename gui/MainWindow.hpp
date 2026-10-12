#pragma once

#include <QMainWindow>
#include <memory>
#include <vector>

#include "cellsight/CycleAnalyzer.hpp"
#include "cellsight/Repository.hpp"
#include "cellsight/User.hpp"

class QAction;
class QChartView;
class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSpinBox;
class QTableWidget;

// Fenêtre principale : liste des cellules à gauche, trois onglets à droite
// (fiche, mesures, indicateurs), exports dans le menu « Fichier ».
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(cs::CellRepository& repository, cs::User user, QWidget* parent = nullptr);

private slots:
    void onFilterChanged(const QString& text);
    void onCellSelected(int row);
    void updateMeasureChart();
    void updateMetricChart();
    void onExportMat();       // fourni : le modèle à suivre
    void onExportCsv();       // Étape 8
    void onExportMetrics();
    void onOpenDatasheet();   // Étape 8
    void onExportChartPng();  // Bonus
    void onAbout();

private:
    QWidget* buildCellList();
    QWidget* buildInfoTab();
    QWidget* buildMeasuresTab();
    QWidget* buildMetricsTab();
    void buildMenus();
    void buildConnections();  // Étape 8 : trois connexions à ajouter

    void showCellInfo(const cs::Cell& cell);  // Étape 8 : compléter les champs
    void showPlan(const cs::Cell& cell);
    void showMetricTable();
    bool ensureLoaded(cs::Cell& cell);
    void applyPermissions();

    cs::CellRepository& repository_;
    cs::User user_;
    std::vector<std::shared_ptr<cs::Cell>> cells_;
    std::shared_ptr<cs::Cell> current_;
    cs::CycleAnalyzer analyzer_;
    cs::MetricTable metrics_;

    // Liste des cellules
    QLineEdit* filter_ = nullptr;
    QListWidget* cellList_ = nullptr;

    // Onglet « Fiche »
    QLabel* idLabel_ = nullptr;
    QLabel* datasheetLabel_ = nullptr;
    QLabel* chemistryLabel_ = nullptr;
    QLabel* capacityLabel_ = nullptr;
    QLabel* voltageLabel_ = nullptr;
    QLabel* benchLabel_ = nullptr;
    QLabel* channelLabel_ = nullptr;
    QLabel* locationLabel_ = nullptr;
    QLabel* startLabel_ = nullptr;
    QLabel* planLabel_ = nullptr;
    QLabel* planCheckLabel_ = nullptr;
    QTableWidget* planTable_ = nullptr;
    QPushButton* datasheetButton_ = nullptr;

    // Onglet « Mesures »
    QComboBox* quantityCombo_ = nullptr;
    QSpinBox* firstCycle_ = nullptr;
    QSpinBox* lastCycle_ = nullptr;
    QChartView* measureView_ = nullptr;

    // Onglet « Indicateurs »
    QComboBox* metricCombo_ = nullptr;
    QChartView* metricView_ = nullptr;
    QTableWidget* metricTable_ = nullptr;
    QLabel* rulLabel_ = nullptr;

    // Menu « Fichier »
    QAction* exportCsvAction_ = nullptr;
    QAction* exportMatAction_ = nullptr;
    QAction* exportMetricsAction_ = nullptr;
    QAction* exportPngAction_ = nullptr;
};
