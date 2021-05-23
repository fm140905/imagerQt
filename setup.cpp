#include "setup.h"

int Setup::getPixelPos()
{
    // get sipm pixel pos
    for (int i = 0; i < numZ; i++)
    {
        double zi = i * pitchZ - (numZ - 1) * pitchZ / 2.0;
        for (int j = 0; j < numX; j++)
        {
            double xj = j * pitchX - (numX - 1) * pitchX / 2.0;
            for (int k = 0; k < numY; k++)
            {
                double yk = k * pitchY - (numY - 1) * pitchY / 2.0;
                pixels.push_back(Vector3D(xj, yk, zi));
            }
        }
    }
    return 0;
}

int Setup::pixelateSphere() {
    xbs.resize(thetaBins, std::vector<Vector3D>(phiBins, Vector3D(0, 0, 0)));
    thetaBinCenters.resize(thetaBins, 0);
    phiBinCenters.resize(phiBins, 0);
    double phi = phiMin;
    double theta = thetaMin;
    for (int i = 0; i < thetaBins; i++)
    {
        thetaBinCenters[i] = theta;
        phi = phiMin;
        for (int j = 0; j < phiBins; j++)
        {
            if (i==0)
                phiBinCenters[j] = phi;

            xbs[i][j].X = R * std::cos(theta) * std::cos(phi);
            xbs[i][j].Y = R * std::cos(theta) * std::sin(phi);
            xbs[i][j].Z = R * std::sin(theta);
            phi += dphi;
        }
        theta += dtheta;
    }

    return 0;
}

int Setup::randSource(){
    // random  theta and phi
    // trueTheta = float(std::rand()) / RAND_MAX * (thetaMax - thetaMin) + thetaMin;
    // truePhi = float(std::rand()) / RAND_MAX * (phiMax - phiMin) + phiMin;
    trueTheta = float(std::rand()) / RAND_MAX * M_PI_2 - M_PI_4;
    truePhi = float(std::rand()) / RAND_MAX * M_PI - M_PI_2;
    trueSource = Vector3D(R*std::cos(trueTheta)*std::cos(truePhi),
                          R*std::cos(trueTheta)*std::sin(truePhi),
                          R*std::sin(trueTheta));
    return 0;
}

