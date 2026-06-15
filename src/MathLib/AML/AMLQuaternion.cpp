#include "MathLib/AML/AMLQuaternion.h"
#include "MathLib/AML/AMLVector3.h"
#include "MathLib/AML/AMLDCM.h"
#include <cmath>

namespace SpgMth
{
  namespace AML
  {
    // Constructors
    Quaternion::Quaternion()
    :q0(0.0),q1(0.0),q2(0.0),q3(0.0)
    {}
    
    Quaternion::Quaternion(Real q0_, Real q1_, Real q2_, Real q3_)
    :q0(q0_),q1(q1_),q2(q2_),q3(q3_)
    {}

    Quaternion::Quaternion(Real val)
    :q0(val),q1(val),q2(val),q3(val)
    {}

    Quaternion::Quaternion(const Real data_[4])
    :q0(data_[0]),q1(data_[1]),q2(data_[2]),q3(data_[3])
    {}

    Quaternion::Quaternion(Real scalar, const Vector3& vec)
    :q0(scalar),q1(vec.x),q2(vec.y),q3(vec.z)
    {}

    Quaternion::Quaternion(const Vector3& rhs)
    :q0(0.0),q1(rhs.x),q2(rhs.y),q3(rhs.z)
    {}

    // Operator Assignments
    Quaternion& Quaternion::operator+=(const Quaternion& rhs)
    {
        q0 += rhs.q0;
        q1 += rhs.q1;
        q2 += rhs.q2;
        q3 += rhs.q3;
        return *this;
    }
    Quaternion& Quaternion::operator-=(const Quaternion& rhs)
    {
        q0 -= rhs.q0;
        q1 -= rhs.q1;
        q2 -= rhs.q2;
        q3 -= rhs.q3;
        return *this;
    }
    Quaternion& Quaternion::operator*=(const Quaternion& rhs)
    {
        Real q0_new = (rhs.q0 * q0) - (rhs.q1 * q1) - (rhs.q2 * q2) - (rhs.q3 * q3);
        Real q1_new = (rhs.q0 * q1) + (rhs.q1 * q0) - (rhs.q2 * q3) + (rhs.q3 * q2);
        Real q2_new = (rhs.q0 * q2) + (rhs.q1 * q3) + (rhs.q2 * q0) - (rhs.q3 * q1);
        Real q3_new = (rhs.q0 * q3) - (rhs.q1 * q2) + (rhs.q2 * q1) + (rhs.q3 * q0);
        q0 = q0_new;
        q1 = q1_new;
        q2 = q2_new;
        q3 = q3_new;
        return *this;
    }

    Quaternion& Quaternion::operator+=(Real rhs)
    {
        q0 += rhs;
        q1 += rhs;
        q2 += rhs;
        q3 += rhs;
        return *this;
    }
    Quaternion& Quaternion::operator-=(Real rhs)
    {
        q0 -= rhs;
        q1 -= rhs;
        q2 -= rhs;
        q3 -= rhs;
        return *this;
    }
    Quaternion& Quaternion::operator*=(Real rhs)
    {
        q0 *= rhs;
        q1 *= rhs;
        q2 *= rhs;
        q3 *= rhs;
        return *this;
    }
    Quaternion& Quaternion::operator/=(Real rhs)
    {
        q0 /= rhs;
        q1 /= rhs;
        q2 /= rhs;
        q3 /= rhs;
        return *this;
    }

    // Special Object Creators
    const Quaternion Quaternion::identity()
    {
        return Quaternion(1.0, 0.0, 0.0, 0.0);
    }

    // Steam Functions
    std::ostream& operator<<(std::ostream& os, const Quaternion& obj)
    {
        os << "QUAT [" << obj.q0 << "," << obj.q1 << "," << obj.q2 << "," << obj.q3 << "]";
        return os;
    }

