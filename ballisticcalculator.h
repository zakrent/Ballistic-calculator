#ifndef BALLISTICCALCULATOR_H
#define BALLISTICCALCULATOR_H

#include <QVector3D>
#include <vector>

struct CalculationResult
{
    float time;
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

    std::vector<CalculationResult> calculate();

    BallisticCalculator();

private:

    float getG1Coeff(float mach);
};

#endif // BALLISTICCALCULATOR_H
