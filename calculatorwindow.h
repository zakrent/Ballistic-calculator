#ifndef CALCULATORWINDOW_H
#define CALCULATORWINDOW_H

#include "ballisticcalculator.h"

#include <QMainWindow>
#include <QtCharts>

QT_BEGIN_NAMESPACE
namespace Ui { class CalculatorWindow; }
QT_END_NAMESPACE

class CalculatorWindow : public QMainWindow
{
    Q_OBJECT

public:
    CalculatorWindow(QWidget *parent = nullptr);
    ~CalculatorWindow();

private slots:
    void on_calculate_clicked();

    void on_plotDistanceSlider_sliderMoved(int position);

    void on_plotDistanceSpinBox_valueChanged(double arg1);

    void on_Drop_stateChanged(int arg1);

    void on_checkBox_3_stateChanged(int arg1);

    void on_checkBox_4_stateChanged(int arg1);

    void on_checkBox_5_stateChanged(int arg1);

    void on_POI_stateChanged(int arg1);

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::CalculatorWindow *ui;
    BallisticCalculator *calculator;

    union
    {
        struct
        {
            QLineSeries *POISeries;
            QLineSeries *dropSeries;
            QLineSeries *speedSeries;
            QLineSeries *energySeries;
            QLineSeries *timeSeries;
            QLineSeries *windDeflectionSeries;
        };
        QLineSeries *series[6];
    };

    float plotDistance;

    void autoscaleYAxis();
};
#endif // CALCULATORWINDOW_H
