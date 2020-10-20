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

std::vector<CalculationResult> BallisticCalculator::calculate()
{
    std::vector<CalculationResult> results;

    float dt = 0.001f;

    CalculationResult currentState = {};
    currentState.velocity.setX(muzzleVel);

    float airDensity = 1.225f;
    float speedOfSound = 340.29f;

    float standardSD = 703.0f;

    while(true)
    {
        results.push_back(currentState);

        float speed = currentState.velocity.length();
        QVector3D drag = airDensity*currentState.velocity*speed*getG1Coeff(speed/speedOfSound)/(2*standardSD);

        currentState.position = currentState.position + currentState.velocity*dt;
        currentState.velocity = currentState.velocity + drag*dt + QVector3D(0.0f, 0.0f, -9.81f)*dt;
        currentState.time += dt;

        if(currentState.position.x() > maxDistance || currentState.velocity.x() <= 0.1f)
        {
            break;
        }
    }

    return results;
}
