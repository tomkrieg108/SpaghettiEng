#include "Math/AML/AMLEulerAngles.h"
#include "Math/AML/AMLVector3.h"
#include "Math/AML/AMLMatrix33.h"
#include "Math/AML/AMLDCM.h"
#include <cmath>

namespace MathX
{

  namespace AML
  {
      EulerAngles::EulerAngles()
      :phi(0.0), theta(0.0), psi(0.0),
      seq_(EulerSequence::XYZ)
      {}

      EulerAngles::EulerAngles(Real phi_, Real theta_, Real psi_, EulerSequence seq)
      :phi(phi_), theta(theta_), psi(psi_),
      seq_(seq)
      {}

      // Steam Functions
      std::ostream& operator<<(std::ostream& os, const EulerAngles& obj)
      {
          os << "EULER ["<< obj.phi << ", " << obj.theta << ", " << obj.psi << "]";
          return os;
      }

      // Euler Angle Conversions
      Matrix33 eulerAngles2DCM(const EulerAngles& angles)
      {
          switch(angles.getEulerSequence())
          {
              case EulerAngles::EulerSequence::XYZ:
                  return eulerAngles2DCM_XYZ(angles.phi, angles.theta, angles.psi);
              case EulerAngles::EulerSequence::ZXZ:
                  return eulerAngles2DCM_ZXZ(angles.phi, angles.theta, angles.psi);
              case EulerAngles::EulerSequence::XYX:
                  return eulerAngles2DCM_XYX(angles.phi, angles.theta, angles.psi);
              case EulerAngles::EulerSequence::YZY:
                  return eulerAngles2DCM_YZY(angles.phi, angles.theta, angles.psi);
              case EulerAngles::EulerSequence::ZYZ:
                  return eulerAngles2DCM_ZYZ(angles.phi, angles.theta, angles.psi);
              case EulerAngles::EulerSequence::XZX:
                  return eulerAngles2DCM_XZX(angles.phi, angles.theta, angles.psi);
              case EulerAngles::EulerSequence::YXY:
                  return eulerAngles2DCM_YXY(angles.phi, angles.theta, angles.psi);
              case EulerAngles::EulerSequence::YZX:
                  return eulerAngles2DCM_YZX(angles.phi, angles.theta, angles.psi);
              case EulerAngles::EulerSequence::ZXY:
                  return eulerAngles2DCM_ZXY(angles.phi, angles.theta, angles.psi);
              case EulerAngles::EulerSequence::XZY:
                  return eulerAngles2DCM_XZY(angles.phi, angles.theta, angles.psi);
              case EulerAngles::EulerSequence::ZYX:
                  return eulerAngles2DCM_ZYX(angles.phi, angles.theta, angles.psi);
              case EulerAngles::EulerSequence::YXZ:
                  return eulerAngles2DCM_YXZ(angles.phi, angles.theta, angles.psi);
          }
          return Matrix33::identity();
      }

      EulerAngles dcm2EulerAngles(const Matrix33& dcm, const EulerAngles::EulerSequence seq)
      {
          const Real TOL = 0.0001;

          if (isValidDCM(dcm, TOL))
          {
              switch(seq)
              {
                  case EulerAngles::EulerSequence::XYZ:
                      return dcm2EulerAngles_XYZ(dcm);
                  case EulerAngles::EulerSequence::ZXZ:
                      return dcm2EulerAngles_ZXZ(dcm);
                  case EulerAngles::EulerSequence::XYX:
                      return dcm2EulerAngles_XYX(dcm);
                  case EulerAngles::EulerSequence::YZY:
                      return dcm2EulerAngles_YZY(dcm);
                  case EulerAngles::EulerSequence::ZYZ:
                      return dcm2EulerAngles_ZYZ(dcm);
                  case EulerAngles::EulerSequence::XZX:
                      return dcm2EulerAngles_XZX(dcm);
                  case EulerAngles::EulerSequence::YXY:
                      return dcm2EulerAngles_YXY(dcm);
                  case EulerAngles::EulerSequence::YZX:
                      return dcm2EulerAngles_YZX(dcm);
                  case EulerAngles::EulerSequence::ZXY:
                      return dcm2EulerAngles_ZXY(dcm);
                  case EulerAngles::EulerSequence::XZY:
                      return dcm2EulerAngles_XZY(dcm);
                  case EulerAngles::EulerSequence::ZYX:
                      return dcm2EulerAngles_ZYX(dcm);
                  case EulerAngles::EulerSequence::YXZ:
                      return dcm2EulerAngles_YXZ(dcm);
              }
          }
          return EulerAngles();
      }

