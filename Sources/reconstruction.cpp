#include "reconstruction.h"

int addCones(const Setup* config, TH2D* histo, ULong64_t& counts,
             std::vector<Cone>::const_iterator first,
             std::vector<Cone>::const_iterator last)
{
    // project cones onto the spherical surface
    double alpha(0);
    double beta(0);
    double E2(0);
    double sgma2(0);
    double sgmb2(0);
    Vector3D ray;
    for (auto k = first; k < last; k++)
    {
        alpha = k->cosHalfAngle;
        // ideal case: inital energy is known
        // sgma2 = std::pow(0.511*k->E0/std::pow((k->E0-k->Edpst),2)*config->sgmE, 2);
        // realistic case: initial energy = E_1 + E_2
        E2 = k->E0 - k->Edpst;
        sgma2 = std::pow(k->Edpst/std::pow(k->E0, 2) , 2) + std::pow(1/E2 - E2/std::pow(k->E0, 2) , 2);
        sgma2 *= std::pow(config->sgmE * 0.511, 2);
        #pragma omp parallel for private(ray, beta, sgmb2)
        for (int i = 0; i < config->thetaBins; i++)
        {
            for (int j = 0; j < config->phiBins; j++)
            {
                ray = k->apex - config->xbs[i][j];
                beta = getCosAngle(ray, k->axis);
                sgmb2 = std::pow((ray/(ray*ray) + k->axis/(k->axis*k->axis)-
                                (ray+k->axis)/(ray*k->axis))*config->sgmpos * beta, 2);
                sgmb2+= std::pow((ray/(ray*k->axis)-k->axis/(k->axis*k->axis))*config->sgmpos * beta, 2);
                histo->SetBinContent(j+1, i+1,
                    histo->GetBinContent(j+1, i+1) +
                    std::exp(-std::pow((std::pow(beta, config->order)-std::pow(alpha, config->order)), 2)/
                             (2*std::pow(config->order, 2)*
                              (std::pow(alpha,2*config->order-2)*sgma2 +std::pow(beta, 2*config->order-2)*sgmb2))));
            }
        }
    }
    counts += (last - first);

    return 0;
}

int addConesNormalized(const Setup* config, TH2D* histo, ULong64_t& counts,
             std::vector<Cone>::const_iterator first,
             std::vector<Cone>::const_iterator last)
{
    // project cones onto the spherical surface
    double alpha(0);
    double beta(0);
    double E2(0);
    double sgma2(0);
    double sgmb2(0);
    double summ(0);
    Vector3D ray;
    static std::vector<std::vector<double>> probDist(config->thetaBins, std::vector<double>(config->phiBins, 0));
    for (auto k = first; k < last; k++)
    {
        counts++;
        alpha = k->cosHalfAngle;
        // ideal case: inital energy is known
        // sgma2 = std::pow(0.511*k->E0/std::pow((k->E0-k->Edpst),2)*config->sgmE, 2);
        // realistic case: initial energy = E_1 + E_2
        E2 = k->E0 - k->Edpst;
        sgma2 = std::pow(k->Edpst/std::pow(k->E0, 2) , 2) + std::pow(1/E2 - E2/std::pow(k->E0, 2) , 2);
        sgma2 *= std::pow(config->sgmE * 0.511, 2);
        summ = 0;
        #pragma omp parallel for private(ray, beta, sgmb2) shared(probDist) reduction(+:summ)
        for (int i = 0; i < config->thetaBins; i++)
        {
            for (int j = 0; j < config->phiBins; j++)
            {
                ray = k->apex - config->xbs[i][j];
                beta = getCosAngle(ray, k->axis);
                sgmb2 = std::pow((ray/(ray*ray) + k->axis/(k->axis*k->axis)-
                                (ray+k->axis)/(ray*k->axis))*config->sgmpos * beta, 2);
                sgmb2+= std::pow((ray/(ray*k->axis)-k->axis/(k->axis*k->axis))*config->sgmpos * beta, 2);
                probDist[i][j] = std::exp(-std::pow((std::pow(beta, config->order)-std::pow(alpha, config->order)), 2)/
                                (2*std::pow(config->order, 2)*
                                (std::pow(alpha,2*config->order-2)*sgma2 +std::pow(beta, 2*config->order-2)*sgmb2)));
                // use proability density
                // uncomment to use integral in the bin
                // probDist[i][j] = probDist[i][j] * config->dtheta * config->dphi * std::cos(config->thetaBinCenters[i]);
                // summ += probDist[i][j];
                summ += probDist[i][j] * config->dtheta * config->dphi * std::cos(config->thetaBinCenters[i]);
            }
        }
        // #pragma omp parallel for
        for (int i = 0; i < config->thetaBins; i++)
        {
            for (int j = 0; j < config->phiBins; j++)
            {
                histo->SetBinContent(j+1, i+1, (histo->GetBinContent(j+1, i+1)*(counts - 1) + probDist[i][j] / summ) / counts);
            }
        }
    }
    return 0;
}

