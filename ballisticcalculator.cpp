#include "ballisticcalculator.h"
#include "ballisticcoefficients.h"

BallisticCalculator::BallisticCalculator()
{

}

//TODO: interpolate
float BallisticCalculator::getG1Coeff(float mach)
{
    float lastError = 0.0f;
    for(int i = 0; i < G1_COEFF_COUNT; i++)
    {
        float error = std::abs(mach - G1CoeffMach[i]);

        if(error > lastError && i != 0)
        {
            return G1CoeffValue[i-1];
        }

        lastError = error;
    }
    return G1CoeffValue[G1_COEFF_COUNT-1];
}

QVector3D BallisticCalculator::calculateDrag(QVector3D velocity)
{
    float speedOfSound = 20.05f*sqrtf(1+temperature/273.15f);
    float standardSD = 703.0f;

    float speed = velocity.length();
    QVector3D drag = -airDensity*velocity*speed*getG1Coeff(speed/speedOfSound)/(2*standardSD*BC);

    return drag;
}

std::vector<CalculationResult> BallisticCalculator::calculate()
{
    std::vector<CalculationResult> results;

    airDensity = 1.292f*274.15f/(temperature+273.15)*pressure/1013.25;

    float dt = 0.01f;

    CalculationResult currentState = {};
    currentState.velocity.setX(muzzleVel);

    while(true)
    {
        currentState.energy = currentState.velocity.lengthSquared() * bulletMass * 0.5f;

        results.push_back(currentState);

        QVector3D a1 = dt*(calculateDrag(currentState.velocity)        + QVector3D(0.0f, 0.0f, -9.81f));
        QVector3D a2 = dt*(calculateDrag(currentState.velocity+a1*0.5) + QVector3D(0.0f, 0.0f, -9.81f));
        QVector3D a3 = dt*(calculateDrag(currentState.velocity+a2*0.5) + QVector3D(0.0f, 0.0f, -9.81f));
        QVector3D a4 = dt*(calculateDrag(currentState.velocity+a3)     + QVector3D(0.0f, 0.0f, -9.81f));

        currentState.velocity = currentState.velocity + 1.0f/6.0f*(a1 + 2.0f*a2 + 2.0f*a3 + a4);
        currentState.velocity.setY(windSpeed*(1.0f-(currentState.velocity.length()/muzzleVel)));

        currentState.position = currentState.position + currentState.velocity*dt;
        currentState.time += dt;

        if(currentState.position.x() > maxDistance || currentState.velocity.x() <= 1.0f)
        {
            break;
        }
    }

    float lastError = std::numeric_limits<float>::infinity();
    CalculationResult zeroResult = (CalculationResult)results.back();
    for(auto result : results)
    {
        float error = std::abs(result.position.x() - zeroDistance);

        if(error > lastError)
        {
            zeroResult = result;
            break;
        }

        lastError = error;
    }

    float zeroDrop = zeroResult.position.z();
    float sightSlope = -(scopeHeight-zeroDrop)/zeroDistance;

    for(int i = 0; i < (int)results.size(); i++){
        results[i].POI = results[i].position.z() - (scopeHeight + results[i].position.x()*sightSlope);
    }

    return results;
}