      EulerAngles convertEulerAngleSequence(const EulerAngles& angles, const EulerAngles::EulerSequence seq)
      {
          if (angles.getEulerSequence() == seq)
          {
              return angles;
          }
          else if (angles.getEulerSequence() == EulerAngles::EulerSequence::XYZ && 
              seq == EulerAngles::EulerSequence::ZXZ)
          {
              return converEulerAnglesXYZtoZXZ(angles.phi, angles.theta, angles.psi);
          }
          else if (angles.getEulerSequence() == EulerAngles::EulerSequence::ZXZ && 
                  seq == EulerAngles::EulerSequence::XYZ)
          {
              return converEulerAnglesZXZtoXYZ(angles.phi, angles.theta, angles.psi);
          }
          else // General Case
          {
              Matrix33 dcm = eulerAngles2DCM(angles);
              return dcm2EulerAngles(dcm, seq);
          }
      }

      // Euler Angle to DCM Conversions
      Matrix33 eulerAngles2DCM_XYZ(Real phi, Real theta, Real psi)
      {
          Real data[9];
          Real cosPhi   = cos(phi);
          Real sinPhi   = sin(phi);
          Real cosTheta = cos(theta);
          Real sinTheta = sin(theta);
          Real cosPsi   = cos(psi);
          Real sinPsi   = sin(psi);
          data[0] = cosPsi*cosTheta;
          data[1] = cosTheta*sinPsi;
          data[2] = -sinTheta;
          data[3] = cosPsi*sinTheta*sinPhi - sinPsi*cosPhi;
          data[4] = sinPsi*sinTheta*sinPhi + cosPsi*cosPhi;
          data[5] = sinPhi*cosTheta;
          data[6] = cosPsi*sinTheta*cosPhi + sinPsi*sinPhi;
          data[7] = sinPsi*sinTheta*cosPhi - cosPsi*sinPhi;
          data[8] = cosTheta*cosPhi;
          return Matrix33(data);
      }
      Matrix33 eulerAngles2DCM_ZXZ(Real phi, Real theta, Real psi)
      {
          Real data[9];
          Real cosPhi = cos(phi);
          Real sinPhi = sin(phi);
          Real cosTheta = cos(theta);
          Real sinTheta = sin(theta);
          Real cosPsi = cos(psi);
          Real sinPsi = sin(psi);
          data[0] = cosPhi*cosPsi - sinPhi*cosTheta*sinPsi;
          data[1] = cosPhi*sinPsi + sinPhi*cosTheta*cosPsi;
          data[2] = sinPhi*sinTheta;
          data[3] = -sinPhi*cosPsi - cosPhi*cosTheta*sinPsi;
          data[4] = -sinPhi*sinPsi + cosPhi*cosTheta*cosPsi;
          data[5] = cosPhi*sinTheta;
          data[6] = sinTheta*sinPsi;
          data[7] = -sinTheta*cosPsi;
          data[8] = cosTheta;
          return Matrix33(data);
      }
      Matrix33 eulerAngles2DCM_XYX(Real phi, Real theta, Real psi){return (DCM::rotationX(phi) * DCM::rotationY(theta) * DCM::rotationX(psi));}
      Matrix33 eulerAngles2DCM_YZY(Real phi, Real theta, Real psi){return (DCM::rotationY(phi) * DCM::rotationZ(theta) * DCM::rotationY(psi));}
      Matrix33 eulerAngles2DCM_ZYZ(Real phi, Real theta, Real psi){return (DCM::rotationZ(phi) * DCM::rotationY(theta) * DCM::rotationZ(psi));}
      Matrix33 eulerAngles2DCM_XZX(Real phi, Real theta, Real psi){return (DCM::rotationX(phi) * DCM::rotationZ(theta) * DCM::rotationX(psi));}
      Matrix33 eulerAngles2DCM_YXY(Real phi, Real theta, Real psi){return (DCM::rotationY(phi) * DCM::rotationX(theta) * DCM::rotationY(psi));}
      Matrix33 eulerAngles2DCM_YZX(Real phi, Real theta, Real psi){return (DCM::rotationY(phi) * DCM::rotationZ(theta) * DCM::rotationX(psi));}
      Matrix33 eulerAngles2DCM_ZXY(Real phi, Real theta, Real psi){return (DCM::rotationZ(phi) * DCM::rotationX(theta) * DCM::rotationY(psi));}
      Matrix33 eulerAngles2DCM_XZY(Real phi, Real theta, Real psi){return (DCM::rotationX(phi) * DCM::rotationZ(theta) * DCM::rotationY(psi));}
      Matrix33 eulerAngles2DCM_ZYX(Real phi, Real theta, Real psi){return (DCM::rotationZ(phi) * DCM::rotationY(theta) * DCM::rotationX(psi));}
      Matrix33 eulerAngles2DCM_YXZ(Real phi, Real theta, Real psi){return (DCM::rotationY(phi) * DCM::rotationX(theta) * DCM::rotationZ(psi));}

