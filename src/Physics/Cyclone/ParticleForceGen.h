#pragma once

#include "Physics/Cyclone/Particle.h"
#include "Math/MathX.h"

namespace Cyc
{
  class ParticleForceGenerator
  {
    public:
      virtual void UpdateForce(Particle *particle, MathX::Real time_step) = 0;
  };

  class ParticleGravity : public ParticleForceGenerator
  {
    public:
      ParticleGravity(MathX::Vec3 &gravity) : m_gravity{gravity} {}
      virtual void UpdateForce(Particle *particle, MathX::Real time_step) override;

    private:
      MathX::Vec3 m_gravity;
  };

  class ParticleDrag : public ParticleForceGenerator
  {
    public:
      ParticleDrag(MathX::Real k1, MathX::Real k2) : m_k1{k1},  m_k2{k2} {}
      virtual void UpdateForce(Particle *particle, MathX::Real time_step) override;

    private:
      MathX::Real m_k1;
      MathX::Real m_k2;
  };

  class ParticleSpring : public ParticleForceGenerator
  {
    public:
      ParticleSpring(Particle *other,MathX::Real spring_const, MathX::Real rest_length) :
      m_other{other}, m_spring_const{spring_const}, m_rest_length{rest_length} 
      {}

      virtual void UpdateForce(Particle *particle, MathX::Real time_step) override;

    private:
      Particle *m_other; // The particle at the other end of the spring.
      MathX::Real m_spring_const; // Holds the sprint constant.
      MathX::Real m_rest_length; // Holds the rest length of the spring.
  };

  class ParticleAnchoredSpring : public ParticleForceGenerator
  {
    public:

      ParticleAnchoredSpring(MathX::Vec3 *anchor, MathX::Real spring_const, 
        MathX::Real rest_length) :
      m_anchor{anchor}, m_spring_const{spring_const}, m_rest_length{rest_length} 
      {}
     
      const MathX::Vec3* GetAnchor() const { 
        return m_anchor; 
      }

      void Init(MathX::Vec3 *anchor, MathX::Real spring_const, MathX::Real rest_length) {
        m_anchor = anchor;
        m_spring_const = spring_const;
        m_rest_length = rest_length;
      }

      virtual void UpdateForce(Particle *particle, MathX::Real time_step) override;   

    protected:
      MathX::Vec3 *m_anchor;      // The location of the anchored end of the spring..
      MathX::Real m_spring_const; // Holds the sprint constant.
      MathX::Real m_rest_length;  // Holds the rest length of the spring.
  };

  class ParticleFakeSpring : public ParticleForceGenerator
  {
    public:
      ParticleFakeSpring(MathX::Vec3 *anchor, MathX::Real spring_const, MathX::Real damping) :
      m_anchor{anchor}, m_spring_const{spring_const}, m_damping{damping} 
      {}

      virtual void UpdateForce(Particle *particle, MathX::Real time_step) override;   

    private:
      MathX::Vec3 *m_anchor;      // The location of the anchored end of the spring..
      MathX::Real m_spring_const; // Holds the sprint constant.
      MathX::Real m_damping;      // Holds the damping on the oscillation of the spring.
  };

  class ParticleBungee : public ParticleForceGenerator
  {
    public:
      ParticleBungee(Particle *other,MathX::Real spring_const, MathX::Real rest_length) : m_other{other}, m_spring_const{spring_const}, m_rest_length{rest_length} 
      {}

      virtual void UpdateForce(Particle *particle, MathX::Real time_step) override;

    private:
      Particle *m_other;          // The particle at the other end of the spring.
      MathX::Real m_spring_const; // Holds the sprint constant.
      MathX::Real m_rest_length;  // Holds the rest length of the spring.
  };

  class ParticleAnchoredBungee : public ParticleAnchoredSpring
  {
    public:
       virtual void UpdateForce(Particle *particle, MathX::Real time_step) override; 
  };

  class ParticleBuoyancy : public ParticleForceGenerator
  {
    public:
      ParticleBuoyancy(MathX::Real max_depth, MathX::Real volume, 
        MathX::Real water_height, MathX::Real liquid_density) :
        m_max_depth{max_depth}, m_volume{volume}, m_water_height{water_height},m_liquid_density{liquid_density}
        {}

      virtual void UpdateForce(Particle *particle, MathX::Real time_step) override;

    private:
      MathX::Real m_max_depth;  
      MathX::Real m_volume;  
      MathX::Real m_water_height;  
      MathX::Real m_liquid_density;  
  };


  class ParticleForceRegistry
  {
    public:
      void Add(Particle* particle, ParticleForceGenerator *fg);
      void Remove(Particle* particle, ParticleForceGenerator *fg);
      void Clear();
      void UpdateForces(MathX::Real time_step);

    protected:

      struct ParticleForceRegistration
      {
        Particle *particle;
        ParticleForceGenerator *fg;
      };

      std::vector<ParticleForceRegistration> m_registrations;

  };
  
  



}