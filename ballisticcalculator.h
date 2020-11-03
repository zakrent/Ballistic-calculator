#ifndef BALLISTICCALCULATOR_H
#define BALLISTICCALCULATOR_H

#include <QVector3D>
#include <vector>

struct CalculationResult
{
    float time;
    float energy;
    float POI;
    QVector3D position;
    QVector3D velocity;
};

class BallisticCalculator
{
public:
    float muzzleVel;
    float BC;
    float bulletMass;
    float maxDistance;
    float temperature;
    float pressure;
    float zeroDistance;
    float scopeHeight;
    float windSpeed;

    std::vector<CalculationResult> calculate();

    BallisticCalculator();

private:
    float airDensity;

    float getG1Coeff(float mach);
    QVector3D calculateDrag(QVector3D velocity);

};

#endif // BALLISTICCALCULATOR_H