      // DCM to Euler Angle Conversions
      EulerAngles dcm2EulerAngles_XYZ(const Matrix33& dcm)
      {
          Real phi   = atan2(dcm.m23, dcm.m33);
          Real theta = -asin(dcm.m13);
          Real psi   = atan2(dcm.m12, dcm.m11);   
          return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::XYZ);
      }
      EulerAngles dcm2EulerAngles_ZXZ(const Matrix33& dcm)
      {
          Real phi   = atan2(dcm.m13, dcm.m23);
          Real theta = acos(dcm.m33);
          Real psi   = atan2(dcm.m31, -dcm.m32);   
          return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::ZXZ);
      }
      EulerAngles dcm2EulerAngles_XYX(const Matrix33& dcm)
      {
          Real phi   = atan2(dcm.m21, dcm.m31);
          Real theta = acos(dcm.m11);
          Real psi   = atan2(dcm.m12, -dcm.m13);
          return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::XYX);
      }
      EulerAngles dcm2EulerAngles_YZY(const Matrix33& dcm)
      {
          Real phi   = atan2(dcm.m32, dcm.m12);
          Real theta = acos(dcm.m22);
          Real psi   = atan2(dcm.m23, -dcm.m21);
          return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::YZY);
      }
      EulerAngles dcm2EulerAngles_ZYZ(const Matrix33& dcm)
      {
          Real phi   = atan2(dcm.m23, -dcm.m13);
          Real theta = acos(dcm.m33);
          Real psi   = atan2(dcm.m32, dcm.m31);
          return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::ZYZ);
      }
      EulerAngles dcm2EulerAngles_XZX(const Matrix33& dcm)
      {
          Real phi   = atan2(dcm.m31, -dcm.m21);
          Real theta = acos(dcm.m11);
          Real psi   = atan2(dcm.m13, dcm.m12);
          return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::XZX);
      }
      EulerAngles dcm2EulerAngles_YXY(const Matrix33& dcm)
      {
          Real phi   = atan2(dcm.m12, -dcm.m32);
          Real theta = acos(dcm.m22);
          Real psi   = atan2(dcm.m21, dcm.m23);
          return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::YXY);
      }
      EulerAngles dcm2EulerAngles_YZX(const Matrix33& dcm)
      {
          Real phi   = atan2(dcm.m31, dcm.m11);
          Real theta = -asin(dcm.m21);
          Real psi   = atan2(dcm.m23, dcm.m22);
          return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::YZX);
      }
      EulerAngles dcm2EulerAngles_ZXY(const Matrix33& dcm)
      {
          Real phi   = atan2(dcm.m12, dcm.m22);
          Real theta = -asin(dcm.m32);
          Real psi   = atan2(dcm.m31, dcm.m33);
          return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::ZXY);
      }
      EulerAngles dcm2EulerAngles_XZY(const Matrix33& dcm)
      {
          Real phi   = atan2(-dcm.m32, dcm.m22);
          Real theta = asin(dcm.m12);
          Real psi   = atan2(-dcm.m13, dcm.m11);
          return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::XZY);
      }
      EulerAngles dcm2EulerAngles_ZYX(const Matrix33& dcm)
      {
          Real phi   = atan2(-dcm.m21, dcm.m11);
          Real theta = asin(dcm.m31);
          Real psi   = atan2(-dcm.m32, dcm.m33);
          return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::ZYX);
      }
      EulerAngles dcm2EulerAngles_YXZ(const Matrix33& dcm)
      {
          Real phi   = atan2(-dcm.m13, dcm.m33);
          Real theta = asin(dcm.m23);
          Real psi   = atan2(-dcm.m21, dcm.m22);
          return EulerAngles(phi, theta, psi, EulerAngles::EulerSequence::YXZ);
      }

      // Euler Angle Sequence Conversions (algebraic method)
      EulerAngles converEulerAnglesXYZtoZXZ(Real phi, Real theta, Real psi)
      {
          const Real cosPhi   = cos(phi);
          const Real sinPhi   = sin(phi);
          const Real cosTheta = cos(theta);
          const Real sinTheta = sin(theta);
          const Real cosPsi   = cos(psi);
          const Real sinPsi   = sin(psi);
          const Real phiZXZ   = atan2(-sinTheta, sinPhi*cosTheta);
          const Real thetaZXZ = acos(cosPhi*cosTheta);
          const Real psiZXZ   = atan2(cosPhi*sinTheta*cosPsi + sinPhi*sinPsi, -cosPhi*sinTheta*sinPsi + sinPhi*cosPsi);
          return EulerAngles(phiZXZ, thetaZXZ, psiZXZ, EulerAngles::EulerSequence::ZXZ);
      }
      EulerAngles converEulerAnglesZXZtoXYZ(Real phi, Real theta, Real psi)
      {
          const Real cosPhi   = cos(phi);
          const Real sinPhi   = sin(phi);
          const Real cosTheta = cos(theta);
          const Real sinTheta = sin(theta);
          const Real cosPsi   = cos(psi);
          const Real sinPsi   = sin(psi);
          const Real phiXYZ   = atan2(cosPhi*sinTheta, cosTheta);
          const Real thetaXYZ = -asin(sinPhi*sinTheta);
          const Real psiXYZ   = atan2(cosPhi*sinPsi + sinPhi*cosTheta*cosPsi, cosPhi*cosPsi - sinPhi*cosTheta*sinPsi);
          return EulerAngles(phiXYZ, thetaXYZ, psiXYZ, EulerAngles::EulerSequence::XYZ);
      }

      // Euler Angle Rates
      EulerAngles integrateEulerAngles(const EulerAngles& angles, const EulerAngles& angleRates, Real dt)
      {
          EulerAngles::EulerSequence seq = angles.getEulerSequence();
          if (seq == angleRates.getEulerSequence())
          {
              Real phiNew   = angles.phi + angleRates.phi * dt;
              Real thetaNew = angles.theta + angleRates.theta * dt;
              Real psiNew   = angles.psi + angleRates.psi * dt;
              return EulerAngles(phiNew, thetaNew, psiNew, seq);
          }
          return angles;
      }

      EulerAngles eulerAngleKinematicRates(const EulerAngles& angles, const Vector3& bodyRates)
      {
          Vector3 eulerRates;
          EulerAngles::EulerSequence seq = angles.getEulerSequence();
          switch(seq)
          {
              case EulerAngles::EulerSequence::XYZ:
                  eulerRates = eulerAngleRatesMatrix_XYZ(angles.phi, angles.theta, angles.psi) * bodyRates;
                  break;  
              case EulerAngles::EulerSequence::ZXZ:
                  eulerRates = eulerAngleRatesMatrix_ZXZ(angles.phi, angles.theta, angles.psi) * bodyRates;
                  break; 
              case EulerAngles::EulerSequence::YZY:
                  eulerRates = eulerAngleRatesMatrix_YZY(angles.phi, angles.theta, angles.psi) * bodyRates;
                  break; 
              case EulerAngles::EulerSequence::XYX:
                  eulerRates = eulerAngleRatesMatrix_XYX(angles.phi, angles.theta, angles.psi) * bodyRates;
                  break; 
              case EulerAngles::EulerSequence::ZYZ:
                  eulerRates = eulerAngleRatesMatrix_ZYZ(angles.phi, angles.theta, angles.psi) * bodyRates;
                  break; 
              case EulerAngles::EulerSequence::XZX:
                  eulerRates = eulerAngleRatesMatrix_XZX(angles.phi, angles.theta, angles.psi) * bodyRates;
                  break; 
              case EulerAngles::EulerSequence::YXY:
                  eulerRates = eulerAngleRatesMatrix_YXY(angles.phi, angles.theta, angles.psi) * bodyRates;
                  break; 
              case EulerAngles::EulerSequence::YZX:
                  eulerRates = eulerAngleRatesMatrix_YZX(angles.phi, angles.theta, angles.psi) * bodyRates;
                  break; 
              case EulerAngles::EulerSequence::ZXY:
                  eulerRates = eulerAngleRatesMatrix_ZXY(angles.phi, angles.theta, angles.psi) * bodyRates;
                  break; 
              case EulerAngles::EulerSequence::XZY:
                  eulerRates = eulerAngleRatesMatrix_XZY(angles.phi, angles.theta, angles.psi) * bodyRates;
                  break; 
              case EulerAngles::EulerSequence::ZYX:
                  eulerRates = eulerAngleRatesMatrix_ZYX(angles.phi, angles.theta, angles.psi) * bodyRates;
                  break; 
              case EulerAngles::EulerSequence::YXZ:
                  eulerRates = eulerAngleRatesMatrix_YXZ(angles.phi, angles.theta, angles.psi) * bodyRates;
                  break; 
          }
          return EulerAngles(eulerRates.x, eulerRates.y, eulerRates.z, seq);
      }

      Matrix33 eulerAngleRatesMatrix_XYZ(Real phi, Real theta, Real psi)
      {
          const Real cosPhi   = cos(phi);
          const Real sinPhi   = sin(phi);
          const Real cosTheta = cos(theta);
          const Real tanTheta = tan(theta);
          const Real secTheta = 1.0 / cosTheta;
          
          Real data[3][3] = {{1.0, sinPhi*tanTheta, cosPhi*tanTheta},
                              {0.0, cosPhi, -sinPhi},
                              {0.0, sinPhi*secTheta, cosPhi*secTheta}};
                              
          return Matrix33(data);
      }
      Matrix33 eulerAngleRatesMatrix_ZXZ(Real phi, Real theta, Real psi)
      {
          const Real cosPhi   = cos(phi);
          const Real sinPhi   = sin(phi);
          const Real cosTheta = cos(theta);
          const Real sinTheta = sin(theta);
          const Real cscTheta = 1.0 / sinTheta;
          
          Real data[3][3] = {{-sinPhi*cosTheta*cscTheta, -cosPhi*cosTheta*cscTheta, sinTheta*cscTheta},
                              {cosPhi*sinTheta*cscTheta, -sinPhi*sinTheta*cscTheta, 0.0},
                              {sinPhi*cscTheta, cosPhi*cscTheta, 0.0}};
                              
          return Matrix33(data);
      }
      Matrix33 eulerAngleRatesMatrix_YZY(Real phi, Real theta, Real psi)
      {
          const Real cosPhi   = cos(phi);
          const Real sinPhi   = sin(phi);
          const Real cosTheta = cos(theta);
          const Real sinTheta = sin(theta);
          const Real cscTheta = 1.0 / sinTheta;
          
          Real data[3][3] = {{-cosPhi*cosTheta*cscTheta, sinTheta*cscTheta, -sinPhi*cosTheta*cscTheta},
                              {-sinPhi*sinTheta*cscTheta, 0.0, cosPhi*sinTheta*cscTheta},
                              {cosPhi*cscTheta, 0.0, sinPhi*cscTheta}};
                              
          return Matrix33(data);
      }
      Matrix33 eulerAngleRatesMatrix_ZYZ(Real phi, Real theta, Real psi)
      {
          const Real cosPhi   = cos(phi);
          const Real sinPhi   = sin(phi);
          const Real cosTheta = cos(theta);
          const Real sinTheta = sin(theta);
          const Real cscTheta = 1.0 / sinTheta;
          
          Real data[3][3] = {{cosPhi*cosTheta*cscTheta, -sinPhi*cosTheta*cscTheta, sinTheta},
                              {sinPhi*sinTheta*cscTheta, cosPhi*sinTheta*cscTheta, 0.0},
                              {-cosPhi*cscTheta, sinPhi*cscTheta, 0.0}};
                              
          return Matrix33(data);
      }
      Matrix33 eulerAngleRatesMatrix_XYX(Real phi, Real theta, Real psi)
      {
          const Real cosPhi   = cos(phi);
          const Real sinPhi   = sin(phi);
          const Real cosTheta = cos(theta);
          const Real sinTheta = sin(theta);
          const Real cscTheta = 1.0 / sinTheta;
          
          Real data[3][3] = {{sinTheta*cscTheta, -sinPhi*cosTheta*cscTheta, -cosPhi*cosTheta*cscTheta},
                              {0.0, cosPhi*sinTheta*cscTheta, -sinPhi*sinTheta*cscTheta},
                              {0.0, sinPhi*cscTheta, cosPhi*cscTheta}};
                              
          return Matrix33(data);
      }
      Matrix33 eulerAngleRatesMatrix_XZX(Real phi, Real theta, Real psi)
      {
          const Real cosPhi   = cos(phi);
          const Real sinPhi   = sin(phi);
          const Real cosTheta = cos(theta);
          const Real sinTheta = sin(theta);
          const Real cscTheta = 1.0 / sinTheta;
          
          Real data[3][3] = {{sinTheta*cscTheta, cosPhi*cosTheta*cscTheta, -sinPhi*cosTheta*cscTheta},
                              {0.0, sinPhi*sinTheta*cscTheta, cosPhi*sinTheta*cscTheta},
                              {0.0, -cosPhi*cscTheta, sinPhi*cscTheta}};
                              
          return Matrix33(data);
      }
      Matrix33 eulerAngleRatesMatrix_YXY(Real phi, Real theta, Real psi)
      {
          const Real cosPhi   = cos(phi);
          const Real sinPhi   = sin(phi);
          const Real cosTheta = cos(theta);
          const Real sinTheta = sin(theta);
          const Real cscTheta = 1.0 / sinTheta;
          
          Real data[3][3] = {{-sinPhi*cosTheta*cscTheta, sinTheta*cscTheta, cosPhi*cosTheta*cscTheta},
                              {sinTheta*cosPhi*cscTheta, 0.0, sinTheta*sinPhi*cscTheta},
                              {sinPhi*cscTheta, 0.0, -cosPhi*cscTheta}};
                              
          return Matrix33(data);
      }
      Matrix33 eulerAngleRatesMatrix_YZX(Real phi, Real theta, Real psi)
      {
          const Real cosPhi   = cos(phi);
          const Real sinPhi   = sin(phi);
          const Real cosTheta = cos(theta);
          const Real sinTheta = sin(theta);
          const Real secTheta = 1.0 / cosTheta;
          
          Real data[3][3] = {{cosPhi*sinTheta*secTheta, cosTheta*secTheta, sinPhi*sinTheta*secTheta},
                              {-sinPhi*cosTheta*secTheta, 0.0, cosPhi*cosTheta*secTheta},
                              {cosPhi*secTheta, 0.0, sinPhi*secTheta}};
                              
          return Matrix33(data);
      }
      Matrix33 eulerAngleRatesMatrix_ZXY(Real phi, Real theta, Real psi)
      {
          const Real cosPhi   = cos(phi);
          const Real sinPhi   = sin(phi);
          const Real cosTheta = cos(theta);
          const Real sinTheta = sin(theta);
          const Real secTheta = 1.0 / cosTheta;
          
          Real data[3][3] = {{sinPhi*sinTheta*secTheta, cosPhi*sinTheta*secTheta, cosTheta*secTheta},
                              {cosTheta*cosPhi*secTheta, -sinPhi*cosTheta*secTheta, 0.0},
                              {sinPhi*secTheta, cosPhi*secTheta, 0.0}};
                              
          return Matrix33(data);
      }
      Matrix33 eulerAngleRatesMatrix_XZY(Real phi, Real theta, Real psi)
      {
          const Real cosPhi   = cos(phi);
          const Real sinPhi   = sin(phi);
          const Real cosTheta = cos(theta);
          const Real sinTheta = sin(theta);
          const Real secTheta = 1.0 / cosTheta;
          
          Real data[3][3] = {{cosTheta*secTheta, -cosPhi*sinTheta*secTheta, sinPhi*sinTheta*secTheta},
                              {0.0, sinPhi*cosTheta*secTheta, cosPhi*cosTheta*secTheta},
                              {0.0, cosPhi*secTheta, -sinPhi*secTheta}};
                              
          return Matrix33(data);
      }
      Matrix33 eulerAngleRatesMatrix_ZYX(Real phi, Real theta, Real psi)
      {
          const Real cosPhi   = cos(phi);
          const Real sinPhi   = sin(phi);
          const Real cosTheta = cos(theta);
          const Real sinTheta = sin(theta);
          const Real secTheta = 1.0 / cosTheta;
          
          Real data[3][3] = {{-cosPhi*sinTheta*secTheta, sinPhi*sinTheta*secTheta, cosTheta*secTheta},
                              {sinPhi*cosTheta*secTheta, cosPhi*cosTheta*secTheta, 0.0},
                              {cosPhi*secTheta, -sinPhi*secTheta, 0.0}};
                              
          return Matrix33(data);
      }
      Matrix33 eulerAngleRatesMatrix_YXZ(Real phi, Real theta, Real psi)
      {
          const Real cosPhi   = cos(phi);
          const Real sinPhi   = sin(phi);
          const Real cosTheta = cos(theta);
          const Real sinTheta = sin(theta);
          const Real secTheta = 1.0 / cosTheta;
          
          Real data[3][3] = {{sinPhi*sinTheta*secTheta, cosTheta*secTheta, -cosPhi*sinTheta*secTheta},
                              {cosPhi*cosTheta*secTheta, 0.0, sinPhi*cosTheta*secTheta},
                              {-sinPhi*secTheta, 0.0, cosPhi*secTheta}};
                              
          return Matrix33(data);
      }

      EulerAngles linearInterpolate(const EulerAngles& startAngles, const EulerAngles& endAngles, Real t)
      {
          if (startAngles.getEulerSequence() == endAngles.getEulerSequence())
          {
              if (t < 0.0){return startAngles;}
              if (t > 1.0){return endAngles;}
              Real phiNew = (1-t) * startAngles.phi + t * endAngles.phi;
              Real thetaNew = (1-t) * startAngles.theta + t * endAngles.theta;
              Real psiNew = (1-t) * startAngles.psi + t * endAngles.psi;
              return EulerAngles(phiNew, thetaNew, psiNew, startAngles.getEulerSequence());
          }
          return EulerAngles();
      }

      EulerAngles smoothInterpolate(const EulerAngles& startAngles, const EulerAngles& endAngles, Real t)
      {
          if (startAngles.getEulerSequence() == endAngles.getEulerSequence())
          {
              if (t < 0.0){return startAngles;}
              if (t > 1.0){return endAngles;}

              Real t2 = t*t;
              Real t3 = t2*t;
              Real t4 = t3*t;
              Real t5 = t4*t;

              Real deltaPhi = endAngles.phi - startAngles.phi;
              Real deltaTheta = endAngles.theta - startAngles.theta;
              Real deltaPsi = endAngles.psi - startAngles.psi;

              Real phiNew   = 6*deltaPhi*t5 + -15*deltaPhi*t4 + 10*deltaPhi*t3 + startAngles.phi;
              Real thetaNew = 6*deltaTheta*t5 + -15*deltaTheta*t4 + 10*deltaTheta*t3 + startAngles.theta;
              Real psiNew   = 6*deltaPsi*t5 + -15*deltaPsi*t4 + 10*deltaPsi*t3 + startAngles.psi;

              return EulerAngles(phiNew, thetaNew, psiNew, startAngles.getEulerSequence());
          }
          return EulerAngles();
      }

  };

}