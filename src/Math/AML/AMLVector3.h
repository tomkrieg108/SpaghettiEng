#pragma once
#include "Math/MathX.h"

#include <iostream>

namespace MathX
{
  namespace AML
  {
      class Vector3
      {
          public:

              // Public Element Data
              union
              {
                  Real data[3];
                  struct{Real x, y, z;};
              };
              
              // Constructors
              Vector3();
              Vector3(Real val);
              Vector3(Real x, Real y, Real z);
              Vector3(const Real data[3]);

              // Operator Assignments (Vector)
              Vector3& operator+=(const Vector3& rhs);
              Vector3& operator-=(const Vector3& rhs);
              Vector3& operator*=(const Vector3& rhs);
              Vector3& operator/=(const Vector3& rhs);

              // Operator Assignments (Scalar)
              Vector3& operator+=(Real s);
              Vector3& operator-=(Real s);
              Vector3& operator*=(Real s);
              Vector3& operator/=(Real s);

              // Special Object Creators
              static const Vector3 xAxis();
              static const Vector3 yAxis();
              static const Vector3 zAxis();

      };

      // Vector / Vector Elementwise Operations
      Vector3 operator-(const Vector3& rhs);
      Vector3 operator+(const Vector3& lhs, const Vector3& rhs);
      Vector3 operator-(const Vector3& lhs, const Vector3& rhs);
      Vector3 operator*(const Vector3& lhs, const Vector3& rhs);
      Vector3 operator/(const Vector3& lhs, const Vector3& rhs);

      // Vector / Scalar Operations
      Vector3 operator+(const Vector3& lhs, Real s);
      Vector3 operator-(const Vector3& lhs, Real s);
      Vector3 operator*(const Vector3& lhs, Real s);
      Vector3 operator/(const Vector3& lhs, Real s);
      Vector3 operator+(Real s, const Vector3& rhs);
      Vector3 operator-(Real s, const Vector3& rhs);
      Vector3 operator*(Real s, const Vector3& rhs);
      Vector3 operator/(Real s, const Vector3& rhs);

      // Vector Operations
      Real norm(const Vector3& rhs);
      void normalise(Vector3& rhs);
      Vector3 unit(const Vector3& rhs);
      Vector3 cross(const Vector3& lhs, const Vector3& rhs);
      Real dot(const Vector3& lhs, const Vector3& rhs);

      // Stream Functions
      std::ostream& operator<<(std::ostream& os, const Vector3& obj);

  } //AML

} //MathX