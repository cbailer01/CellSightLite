#pragma once
// Petites fabriques de graphiques Qt Charts (fournies).

#include <QChart>
#include <QChartView>
#include <QString>
#include <vector>

namespace charts {

// Courbe y(x). Au-delà de quelques milliers de points, la courbe est
// sous-échantillonnée : l'écran n'a pas plus de pixels que cela.
QChart* line(const std::vector<double>& x, const std::vector<double>& y, const QString& title,
             const QString& xTitle, const QString& yTitle, bool showPoints = false);

// Graphique vide portant un message (« Étape 4 à implémenter », ...).
QChart* message(const QString& text);

// Remplace le graphique affiché. QChartView ne détruit pas l'ancien : on s'en charge.
void replace(QChartView* view, QChart* chart);

}  // namespace charts
