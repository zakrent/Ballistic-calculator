#include "calculatorwindow.h"
#include "ui_calculatorwindow.h"

CalculatorWindow::CalculatorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CalculatorWindow)
    , calculator(new BallisticCalculator)
{
    ui->setupUi(this);

    POISeries = new QLineSeries();
    POISeries->setName("POI [m]");

    dropSeries = new QLineSeries();
    dropSeries->setName("Drop [m]");

    speedSeries = new QLineSeries();
    speedSeries->setName("Speed [m/s]");
    speedSeries->setVisible(false);

    energySeries = new QLineSeries();
    energySeries->setName("Energy [J]");
    energySeries->setVisible(false);

    timeSeries = new QLineSeries();
    timeSeries->setName("Time [s]");
    timeSeries->setVisible(false);

    ui->chartView->chart()->addSeries(POISeries);
    ui->chartView->chart()->addSeries(dropSeries);
    ui->chartView->chart()->addSeries(speedSeries);
    ui->chartView->chart()->addSeries(energySeries);
    ui->chartView->chart()->addSeries(timeSeries);
    ui->chartView->chart()->createDefaultAxes();

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);


    plotDistance = 1000.0f;

    ui->chartView->chart()->axes(Qt::Horizontal).back()->setRange(0, plotDistance);

    ui->chartView->setRenderHints(QPainter::Antialiasing);
}

CalculatorWindow::~CalculatorWindow()
{
    delete ui;
}

void CalculatorWindow::on_calculate_clicked()
{
    calculator->muzzleVel    = ui->muzzleVel->value();
    calculator->maxDistance  = ui->maxDistance->value();
    calculator->BC           = ui->BC->value();
    calculator->bulletMass   = ui->bulletMass->value()/1000.0f;
    calculator->temperature  = ui->temperature->value();
    calculator->pressure     = ui->pressure->value();
    calculator->scopeHeight  = ui->scopeHeight->value()/100.0f;
    calculator->zeroDistance = ui->zeroDistance->value();
    calculator->windSpeed    = ui->windSpeed->value();

    plotDistance = ui->maxDistance->value();
    ui->plotDistanceSlider->setValue(plotDistance/100.0f);
    ui->plotDistanceSpinBox->setValue(plotDistance);
    ui->chartView->chart()->axes(Qt::Horizontal).back()->setRange(0, plotDistance);

    std::vector<CalculationResult> results = calculator->calculate();

    POISeries->clear();
    dropSeries->clear();
    speedSeries->clear();
    energySeries->clear();
    timeSeries->clear();

    for(CalculationResult result : results){
        POISeries->append(result.position.x(), result.POI);
        dropSeries->append(result.position.x(), result.position.z());
        speedSeries->append(result.position.x(), result.velocity.length());
        energySeries->append(result.position.x(), result.energy);
        timeSeries->append(result.position.x(), result.time);
    }

    autoscaleYAxis();

}

void CalculatorWindow::on_plotDistanceSlider_sliderMoved(int position)
{
    plotDistance = position*100.0f;
    ui->chartView->chart()->axes(Qt::Horizontal).back()->setRange(0, plotDistance);
    ui->plotDistanceSpinBox->setValue(plotDistance);
}

void CalculatorWindow::on_plotDistanceSpinBox_valueChanged(double arg1)
{
    plotDistance = arg1;
    ui->chartView->chart()->axes(Qt::Horizontal).back()->setRange(0, plotDistance);
    ui->plotDistanceSlider->setValue(plotDistance/100.0f);

}

void CalculatorWindow::on_Drop_stateChanged(int arg1)
{
    dropSeries->setVisible(arg1);
    autoscaleYAxis();
}


void CalculatorWindow::on_checkBox_3_stateChanged(int arg1)
{
    speedSeries->setVisible(arg1);
    autoscaleYAxis();
}


void CalculatorWindow::on_checkBox_4_stateChanged(int arg1)
{
    energySeries->setVisible(arg1);
    autoscaleYAxis();
}

void CalculatorWindow::on_checkBox_5_stateChanged(int arg1)
{
    timeSeries->setVisible(arg1);
    autoscaleYAxis();
}


void CalculatorWindow::on_POI_stateChanged(int arg1)
{
    POISeries->setVisible(arg1);
    autoscaleYAxis();
}


void CalculatorWindow::autoscaleYAxis()
{
    float min = std::numeric_limits<float>::infinity();
    float max = -std::numeric_limits<double>::infinity();
    for(auto series_ : series){
        if(!series_->isVisible())
            continue;

        for(auto point : series_->points()){
            if(point.y() > max){
                max = point.y();
            }
            if(point.y() < min){
                min = point.y();
            }
        }
    }

    ui->chartView->chart()->axes(Qt::Vertical).back()->setRange(min, max);

    QValueAxis *axis = (QValueAxis*)ui->chartView->chart()->axes(Qt::Vertical).back();
    axis->applyNiceNumbers();
    axis->setTickCount(11);

}
