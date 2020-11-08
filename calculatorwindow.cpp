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
    POISeries->setVisible(false);

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

    windDeflectionSeries = new QLineSeries();
    windDeflectionSeries->setName("Wind deflection [m]");
    windDeflectionSeries->setVisible(false);

    ui->chartView->chart()->addSeries(POISeries);
    ui->chartView->chart()->addSeries(dropSeries);
    ui->chartView->chart()->addSeries(speedSeries);
    ui->chartView->chart()->addSeries(energySeries);
    ui->chartView->chart()->addSeries(timeSeries);
    ui->chartView->chart()->addSeries(windDeflectionSeries);
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
    windDeflectionSeries->clear();

    for(CalculationResult result : results){
        POISeries->append(result.position.x(), result.POI);
        dropSeries->append(result.position.x(), result.position.z());
        speedSeries->append(result.position.x(), result.velocity.length());
        energySeries->append(result.position.x(), result.energy);
        timeSeries->append(result.position.x(), result.time);
        windDeflectionSeries->append(result.position.x(), result.position.y());
    }

    autoscaleYAxis();

    ui->tableWidget->clearContents();

    int tableRows = ui->maxDistance->value()/ui->stepSize->value();
    ui->tableWidget->setRowCount(tableRows);
    int nextRow = 0;
    float nextDistance = ui->stepSize->value();
    float prevError = 100000.0;
    CalculationResult prevResult = results[0];
    for(int i = 0; i < (int)results.size(); i++){
        CalculationResult result = results[i];

        float error = std::abs(result.position.x() - nextDistance);

        if(i == (int)results.size() - 1){
            error = 100000.0f;
            prevResult = result;
        }

        if(error > prevError){
            ui->tableWidget->setItem(nextRow, 0, new QTableWidgetItem(QString::number(std::roundf(prevResult.position.x()/ui->stepSize->value())*ui->stepSize->value())));
            ui->tableWidget->setItem(nextRow, 1, new QTableWidgetItem(QString::number(prevResult.POI)));
            ui->tableWidget->setItem(nextRow, 2, new QTableWidgetItem(QString::number(prevResult.position.z())));
            ui->tableWidget->setItem(nextRow, 3, new QTableWidgetItem(QString::number(prevResult.position.y())));
            ui->tableWidget->setItem(nextRow, 4, new QTableWidgetItem(QString::number(prevResult.energy)));

            prevError = 10000.0;
            nextDistance += ui->stepSize->value();
            nextRow++;
        }
        else{
            prevError = error;
            prevResult = result;
        }
    }

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


void CalculatorWindow::on_checkBox_stateChanged(int arg1)
{
    windDeflectionSeries->setVisible(arg1);
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

void CalculatorWindow::on_export_2_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this);

    int separatorIdx = ui->separator->currentIndex();

    QString separator = " ";

    switch(separatorIdx){
        case 0:
            separator = " ";
        break;
        case 1:
            separator = ";";
        break;
        case 2:
            separator = ",";
        break;
    }

    if(!fileName.isEmpty()){
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly)){
            QMessageBox::information(this, tr("Unable to open file"),
                   file.errorString());
            return;
        }
        QTextStream out(&file);

        for(int j = 0; j < ui->tableWidget->columnCount(); j++){
            out<<ui->tableWidget->horizontalHeaderItem(j)->text();
            if(j != ui->tableWidget->columnCount()-1)
                out<<separator;
        }
        out<<Qt::endl;

        for(int i = 0; i < ui->tableWidget->rowCount(); ++i){
            for (int j = 0; j < ui->tableWidget->columnCount(); j++){
                out<<ui->tableWidget->item(i, j)->text();
                if(j != ui->tableWidget->columnCount()-1)
                    out<<separator;

            }
            out<<Qt::endl;
        }
    }
}
