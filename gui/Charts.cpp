#include "Charts.hpp"

#include <QLineSeries>
#include <QValueAxis>
#include <algorithm>
#include <cmath>

namespace charts {

namespace {
constexpr std::size_t kMaxPoints = 5000;
}

QChart* line(const std::vector<double>& x, const std::vector<double>& y, const QString& title,
             const QString& xTitle, const QString& yTitle, bool showPoints) {
    auto* series = new QLineSeries;
    const std::size_t n = std::min(x.size(), y.size());
    const std::size_t stride = n > kMaxPoints ? n / kMaxPoints + 1 : 1;
    for (std::size_t k = 0; k < n; k += stride) {
        if (!std::isnan(x[k]) && !std::isnan(y[k])) {  // NaN = valeur non calculable
            series->append(x[k], y[k]);
        }
    }
    series->setPointsVisible(showPoints);

    auto* chart = new QChart;
    chart->addSeries(series);  // le graphique devient propriétaire de la série
    chart->setTitle(title);
    chart->legend()->hide();

    auto* axisX = new QValueAxis;
    axisX->setTitleText(xTitle);
    auto* axisY = new QValueAxis;
    axisY->setTitleText(yTitle);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    axisX->applyNiceNumbers();  // graduations « rondes » : 0, 20, 40... plutôt que 30.8
    axisY->applyNiceNumbers();
    return chart;
}

QChart* message(const QString& text) {
    auto* chart = new QChart;
    chart->setTitle(text);
    chart->legend()->hide();
    return chart;
}

void replace(QChartView* view, QChart* chart) {
    QChart* old = view->chart();
    view->setChart(chart);
    delete old;
}

}  // namespace charts
