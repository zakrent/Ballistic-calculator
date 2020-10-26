#include "calculatorwindow.h"
#include "ui_calculatorwindow.h"

CalculatorWindow::CalculatorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CalculatorWindow)
    , calculator(new BallisticCalculator)
{
    ui->setupUi(this);

}

CalculatorWindow::~CalculatorWindow()
{
    delete ui;
}

void CalculatorWindow::on_calculate_clicked()
{
    calculator->muzzleVel   = ui->muzzleVel->value();
    calculator->maxDistance = ui->maxDistance->value();
    calculator->BC          = ui->BC->value();

    std::vector<CalculationResult> results = calculator->calculate();

    QLineSeries *dropSeries = new QLineSeries();
    QLineSeries *timeSeries = new QLineSeries();
    dropSeries->setName("Drop");
    for(CalculationResult result : results){
        dropSeries->append(result.position.x(), result.position.z());
        //timeSeries->append(result.position.x(), result.position.z());
    }



    ui->chartView->chart()->removeAllSeries();
    ui->chartView->chart()->addSeries(dropSeries);

    ui->chartView->chart()->createDefaultAxes();

    //ui->chartView->chart()->addSeries(timeSeries);
}
