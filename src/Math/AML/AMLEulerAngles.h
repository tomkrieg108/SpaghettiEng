#pragma once
#include "Math/MathX.h"
#include "AMLMatrix33.h"
#include "AMLVector3.h"
#include <limits>

namespace MathX
{
  namespace AML
  {
    class Matrix33;
    class Vector3;
    class EulerAngles
    {
        public:

            enum class EulerSequence
            {
                ZXZ,
                XYX,
                YZY,
                ZYZ,
                XZX,
                YXY,
                XYZ,
                YZX,
                ZXY,
                XZY,
                ZYX,
                YXZ
            };
            
            // Constructors
            EulerAngles();
            EulerAngles(Real phi_, Real theta_, Real psi_, EulerSequence seq = EulerSequence::XYZ);

            // Euler Angle Operations    
            EulerSequence getEulerSequence() const {return seq_;};

            // Euler Angles
            Real phi;
            Real theta;
            Real psi;

        private:

            // Euler Angle Sequence
            EulerSequence   seq_;
    };

    // Stream Functions
    std::ostream& operator<<(std::ostream& os, const EulerAngles& obj);

    // Euler Angle Conversions
    Matrix33 eulerAngles2DCM(const EulerAngles& angles);
    EulerAngles dcm2EulerAngles(const Matrix33& dcm, const EulerAngles::EulerSequence seq = EulerAngles::EulerSequence::XYZ);
    EulerAngles convertEulerAngleSequence(const EulerAngles& angles, const EulerAngles::EulerSequence seq);

    // Euler Angles to DCM Conversions
    Matrix33 eulerAngles2DCM_ZXZ(Real phi, Real theta, Real psi);
    Matrix33 eulerAngles2DCM_XYX(Real phi, Real theta, Real psi);
    Matrix33 eulerAngles2DCM_YZY(Real phi, Real theta, Real psi);
    Matrix33 eulerAngles2DCM_ZYZ(Real phi, Real theta, Real psi);
    Matrix33 eulerAngles2DCM_XZX(Real phi, Real theta, Real psi);
    Matrix33 eulerAngles2DCM_YXY(Real phi, Real theta, Real psi);
    Matrix33 eulerAngles2DCM_XYZ(Real phi, Real theta, Real psi);
    Matrix33 eulerAngles2DCM_YZX(Real phi, Real theta, Real psi);
    Matrix33 eulerAngles2DCM_ZXY(Real phi, Real theta, Real psi);
    Matrix33 eulerAngles2DCM_XZY(Real phi, Real theta, Real psi);
    Matrix33 eulerAngles2DCM_ZYX(Real phi, Real theta, Real psi);
    Matrix33 eulerAngles2DCM_YXZ(Real phi, Real theta, Real psi);

    // DCM to Euler Angle Conversions
    EulerAngles dcm2EulerAngles_ZXZ(const Matrix33& dcm);
    EulerAngles dcm2EulerAngles_XYX(const Matrix33& dcm);
    EulerAngles dcm2EulerAngles_YZY(const Matrix33& dcm);
    EulerAngles dcm2EulerAngles_ZYZ(const Matrix33& dcm);
    EulerAngles dcm2EulerAngles_XZX(const Matrix33& dcm);
    EulerAngles dcm2EulerAngles_YXY(const Matrix33& dcm);
    EulerAngles dcm2EulerAngles_XYZ(const Matrix33& dcm);
    EulerAngles dcm2EulerAngles_YZX(const Matrix33& dcm);
    EulerAngles dcm2EulerAngles_ZXY(const Matrix33& dcm);
    EulerAngles dcm2EulerAngles_XZY(const Matrix33& dcm);
    EulerAngles dcm2EulerAngles_ZYX(const Matrix33& dcm);
    EulerAngles dcm2EulerAngles_YXZ(const Matrix33& dcm);

    // Euler Angle Sequence Conversions
    EulerAngles converEulerAnglesXYZtoZXZ(Real phi, Real theta, Real psi);
    EulerAngles converEulerAnglesZXZtoXYZ(Real phi, Real theta, Real psi);

    // Euler Angle Kinematics
    EulerAngles integrateEulerAngles(const EulerAngles& angles, const EulerAngles& angleRates, Real dt);
    EulerAngles eulerAngleKinematicRates(const EulerAngles& angles, const Vector3& bodyRates);

    Matrix33 eulerAngleRatesMatrix_XYZ(Real phi, Real theta, Real psi);
    Matrix33 eulerAngleRatesMatrix_ZXZ(Real phi, Real theta, Real psi);
    Matrix33 eulerAngleRatesMatrix_YZY(Real phi, Real theta, Real psi);
    Matrix33 eulerAngleRatesMatrix_XYX(Real phi, Real theta, Real psi);
    Matrix33 eulerAngleRatesMatrix_ZYZ(Real phi, Real theta, Real psi);
    Matrix33 eulerAngleRatesMatrix_XZX(Real phi, Real theta, Real psi);
    Matrix33 eulerAngleRatesMatrix_YXY(Real phi, Real theta, Real psi);
    Matrix33 eulerAngleRatesMatrix_YZX(Real phi, Real theta, Real psi);
    Matrix33 eulerAngleRatesMatrix_ZXY(Real phi, Real theta, Real psi);
    Matrix33 eulerAngleRatesMatrix_XZY(Real phi, Real theta, Real psi);
    Matrix33 eulerAngleRatesMatrix_ZYX(Real phi, Real theta, Real psi);
    Matrix33 eulerAngleRatesMatrix_YXZ(Real phi, Real theta, Real psi);

    // Euler Angle Interpolation
    EulerAngles interpolate(const EulerAngles& startAngles, const EulerAngles& endAngles, Real t);


  }
}