    // Quaternion / Quaternion Operations
    Quaternion operator-(const Quaternion& rhs) {return (Quaternion(rhs) *= -1.0);}
    Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) {return (Quaternion(lhs) += rhs);}
    Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs) {return (Quaternion(lhs) -= rhs);}
    Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs) {return (Quaternion(lhs) *= rhs);}

    // Quaternion / Scalar Operations
    Quaternion operator+(const Quaternion& lhs, Real s) {return (Quaternion(lhs) += s);}
    Quaternion operator-(const Quaternion& lhs, Real s) {return (Quaternion(lhs) -= s);}
    Quaternion operator*(const Quaternion& lhs, Real s) {return (Quaternion(lhs) *= s);}
    Quaternion operator/(const Quaternion& lhs, Real s) {return (Quaternion(lhs) /= s);}
    Quaternion operator+(Real s, const Quaternion& rhs) {return Quaternion(s + rhs.q0, s + rhs.q1, s + rhs.q2, s + rhs.q3);}
    Quaternion operator-(Real s, const Quaternion& rhs) {return Quaternion(s - rhs.q0, s - rhs.q1, s - rhs.q2, s - rhs.q3);}
    Quaternion operator*(Real s, const Quaternion& rhs) {return Quaternion(s * rhs.q0, s * rhs.q1, s * rhs.q2, s * rhs.q3);}
    Quaternion operator/(Real s, const Quaternion& rhs) {return Quaternion(s / rhs.q0, s / rhs.q1, s / rhs.q2, s / rhs.q3);}

    // Quaternion / Vector Operations
    Vector3 operator*(const Quaternion& lhs, const Vector3& rhs) {return quat2DCM(lhs) * rhs;}

    // Quaternion Operations
    Quaternion conjugate(const Quaternion& rhs){return Quaternion(rhs.q0, -rhs.q1, -rhs.q2, -rhs.q3);}
    Real norm(const Quaternion& rhs) {return sqrt(rhs.q0*rhs.q0 + rhs.q1*rhs.q1 + rhs.q2*rhs.q2 + rhs.q3*rhs.q3);}
    Quaternion inverse(const Quaternion& rhs) {return (conjugate(rhs) / norm(rhs));}
    Quaternion unit(const Quaternion& rhs) 
    {
        Real mag = norm(rhs);
        if(mag > 0.0){return (Quaternion(rhs)/mag);}
        return Quaternion(rhs);
    }
    void normalise(Quaternion& rhs)
    {
        Real mag = norm(rhs);
        if(mag > 0.0){rhs /= mag;}
    }
    bool isUnitQuat(const Quaternion& rhs, Real tol) {return (fabs(norm(rhs) - 1.0) < 2.0 * tol);}
    Real dot(const Quaternion& lhs, const Quaternion& rhs)
    {
        return (rhs.q0*lhs.q0 + rhs.q1*lhs.q1 + rhs.q2*lhs.q2 + rhs.q3*lhs.q3);
    }


    // DCM Conversion Functions
    Matrix33 quat2DCM(const Quaternion& rhs)
    {
        const Real TOL = 0.0001;

        // Check if valid rotation quaternion
        if (isUnitQuat(rhs, TOL))
        {
            Real data[3][3];
            const Real q0   = rhs.q0;
            const Real q1   = rhs.q1;
            const Real q2   = rhs.q2;
            const Real q3   = rhs.q3;
            const Real q0_2 = q0*q0;
            const Real q1_2 = q1*q1;
            const Real q2_2 = q2*q2;
            const Real q3_2 = q3*q3;
            const Real q1q2 = q1*q2;
            const Real q0q3 = q0*q3;
            const Real q1q3 = q1*q3;
            const Real q0q2 = q0*q2;
            const Real q2q3 = q2*q3;
            const Real q0q1 = q0*q1;
            data[0][0] = q0_2 + q1_2 - q2_2 - q3_2;
            data[0][1] = 2.0 * (q1q2 + q0q3);
            data[0][2] = 2.0 * (q1q3 - q0q2);
            data[1][0] = 2.0 * (q1q2 - q0q3);
            data[1][1] = q0_2 - q1_2 + q2_2 - q3_2;
            data[1][2] = 2.0 * (q2q3 + q0q1);
            data[2][0] = 2.0 * (q1q3 + q0q2);
            data[2][1] = 2.0 * (q2q3 - q0q1);
            data[2][2] = q0_2 - q1_2 - q2_2 + q3_2;
            return Matrix33(data);
        }
        return Matrix33::identity();
    }
    Quaternion dcm2Quat(const Matrix33& dcm)
    {
        const Real TOL = 0.0001;

        // Check if DCM is Valid
        if (isValidDCM(dcm, TOL))
        {
            Real q0 = 0.0;
            Real q1 = 0.0;
            Real q2 = 0.0;
            Real q3 = 0.0;
            const Real x4q0_2 = (1.0 + dcm.m11 + dcm.m22 + dcm.m33);
            const Real x4q1_2 = (1.0 + dcm.m11 - dcm.m22 - dcm.m33);
            const Real x4q2_2 = (1.0 - dcm.m11 + dcm.m22 - dcm.m33);
            const Real x4q3_2 = (1.0 - dcm.m11 - dcm.m22 + dcm.m33);
            const Real x4q2q3 = dcm.m23 + dcm.m32;
            const Real x4q1q3 = dcm.m31 + dcm.m13;
            const Real x4q1q2 = dcm.m12 + dcm.m21;
            const Real x4q0q1 = dcm.m23 - dcm.m32;
            const Real x4q0q2 = dcm.m31 - dcm.m13;
            const Real x4q0q3 = dcm.m12 - dcm.m21;

            // Check which Trace is Largest
            if (x4q0_2 >= x4q1_2 && 
                x4q0_2 >= x4q2_2 && 
                x4q0_2 >= x4q3_2) // 4q0_2 Largest
            {
                const Real x2q0 = sqrt(x4q0_2);
                q0 = 0.5 * x2q0;
                q1 = 0.5 * x4q0q1 / x2q0;
                q2 = 0.5 * x4q0q2 / x2q0;
                q3 = 0.5 * x4q0q3 / x2q0;
            }
            else if (x4q1_2 >= x4q0_2 && 
                    x4q1_2 >= x4q2_2 && 
                    x4q1_2 >= x4q3_2) // 4q1_2 Largest
            {
                const Real x2q1 = sqrt(x4q1_2);
                q0 = 0.5 * x4q0q1 / x2q1;
                q1 = 0.5 * x2q1;
                q2 = 0.5 * x4q1q2 / x2q1;
                q3 = 0.5 * x4q1q3 / x2q1;
            }
            else if (x4q2_2 >= x4q0_2 && 
                    x4q2_2 >= x4q1_2 && 
                    x4q2_2 >= x4q3_2) // 4q2_2 Largest
            {
                const Real x2q2 = sqrt(x4q2_2);
                q0 = 0.5 * x4q0q2 / x2q2;
                q1 = 0.5 * x4q1q2 / x2q2;
                q2 = 0.5 * x2q2;
                q3 = 0.5 * x4q2q3 / x2q2;
            }
            else if (x4q3_2 >= x4q0_2 && 
                    x4q3_2 >= x4q1_2 && 
                    x4q3_2 >= x4q2_2) // 4q3_2 Largest
            {
                const Real x2q3 = sqrt(x4q3_2);
                q0 = 0.5 * x4q0q3 / x2q3;
                q1 = 0.5 * x4q1q3 / x2q3;
                q2 = 0.5 * x4q2q3 / x2q3;
                q3 = 0.5 * x2q3;
            }
            return Quaternion(q0, q1, q2, q3);
        }
        return Quaternion::identity();
    }

    Quaternion eulerAngles2Quat(const EulerAngles& angles)
    {
        switch(angles.getEulerSequence())
        {
            case EulerAngles::EulerSequence::XYZ:
                return eulerAngles2Quat_XYZ(angles.phi, angles.theta, angles.psi);
            case EulerAngles::EulerSequence::ZXZ:
                return eulerAngles2Quat_ZXZ(angles.phi, angles.theta, angles.psi);
            case EulerAngles::EulerSequence::XYX:
                return eulerAngles2Quat_XYX(angles.phi, angles.theta, angles.psi);
            case EulerAngles::EulerSequence::YZY:
                return eulerAngles2Quat_YZY(angles.phi, angles.theta, angles.psi);
            case EulerAngles::EulerSequence::ZYZ:
                return eulerAngles2Quat_ZYZ(angles.phi, angles.theta, angles.psi);
            case EulerAngles::EulerSequence::XZX:
                return eulerAngles2Quat_XZX(angles.phi, angles.theta, angles.psi);
            case EulerAngles::EulerSequence::YXY:
                return eulerAngles2Quat_YXY(angles.phi, angles.theta, angles.psi);
            case EulerAngles::EulerSequence::YZX:
                return eulerAngles2Quat_YZX(angles.phi, angles.theta, angles.psi);
            case EulerAngles::EulerSequence::ZXY:
                return eulerAngles2Quat_ZXY(angles.phi, angles.theta, angles.psi);
            case EulerAngles::EulerSequence::XZY:
                return eulerAngles2Quat_XZY(angles.phi, angles.theta, angles.psi);
            case EulerAngles::EulerSequence::ZYX:
                return eulerAngles2Quat_ZYX(angles.phi, angles.theta, angles.psi);
            case EulerAngles::EulerSequence::YXZ:
                return eulerAngles2Quat_YXZ(angles.phi, angles.theta, angles.psi);
        }
        return Quaternion::identity();
    }

    EulerAngles quat2EulerAngles(const Quaternion& quat, const EulerAngles::EulerSequence seq)
    {
        // Check for valid Quat
        if (isUnitQuat(quat))
        {
            switch(seq)
            {
                case EulerAngles::EulerSequence::XYZ:
                    return quat2EulerAngles_XYZ(quat);
                case EulerAngles::EulerSequence::ZXZ:
                    return quat2EulerAngles_ZXZ(quat);
                case EulerAngles::EulerSequence::XYX:
                    return quat2EulerAngles_XYX(quat);
                case EulerAngles::EulerSequence::YZY:
                    return quat2EulerAngles_YZY(quat);
                case EulerAngles::EulerSequence::ZYZ:
                    return quat2EulerAngles_ZYZ(quat);
                case EulerAngles::EulerSequence::XZX:
                    return quat2EulerAngles_XZX(quat);
                case EulerAngles::EulerSequence::YXY:
                    return quat2EulerAngles_YXY(quat);
                case EulerAngles::EulerSequence::YZX:
                    return quat2EulerAngles_YZX(quat);
                case EulerAngles::EulerSequence::ZXY:
                    return quat2EulerAngles_ZXY(quat);
                case EulerAngles::EulerSequence::XZY:
                    return quat2EulerAngles_XZY(quat);
                case EulerAngles::EulerSequence::ZYX:
                    return quat2EulerAngles_ZYX(quat);
                case EulerAngles::EulerSequence::YXZ:
                    return quat2EulerAngles_YXZ(quat);
            }
        }
        return EulerAngles();
    }


    Quaternion eulerAngles2Quat_ZXZ(Real phi, Real theta, Real psi)
    {
        const Real c1 = cos(0.5 * phi);
        const Real s1 = sin(0.5 * phi);
        const Real c2 = cos(0.5 * theta);
        const Real s2 = sin(0.5 * theta);
        const Real c3 = cos(0.5 * psi);
        const Real s3 = sin(0.5 * psi);
        const Real q0 = c1*c2*c3 - s1*c2*s3;
        const Real q1 = c1*c2*s3 + s1*s2*s3;
        const Real q2 = c1*s2*s3 - s1*c2*s3;
        const Real q3 = c1*c2*s3 + c1*c2*s3;
        return Quaternion(q0, q1, q2, q3);
    }
    Quaternion eulerAngles2Quat_XYX(Real phi, Real theta, Real psi)
    {
        const Real c1 = cos(0.5 * phi);
        const Real s1 = sin(0.5 * phi);
        const Real c2 = cos(0.5 * theta);
        const Real s2 = sin(0.5 * theta);
        const Real c3 = cos(0.5 * psi);
        const Real s3 = sin(0.5 * psi);
        const Real q0 = c1*c2*c3 - s1*c2*s3;
        const Real q1 = c1*c2*s3 + s1*c2*c3;
        const Real q2 = c1*s2*c3 + s1*s2*s3;
        const Real q3 = c1*s2*s3 - s1*s2*c3;
        return Quaternion(q0, q1, q2, q3);
    }
    Quaternion eulerAngles2Quat_YZY(Real phi, Real theta, Real psi)
    {
        const Real c1 = cos(0.5 * phi);
        const Real s1 = sin(0.5 * phi);
        const Real c2 = cos(0.5 * theta);
        const Real s2 = sin(0.5 * theta);
        const Real c3 = cos(0.5 * psi);
        const Real s3 = sin(0.5 * psi);
        const Real q0 = c1*c2*c3 - s1*c2*s3;
        const Real q1 = c1*s2*s3 - s1*s2*c3;
        const Real q2 = c1*c2*s3 + s1*c2*c3;
        const Real q3 = c1*s2*c3 + s1*s2*s3;
        return Quaternion(q0, q1, q2, q3);
    }
    Quaternion eulerAngles2Quat_ZYZ(Real phi, Real theta, Real psi)
    {
        const Real c1 = cos(0.5 * phi);
        const Real s1 = sin(0.5 * phi);
        const Real c2 = cos(0.5 * theta);
        const Real s2 = sin(0.5 * theta);
        const Real c3 = cos(0.5 * psi);
        const Real s3 = sin(0.5 * psi);
        const Real q0 = c1*c2*c3 - s1*c2*s3;
        const Real q1 = c1*s2*c3 + s1*s2*s3;
        const Real q2 = c1*s2*s3 - s1*s2*c3;
        const Real q3 = c1*c2*s3 + s1*c2*c3;
        return Quaternion(q0, q1, q2, q3);
    }
    Quaternion eulerAngles2Quat_XZX(Real phi, Real theta, Real psi)
    {
        const Real c1 = cos(0.5 * phi);
        const Real s1 = sin(0.5 * phi);
        const Real c2 = cos(0.5 * theta);
        const Real s2 = sin(0.5 * theta);
        const Real c3 = cos(0.5 * psi);
        const Real s3 = sin(0.5 * psi);
        const Real q0 = c1*c2*c3 - s1*c2*s3;
        const Real q1 = c1*c2*s3 + s1*c2*c3;
        const Real q2 = s1*s2*c3 - c1*s2*s3;
        const Real q3 = c1*s2*c3 + s1*s2*s3;
        return Quaternion(q0, q1, q2, q3);
    }
    Quaternion eulerAngles2Quat_YXY(Real phi, Real theta, Real psi)
    {
        const Real c1 = cos(0.5 * phi);
        const Real s1 = sin(0.5 * phi);
        const Real c2 = cos(0.5 * theta);
        const Real s2 = sin(0.5 * theta);
        const Real c3 = cos(0.5 * psi);
        const Real s3 = sin(0.5 * psi);
        const Real q0 = c1*c2*c3 - s1*c2*s3;
        const Real q1 = c1*s2*c3 + s1*s2*s3;
        const Real q2 = c1*c2*s3 + s1*c2*c3;
        const Real q3 = s1*s2*c3 - c1*s2*s3;
        return Quaternion(q0, q1, q2, q3);
    }
    Quaternion eulerAngles2Quat_XYZ(Real phi, Real theta, Real psi)
    {
        const Real c1 = cos(0.5 * phi);
        const Real s1 = sin(0.5 * phi);
        const Real c2 = cos(0.5 * theta);
        const Real s2 = sin(0.5 * theta);
        const Real c3 = cos(0.5 * psi);
        const Real s3 = sin(0.5 * psi);
        const Real q0 = c1*c2*c3 + s1*s2*s3;
        const Real q1 = s1*c2*c3 - c1*s2*s3;
        const Real q2 = c1*s2*c3 + s1*c2*s3;
        const Real q3 = c1*c2*s3 - s1*s2*c3;
        return Quaternion(q0, q1, q2, q3);
    }
    Quaternion eulerAngles2Quat_YZX(Real phi, Real theta, Real psi)
    {
        const Real c1 = cos(0.5 * phi);
        const Real s1 = sin(0.5 * phi);
        const Real c2 = cos(0.5 * theta);
        const Real s2 = sin(0.5 * theta);
        const Real c3 = cos(0.5 * psi);
        const Real s3 = sin(0.5 * psi);
        const Real q0 = c1*c2*c3 + s1*s2*s3;
        const Real q1 = c1*c2*s3 - s1*s2*c3;
        const Real q2 = s1*c2*c3 - c1*s2*s3;
        const Real q3 = c1*s2*c3 + s1*c2*s3;
        return Quaternion(q0, q1, q2, q3);
    }
    Quaternion eulerAngles2Quat_ZXY(Real phi, Real theta, Real psi)
    {
        const Real c1 = cos(0.5 * phi);
        const Real s1 = sin(0.5 * phi);
        const Real c2 = cos(0.5 * theta);
        const Real s2 = sin(0.5 * theta);
        const Real c3 = cos(0.5 * psi);
        const Real s3 = sin(0.5 * psi);
        const Real q0 = c1*c2*c3 + s1*s2*s3;
        const Real q1 = c1*s2*c3 + s1*c2*s3;
        const Real q2 = c1*c2*s3 - s1*s2*c3;
        const Real q3 = s1*c2*c3 - c1*s2*s3;
        return Quaternion(q0, q1, q2, q3);
    }
    Quaternion eulerAngles2Quat_XZY(Real phi, Real theta, Real psi)
    {
        const Real c1 = cos(0.5 * phi);
        const Real s1 = sin(0.5 * phi);
        const Real c2 = cos(0.5 * theta);
        const Real s2 = sin(0.5 * theta);
        const Real c3 = cos(0.5 * psi);
        const Real s3 = sin(0.5 * psi);
        const Real q0 = c1*c2*c3 - s1*s2*s3;
        const Real q1 = c1*s2*s3 + s1*c2*c3;
        const Real q2 = c1*c2*s3 + s1*s2*c3;
        const Real q3 = c1*s2*c3 - s1*c2*s3;
        return Quaternion(q0, q1, q2, q3);
    }
    Quaternion eulerAngles2Quat_ZYX(Real phi, Real theta, Real psi)
    {
        const Real c1 = cos(0.5 * phi);
        const Real s1 = sin(0.5 * phi);
        const Real c2 = cos(0.5 * theta);
        const Real s2 = sin(0.5 * theta);
        const Real c3 = cos(0.5 * psi);
        const Real s3 = sin(0.5 * psi);
        const Real q0 = c1*c2*c3 - s1*s2*s3;
        const Real q1 = c1*c2*s3 + s1*s2*c3;
        const Real q2 = c1*s2*c3 - s1*c2*s3;
        const Real q3 = c1*s2*s3 + s1*c2*c3;
        return Quaternion(q0, q1, q2, q3);
    }
    Quaternion eulerAngles2Quat_YXZ(Real phi, Real theta, Real psi)
    {
        const Real c1 = cos(0.5 * phi);
        const Real s1 = sin(0.5 * phi);
        const Real c2 = cos(0.5 * theta);
        const Real s2 = sin(0.5 * theta);
        const Real c3 = cos(0.5 * psi);
        const Real s3 = sin(0.5 * psi);
        const Real q0 = c1*c2*c3 - s1*s2*s3;
        const Real q1 = c1*s2*c3 - s1*c2*s3;
        const Real q2 = c1*s2*s3 + s1*c2*s3;
        const Real q3 = c1*c2*s3 + s1*s2*c3;
        return Quaternion(q0, q1, q2, q3);
    }

    // Quaternion to Euler Angle Conversions
    EulerAngles quat2EulerAngles_ZXZ(const Quaternion& quat)
    {
        const Real q0_2   = quat.q0*quat.q0;
        const Real q1_2   = quat.q1*quat.q1;
        const Real q2_2   = quat.q2*quat.q2;
        const Real q3_2   = quat.q3*quat.q3;
        const Real x2q1q3 = 2.0*quat.q1*quat.q3;
        const Real x2q0q2 = 2.0*quat.q0*quat.q2;
        const Real x2q2q3 = 2.0*quat.q2*quat.q3;
        const Real x2q0q1 = 2.0*quat.q0*quat.q1;
        const Real phi   = atan2(x2q1q3 - x2q0q2, x2q2q3 + x2q0q1);
        const Real theta = acos(q0_2 - q1_2 - q2_2 + q3_2);
        const Real psi   = atan2(x2q1q3 + x2q0q2, -(x2q2q3 - x2q0q1));   
        return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::ZXZ);
    }
    EulerAngles quat2EulerAngles_XYX(const Quaternion& quat)
    {
        const Real q0_2   = quat.q0*quat.q0;
        const Real q1_2   = quat.q1*quat.q1;
        const Real q2_2   = quat.q2*quat.q2;
        const Real q3_2   = quat.q3*quat.q3;
        const Real x2q1q2 = 2.0*quat.q1*quat.q2;
        const Real x2q0q3 = 2.0*quat.q0*quat.q3;
        const Real x2q1q3 = 2.0*quat.q1*quat.q3;
        const Real x2q0q2 = 2.0*quat.q0*quat.q2;
        const Real m11 = q0_2 + q1_2 - q2_2 - q3_2;
        const Real m12 = x2q1q2 + x2q0q3;
        const Real m13 = x2q1q3 - x2q0q2;
        const Real m21 = x2q1q2 - x2q0q3;
        const Real m31 = x2q1q3 + x2q0q2;
        const Real phi   = atan2(m21, m31);
        const Real theta = acos(m11);
        const Real psi   = atan2(m12, -m13);
        return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::XYX);
    }
    EulerAngles quat2EulerAngles_YZY(const Quaternion& quat)
    {
        const Real q0_2   = quat.q0*quat.q0;
        const Real q1_2   = quat.q1*quat.q1;
        const Real q2_2   = quat.q2*quat.q2;
        const Real q3_2   = quat.q3*quat.q3;
        const Real x2q1q2 = 2.0*quat.q1*quat.q2;
        const Real x2q0q3 = 2.0*quat.q0*quat.q3;
        const Real x2q2q3 = 2.0*quat.q2*quat.q3;
        const Real x2q0q1 = 2.0*quat.q0*quat.q1;
        const Real m12 = x2q1q2 + x2q0q3;
        const Real m21 = x2q1q2 - x2q0q3;
        const Real m22 = q0_2 - q1_2 + q2_2 + q3_2;
        const Real m23 = x2q2q3 + x2q0q1;
        const Real m32 = x2q2q3 - x2q0q1;
        const Real phi   = atan2(m32, m12);
        const Real theta = acos(m22);
        const Real psi   = atan2(m23, -m21);
        return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::YZY);

    }
    EulerAngles quat2EulerAngles_ZYZ(const Quaternion& quat)
    {
        const Real q0_2   = quat.q0*quat.q0;
        const Real q1_2   = quat.q1*quat.q1;
        const Real q2_2   = quat.q2*quat.q2;
        const Real q3_2   = quat.q3*quat.q3;
        const Real x2q1q3 = 2.0*quat.q1*quat.q3;
        const Real x2q0q2 = 2.0*quat.q0*quat.q2;
        const Real x2q2q3 = 2.0*quat.q2*quat.q3;
        const Real x2q0q1 = 2.0*quat.q0*quat.q1;
        const Real m13 = x2q1q3 - x2q0q2;
        const Real m23 = x2q2q3 + x2q0q1;
        const Real m31 = x2q1q3 + x2q0q2;
        const Real m32 = x2q2q3 - x2q0q1;
        const Real m33 = q0_2 - q1_2 - q2_2 + q3_2;
        const Real phi   = atan2(m23, -m13);
        const Real theta = acos(m33);
        const Real psi   = atan2(m32, m31);
        return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::ZYZ);
    }
    EulerAngles quat2EulerAngles_XZX(const Quaternion& quat)
    {
        const Real q0_2   = quat.q0*quat.q0;
        const Real q1_2   = quat.q1*quat.q1;
        const Real q2_2   = quat.q2*quat.q2;
        const Real q3_2   = quat.q3*quat.q3;
        const Real x2q1q2 = 2.0*quat.q1*quat.q2;
        const Real x2q0q3 = 2.0*quat.q0*quat.q3;
        const Real x2q1q3 = 2.0*quat.q1*quat.q3;
        const Real x2q0q2 = 2.0*quat.q0*quat.q2;
        const Real m11 = q0_2 + q1_2 - q2_2 - q3_2;
        const Real m12 = x2q1q2 + x2q0q3;
        const Real m13 = x2q1q3 - x2q0q2;
        const Real m21 = x2q1q2 - x2q0q3;
        const Real m31 = x2q1q3 + x2q0q2;
        const Real phi   = atan2(m31, -m21);
        const Real theta = acos(m11);
        const Real psi   = atan2(m13, m12);
        return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::XZX);
    }
    EulerAngles quat2EulerAngles_YXY(const Quaternion& quat)
    {
        const Real q0_2   = quat.q0*quat.q0;
        const Real q1_2   = quat.q1*quat.q1;
        const Real q2_2   = quat.q2*quat.q2;
        const Real q3_2   = quat.q3*quat.q3;
        const Real x2q1q2 = 2.0*quat.q1*quat.q2;
        const Real x2q0q3 = 2.0*quat.q0*quat.q3;
        const Real x2q2q3 = 2.0*quat.q2*quat.q3;
        const Real x2q0q1 = 2.0*quat.q0*quat.q1;
        const Real m12 = x2q1q2 + x2q0q3;
        const Real m21 = x2q1q2 - x2q0q3;
        const Real m22 = q0_2 - q1_2 + q2_2 + q3_2;
        const Real m23 = x2q2q3 + x2q0q1;
        const Real m32 = x2q2q3 - x2q0q1;
        const Real phi   = atan2(m12, -m32);
        const Real theta = acos(m22);
        const Real psi   = atan2(m21, m23);
        return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::YXY);
    }
    EulerAngles quat2EulerAngles_XYZ(const Quaternion& quat)
    {
        const Real q0_2   = quat.q0*quat.q0;
        const Real q1_2   = quat.q1*quat.q1;
        const Real q2_2   = quat.q2*quat.q2;
        const Real q3_2   = quat.q3*quat.q3;
        const Real x2q1q2 = 2.0*quat.q1*quat.q2;
        const Real x2q0q3 = 2.0*quat.q0*quat.q3;
        const Real x2q1q3 = 2.0*quat.q1*quat.q3;
        const Real x2q0q2 = 2.0*quat.q0*quat.q2;
        const Real x2q2q3 = 2.0*quat.q2*quat.q3;
        const Real x2q0q1 = 2.0*quat.q0*quat.q1;
        const Real m11 = q0_2 + q1_2 - q2_2 - q3_2;
        const Real m12 = x2q1q2 + x2q0q3;
        const Real m13 = x2q1q3 - x2q0q2;
        const Real m23 = x2q2q3 + x2q0q1;
        const Real m33 = q0_2 - q1_2 - q2_2 + q3_2;
        const Real phi   = atan2(m23, m33);
        const Real theta = -asin(m13);
        const Real psi   = atan2(m12, m11);   
        return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::XYZ);
    }
    EulerAngles quat2EulerAngles_YZX(const Quaternion& quat)
    {
        const Real q0_2   = quat.q0*quat.q0;
        const Real q1_2   = quat.q1*quat.q1;
        const Real q2_2   = quat.q2*quat.q2;
        const Real q3_2   = quat.q3*quat.q3;
        const Real x2q1q2 = 2.0*quat.q1*quat.q2;
        const Real x2q0q3 = 2.0*quat.q0*quat.q3;
        const Real x2q1q3 = 2.0*quat.q1*quat.q3;
        const Real x2q0q2 = 2.0*quat.q0*quat.q2;
        const Real x2q2q3 = 2.0*quat.q2*quat.q3;
        const Real x2q0q1 = 2.0*quat.q0*quat.q1;
        const Real m11 = q0_2 + q1_2 - q2_2 - q3_2;
        const Real m21 = x2q1q2 - x2q0q3;
        const Real m22 = q0_2 - q1_2 + q2_2 + q3_2;
        const Real m23 = x2q2q3 + x2q0q1;
        const Real m31 = x2q1q3 + x2q0q2;
        const Real phi   = atan2(m31, m11);
        const Real theta = -asin(m21);
        const Real psi   = atan2(m23, m22);
        return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::YZX);
    }
    EulerAngles quat2EulerAngles_ZXY(const Quaternion& quat)
    {
        const Real q0_2   = quat.q0*quat.q0;
        const Real q1_2   = quat.q1*quat.q1;
        const Real q2_2   = quat.q2*quat.q2;
        const Real q3_2   = quat.q3*quat.q3;
        const Real x2q1q2 = 2.0*quat.q1*quat.q2;
        const Real x2q0q3 = 2.0*quat.q0*quat.q3;
        const Real x2q1q3 = 2.0*quat.q1*quat.q3;
        const Real x2q0q2 = 2.0*quat.q0*quat.q2;
        const Real x2q2q3 = 2.0*quat.q2*quat.q3;
        const Real x2q0q1 = 2.0*quat.q0*quat.q1;
        const Real m12 = x2q1q2 + x2q0q3;
        const Real m22 = q0_2 - q1_2 + q2_2 + q3_2;
        const Real m31 = x2q1q3 + x2q0q2;
        const Real m32 = x2q2q3 - x2q0q1;
        const Real m33 = q0_2 - q1_2 - q2_2 + q3_2;
        const Real phi   = atan2(m12, m22);
        const Real theta = -asin(m32);
        const Real psi   = atan2(m31, m33);
        return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::ZXY);
    }
    EulerAngles quat2EulerAngles_XZY(const Quaternion& quat)
    {
        const Real q0_2   = quat.q0*quat.q0;
        const Real q1_2   = quat.q1*quat.q1;
        const Real q2_2   = quat.q2*quat.q2;
        const Real q3_2   = quat.q3*quat.q3;
        const Real x2q1q2 = 2.0*quat.q1*quat.q2;
        const Real x2q0q3 = 2.0*quat.q0*quat.q3;
        const Real x2q1q3 = 2.0*quat.q1*quat.q3;
        const Real x2q0q2 = 2.0*quat.q0*quat.q2;
        const Real x2q2q3 = 2.0*quat.q2*quat.q3;
        const Real x2q0q1 = 2.0*quat.q0*quat.q1;
        const Real m11 = q0_2 + q1_2 - q2_2 - q3_2;
        const Real m12 = x2q1q2 + x2q0q3;
        const Real m13 = x2q1q3 - x2q0q2;
        const Real m22 = q0_2 - q1_2 + q2_2 + q3_2;
        const Real m32 = x2q2q3 - x2q0q1;
        const Real phi   = atan2(-m32, m22);
        const Real theta = asin(m12);
        const Real psi   = atan2(-m13, m11);
        return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::XZY);
    }
    EulerAngles quat2EulerAngles_ZYX(const Quaternion& quat)
    {
        const Real q0_2   = quat.q0*quat.q0;
        const Real q1_2   = quat.q1*quat.q1;
        const Real q2_2   = quat.q2*quat.q2;
        const Real q3_2   = quat.q3*quat.q3;
        const Real x2q1q2 = 2.0*quat.q1*quat.q2;
        const Real x2q0q3 = 2.0*quat.q0*quat.q3;
        const Real x2q1q3 = 2.0*quat.q1*quat.q3;
        const Real x2q0q2 = 2.0*quat.q0*quat.q2;
        const Real x2q2q3 = 2.0*quat.q2*quat.q3;
        const Real x2q0q1 = 2.0*quat.q0*quat.q1;
        const Real m11 = q0_2 + q1_2 - q2_2 - q3_2;
        const Real m21 = x2q1q2 - x2q0q3;
        const Real m31 = x2q1q3 + x2q0q2;
        const Real m32 = x2q2q3 - x2q0q1;
        const Real m33 = q0_2 - q1_2 - q2_2 + q3_2;
        const Real phi   = atan2(-m21, m11);
        const Real theta = asin(m31);
        const Real psi   = atan2(-m32, m33);
        return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::ZYX);
    }
    EulerAngles quat2EulerAngles_YXZ(const Quaternion& quat)
    {
        const Real q0_2   = quat.q0*quat.q0;
        const Real q1_2   = quat.q1*quat.q1;
        const Real q2_2   = quat.q2*quat.q2;
        const Real q3_2   = quat.q3*quat.q3;
        const Real x2q1q2 = 2.0*quat.q1*quat.q2;
        const Real x2q0q3 = 2.0*quat.q0*quat.q3;
        const Real x2q1q3 = 2.0*quat.q1*quat.q3;
        const Real x2q0q2 = 2.0*quat.q0*quat.q2;
        const Real x2q2q3 = 2.0*quat.q2*quat.q3;
        const Real x2q0q1 = 2.0*quat.q0*quat.q1;
        const Real m13 = x2q1q3 - x2q0q2;
        const Real m21 = x2q1q2 - x2q0q3;
        const Real m22 = q0_2 - q1_2 + q2_2 + q3_2;
        const Real m23 = x2q2q3 + x2q0q1;
        const Real m33 = q0_2 - q1_2 - q2_2 + q3_2;
        const Real phi   = atan2(-m13, m33);
        const Real theta = asin(m23);
        const Real psi   = atan2(-m21, m22);
        return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::YXZ);
    }

    // Quaternion Kinematic Functions
    Quaternion integrateQuat(const Quaternion& quat, const Quaternion& quatRates, Real dt)
    {
        Quaternion quatNew = quat + quatRates * dt; // First Order Euler Integration
        normalise(quatNew);                         // Normalisation
        return quatNew;
    }

    Quaternion quatKinematicRates_BodyRates(const Quaternion& quat, const Vector3& bodyRates)
    {
        Real p = bodyRates.x;
        Real q = bodyRates.y;
        Real r = bodyRates.z;
        Real q0 = 0.5 * (-quat.q1 * p - quat.q2 * q - quat.q3 * r);
        Real q1 = 0.5 * ( quat.q0 * p + quat.q3 * q - quat.q2 * r);
        Real q2 = 0.5 * (-quat.q3 * p + quat.q0 * q + quat.q1 * r);
        Real q3 = 0.5 * ( quat.q2 * p - quat.q1 * q + quat.q0 * r);
        return Quaternion(q0, q1, q2, q3);
    }
    Quaternion quatKinematicRates_WorldRates(const Quaternion& quat, const Vector3& worldRates)
    {
        Real p = worldRates.x;
        Real q = worldRates.y;
        Real r = worldRates.z;
        Real q0 = 0.5 * (-quat.q1 * p - quat.q2 * q - quat.q3 * r);
        Real q1 = 0.5 * ( quat.q0 * p - quat.q3 * q + quat.q2 * r);
        Real q2 = 0.5 * ( quat.q3 * p + quat.q0 * q - quat.q1 * r);
        Real q3 = 0.5 * (-quat.q2 * p + quat.q1 * q + quat.q0 * r);
        return Quaternion(q0, q1, q2, q3);
    }

    // Quaternion Interpolation Functions
    Quaternion linearInterpolate(const Quaternion& startQuat, const Quaternion& endQuat, Real t)
    {
        Quaternion q0 = unit(startQuat);
        Quaternion q1 = unit(endQuat);

        if (t < 0.0){return q0;}
        if (t > 1.0){return q1;}

        Real a = (1.0 - t);
        Real b = t;
        Quaternion qi = unit(a*q0 + b*q1);
        return qi;
    }
    Quaternion slerpInterpolate(const Quaternion& startQuat, const Quaternion& endQuat, Real t)
    {
        Quaternion q0 = unit(startQuat);
        Quaternion q1 = unit(endQuat);

        if (t < 0.0){return q0;}
        if (t > 1.0){return q1;}

        Real quatDot = dot(q0,q1);

        // Check for Negative Dot Product
        if(quatDot < 0)
        {
            q1 = -q1;
            quatDot = -quatDot;
        }

        Real theta = acos(quatDot);

        // Check for Small Angles
        if (theta < 0.0001)
        {
            return linearInterpolate(startQuat, endQuat, t);
        }

        // SLERP
        Real a = sin((1.0-t)*theta) / sin(theta);
        Real b = sin(t*theta) / sin(theta);
        Quaternion qt = unit(a*q0 + b*q1);
        return qt;
    }
  }
}