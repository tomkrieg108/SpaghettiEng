#pragma once

#include "MathLib/AML/AMLEulerAngles.h"
#include <limits>
#include <iostream>

namespace SpgMth
{
  namespace AML
  {
    class Vector3;
    class EulerAngles;
    class DCM;
    class Quaternion
    {
        public:

            union
            {
                Real data[4];
                struct{Real q0, q1, q2, q3;};
            };
            
            // Constructors
            Quaternion();
            explicit Quaternion(Real q0_, Real q1_, Real q2_, Real q3_);
            explicit Quaternion(Real val);
            explicit Quaternion(const Real data[4]);
            explicit Quaternion(Real scalar, const Vector3& vec);
            explicit Quaternion(const Vector3& rhs);

            // Operator Assignments
            Quaternion& operator+=(const Quaternion& rhs);
            Quaternion& operator-=(const Quaternion& rhs);
            Quaternion& operator*=(const Quaternion& rhs);

            Quaternion& operator+=(Real rhs);
            Quaternion& operator-=(Real rhs);
            Quaternion& operator*=(Real rhs);
            Quaternion& operator/=(Real rhs);

            // Special Object Creators
            static const Quaternion identity();
    };

    // Quaternion / Quaternion Operations
    Quaternion operator-(const Quaternion& rhs);
    Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs);
    Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs);
    Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);

    // Quaternion / Scalar Operations
    Quaternion operator+(const Quaternion& lhs, Real s);
    Quaternion operator-(const Quaternion& lhs, Real s);
    Quaternion operator*(const Quaternion& lhs, Real s);
    Quaternion operator/(const Quaternion& lhs, Real s);
    Quaternion operator+(Real s, const Quaternion& rhs);
    Quaternion operator-(Real s, const Quaternion& rhs);
    Quaternion operator*(Real s, const Quaternion& rhs);
    Quaternion operator/(Real s, const Quaternion& rhs);

    // Quaternion / Vector Operations
    Vector3 operator*(const Quaternion& lhs, const Vector3& rhs);

    // Quaternion Operations
    Quaternion conjugate(const Quaternion& rhs);
    Real norm(const Quaternion& rhs);
    void normalise(Quaternion& rhs);
    Quaternion inverse(const Quaternion& rhs);
    Quaternion unit(const Quaternion& rhs);
    bool isUnitQuat(const Quaternion& rhs, Real tol = std::numeric_limits<Real>::epsilon());
    Real dot(const Quaternion& lhs, const Quaternion& rhs);

    // Attitude Conversion Functions
    Matrix33 quat2DCM(const Quaternion& quat);
    Quaternion dcm2Quat(const Matrix33& dcm);
    EulerAngles quat2EulerAngles(const Quaternion& quat, const EulerAngles::EulerSequence seq = EulerAngles::EulerSequence::XYZ);
    Quaternion eulerAngles2Quat(const EulerAngles& angles);

    // Euler Angles to Quaternion Conversions
    Quaternion eulerAngles2Quat_ZXZ(Real phi, Real theta, Real psi);
    Quaternion eulerAngles2Quat_XYX(Real phi, Real theta, Real psi);
    Quaternion eulerAngles2Quat_YZY(Real phi, Real theta, Real psi);
    Quaternion eulerAngles2Quat_ZYZ(Real phi, Real theta, Real psi);
    Quaternion eulerAngles2Quat_XZX(Real phi, Real theta, Real psi);
    Quaternion eulerAngles2Quat_YXY(Real phi, Real theta, Real psi);
    Quaternion eulerAngles2Quat_XYZ(Real phi, Real theta, Real psi);
    Quaternion eulerAngles2Quat_YZX(Real phi, Real theta, Real psi);
    Quaternion eulerAngles2Quat_ZXY(Real phi, Real theta, Real psi);
    Quaternion eulerAngles2Quat_XZY(Real phi, Real theta, Real psi);
    Quaternion eulerAngles2Quat_ZYX(Real phi, Real theta, Real psi);
    Quaternion eulerAngles2Quat_YXZ(Real phi, Real theta, Real psi);

    // Quaternion to Euler Angle Conversions
    EulerAngles quat2EulerAngles_ZXZ(const Quaternion& quat);
    EulerAngles quat2EulerAngles_XYX(const Quaternion& quat);
    EulerAngles quat2EulerAngles_YZY(const Quaternion& quat);
    EulerAngles quat2EulerAngles_ZYZ(const Quaternion& quat);
    EulerAngles quat2EulerAngles_XZX(const Quaternion& quat);
    EulerAngles quat2EulerAngles_YXY(const Quaternion& quat);
    EulerAngles quat2EulerAngles_XYZ(const Quaternion& quat);
    EulerAngles quat2EulerAngles_YZX(const Quaternion& quat);
    EulerAngles quat2EulerAngles_ZXY(const Quaternion& quat);
    EulerAngles quat2EulerAngles_XZY(const Quaternion& quat);
    EulerAngles quat2EulerAngles_ZYX(const Quaternion& quat);
    EulerAngles quat2EulerAngles_YXZ(const Quaternion& quat);

    // Quaternion Kinematic Functions
    Quaternion integrateQuat(const Quaternion& quat, const Quaternion& quatRates, Real dt);
    Quaternion quatKinematicRates_BodyRates(const Quaternion& quat, const Vector3& bodyRates);
    Quaternion quatKinematicRates_WorldRates(const Quaternion& quat, const Vector3& worldRates);

    // Quaternion Interpolation Functions
    Quaternion linearInterpolate(const Quaternion& startAngles, const Quaternion& endAngles, Real t);
    Quaternion slerpInterpolate(const Quaternion& startAngles, const Quaternion& endAngles, Real t);

    // Stream Functions
    std::ostream& operator<<(std::ostream& os, const Quaternion& obj);

  }
}

