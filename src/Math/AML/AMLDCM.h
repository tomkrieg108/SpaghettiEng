#pragma once
#include "Math/MathX.h"
#include "AMLMatrix33.h"
#include "AMLVector3.h"
#include <limits>

namespace MathX
{
  namespace AML
  {
    class DCM
    {
      public:

        static const Matrix33 rotationX(Real theta);
        static const Matrix33 rotationY(Real theta);
        static const Matrix33 rotationZ(Real theta);
    };

    bool isValidDCM(const Matrix33& dcm, Real tol = std::numeric_limits<Real>::epsilon());
    void normalise(Matrix33& dcm);

    // DCM Kinematic Functions
    Matrix33 integrateDCM(const Matrix33& dcm, const Matrix33& dcmRates, Real dt);
    Matrix33 dcmKinematicRates_BodyRates(const Matrix33& dcm, const Vector3& bodyRates);
    Matrix33 dcmKinematicRates_WorldRates(const Matrix33& dcm, const Vector3& worldRates);
  }
}