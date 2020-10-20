#ifndef CALCULATORWINDOW_H
#define CALCULATORWINDOW_H

#include "ballisticcalculator.h"

#include <QMainWindow>

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

private:
    Ui::CalculatorWindow *ui;
    BallisticCalculator *calculator;
};
#endif // CALCULATORWINDOW_H
