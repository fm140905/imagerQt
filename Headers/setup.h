#ifndef SETUP_H
#define SETUP_H


#include <vector>
#include <math.h>
#include <stdexcept>

#include <QString>

class Vector3D
{
public:
    double X=0;
    double Y=0;
    double Z=0;
    Vector3D():Vector3D(0, 0, 0) {}
    Vector3D(const double x, const double y, const double z): X(x), Y(y), Z(z) {}

    Vector3D operator+(const Vector3D& r) const {
        return Vector3D(this->X + r.X, this->Y + r.Y, this->Z + r.Z);
    }
    Vector3D operator-(const Vector3D& r) const {
        return Vector3D(this->X - r.X, this->Y - r.Y, this->Z - r.Z);
    }

    double operator*(const Vector3D& r) const {
        return this->X * r.X + this->Y * r.Y + this->Z * r.Z;
    }
    Vector3D operator*(const double& d) const {
        return Vector3D(this->X * d,  this->Y * d, this->Z * d);
    }
    Vector3D operator/(const double& d) const {
        return Vector3D(this->X / d,  this->Y / d, this->Z / d);
    }
};

inline Vector3D operator*(const double& d, const Vector3D& r){
    return r * d;
}

inline double getCosAngle(const Vector3D& l, const Vector3D& r) {
    return l*r / std::sqrt((l*l) * (r*r));
}


class Setup
{
private:
    /* data */
public:
    double pitchX = 11;
    int      numX =  7;
    double pitchY = 13;
    int      numY =  4;
    double pitchZ = 35;
    int      numZ =  2;
    // pixel coordinates, mm
    std::vector<Vector3D> pixels;
    int getPixelPos();
    // radius of projection sphere,  mm
    double R = 500;
    Setup(): Setup(500) {}
    Setup(const double r):R(r) {
        getPixelPos();
        pixelateSphere();
        randSource();
    }
    Setup(const Vector3D v):
        R(std::sqrt(v.X*v.X + v.Y*v.Y + v.Z * v.Z)),
        trueSource(v)
    {
        getPixelPos();
        pixelateSphere();
        trueTheta = std::asin(v.Z / R);
        truePhi = std::atan2(v.Y, v.X);
    }

    // image setup
    // Altitude angle, rad
    const int thetaBins = 180;
    const int phiBins = 360;
    const double dtheta = 180 / double(thetaBins) * M_PI / 180;
    const double thetaMin = -90 * M_PI / 180 + dtheta / 2;
    const double thetaMax = 90 * M_PI / 180 - dtheta / 2;
    std::vector<double> thetaBinCenters;
    // Azimuthal angle, rad
    const double dphi = 2 * M_PI /double(phiBins);
    const double phiMin = -M_PI + dphi / 2;
    const double phiMax = M_PI - dphi / 2;
    std::vector<double> phiBinCenters;
    // pixelate the projection sphere
    std::vector<std::vector<Vector3D>> xbs;
    int pixelateSphere();

    // simulation setup
    const int nCones= 100;
    double trueTheta;
    double truePhi;
    Vector3D trueSource;
    int randSource();
    const double sgmE = 0.06; // 6% energy resolution
    const Vector3D sgmpos = Vector3D(0.8 / 2, 0.43 / 2, 1.0 / 2); // mm
    // const Vector3D sgmpos(0, 0, 0); // mm
    const int order=3;
    const std::string conefilePath = ":/Data/Data/cones_ideal.txt";

    // refresh every 10 events
    const int chuckSize = 10;

    // queue capacity
    ulong capacity=1024;
    // max number of cones to process
    ulong maxN=10000;
};

class Cone
{
public:
    // interaction site 1 is the apex
    Vector3D apex;
    // site 2 - site 1 is the axis
    Vector3D axis;
    // half angle = abs (scattering angle)
    // for neutron, 0 < half angle < pi /2
    // for gamma,   0 < half angle < pi
    double cosHalfAngle;
    // initial energy
    double E0;
    // energy deposited in compoton scattering
    double Edpst;
    Cone(): Cone(Vector3D(0,0,0), Vector3D(0,0,0), 0, 0, 0) {}
    Cone(const Vector3D point, const Vector3D line, const double cosTheta, const double E0_, const double Edpst_):
       apex(point), axis(line), cosHalfAngle(cosTheta), E0(E0_), Edpst(Edpst_) {}
    Cone(const std::string& record) {
        apex = Vector3D(std::stod(record.substr(0, 8)),
                        std::stod(record.substr(8, 8)),
                        std::stod(record.substr(16, 8)));
        axis = Vector3D(std::stod(record.substr(24, 8)),
                        std::stod(record.substr(32, 8)),
                        std::stod(record.substr(40, 8)));
        cosHalfAngle = std::stod(record.substr(48, 13));
        E0 = std::stod(record.substr(61, 13));
        Edpst = std::stod(record.substr(74, 13));
    }
    Cone(const QString& record) {
        apex = Vector3D(record.mid(0, 8).toDouble(),
                        record.mid(8, 8).toDouble(),
                        record.mid(16, 8).toDouble());
        axis = Vector3D(record.mid(24, 8).toDouble(),
                        record.mid(32, 8).toDouble(),
                        record.mid(40, 8).toDouble());
        cosHalfAngle = record.mid(48, 13).toDouble();
        E0 = record.mid(61, 13).toDouble();
        Edpst = record.mid(74, 13).toDouble();
    }
};

#endif // SETUP_H
