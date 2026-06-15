#pragma once

#include "PhysLib/Cyclone/Particle.h"
#include "MathLib/MathLib.h"

namespace Cyc
{
  class ParticleForceGenerator
  {
    public:
      virtual void UpdateForce(Particle *particle, SpgMth::Real time_step) = 0;
  };

  class ParticleGravity : public ParticleForceGenerator
  {
    public:
      ParticleGravity(SpgMth::Vec3 &gravity) : m_gravity{gravity} {}
      virtual void UpdateForce(Particle *particle, SpgMth::Real time_step) override;

    private:
      SpgMth::Vec3 m_gravity;
  };

  class ParticleDrag : public ParticleForceGenerator
  {
    public:
      ParticleDrag(SpgMth::Real k1, SpgMth::Real k2) : m_k1{k1},  m_k2{k2} {}
      virtual void UpdateForce(Particle *particle, SpgMth::Real time_step) override;

    private:
      SpgMth::Real m_k1;
      SpgMth::Real m_k2;
  };

  class ParticleSpring : public ParticleForceGenerator
  {
    public:
      ParticleSpring(Particle *other,SpgMth::Real spring_const, SpgMth::Real rest_length) :
      m_other{other}, m_spring_const{spring_const}, m_rest_length{rest_length} 
      {}

      virtual void UpdateForce(Particle *particle, SpgMth::Real time_step) override;

    private:
      Particle *m_other; // The particle at the other end of the spring.
      SpgMth::Real m_spring_const; // Holds the sprint constant.
      SpgMth::Real m_rest_length; // Holds the rest length of the spring.
  };

  class ParticleAnchoredSpring : public ParticleForceGenerator
  {
    public:

      ParticleAnchoredSpring(SpgMth::Vec3 *anchor, SpgMth::Real spring_const, 
        SpgMth::Real rest_length) :
      m_anchor{anchor}, m_spring_const{spring_const}, m_rest_length{rest_length} 
      {}
     
      const SpgMth::Vec3* GetAnchor() const { 
        return m_anchor; 
      }

      void Init(SpgMth::Vec3 *anchor, SpgMth::Real spring_const, SpgMth::Real rest_length) {
        m_anchor = anchor;
        m_spring_const = spring_const;
        m_rest_length = rest_length;
      }

      virtual void UpdateForce(Particle *particle, SpgMth::Real time_step) override;   

    protected:
      SpgMth::Vec3 *m_anchor;      // The location of the anchored end of the spring..
      SpgMth::Real m_spring_const; // Holds the sprint constant.
      SpgMth::Real m_rest_length;  // Holds the rest length of the spring.
  };

  class ParticleFakeSpring : public ParticleForceGenerator
  {
    public:
      ParticleFakeSpring(SpgMth::Vec3 *anchor, SpgMth::Real spring_const, SpgMth::Real damping) :
      m_anchor{anchor}, m_spring_const{spring_const}, m_damping{damping} 
      {}

      virtual void UpdateForce(Particle *particle, SpgMth::Real time_step) override;   

    private:
      SpgMth::Vec3 *m_anchor;      // The location of the anchored end of the spring..
      SpgMth::Real m_spring_const; // Holds the sprint constant.
      SpgMth::Real m_damping;      // Holds the damping on the oscillation of the spring.
  };

  class ParticleBungee : public ParticleForceGenerator
  {
    public:
      ParticleBungee(Particle *other,SpgMth::Real spring_const, SpgMth::Real rest_length) : m_other{other}, m_spring_const{spring_const}, m_rest_length{rest_length} 
      {}

      virtual void UpdateForce(Particle *particle, SpgMth::Real time_step) override;

    private:
      Particle *m_other;          // The particle at the other end of the spring.
      SpgMth::Real m_spring_const; // Holds the sprint constant.
      SpgMth::Real m_rest_length;  // Holds the rest length of the spring.
  };

  class ParticleAnchoredBungee : public ParticleAnchoredSpring
  {
    public:
       virtual void UpdateForce(Particle *particle, SpgMth::Real time_step) override; 
  };

  class ParticleBuoyancy : public ParticleForceGenerator
  {
    public:
      ParticleBuoyancy(SpgMth::Real max_depth, SpgMth::Real volume, 
        SpgMth::Real water_height, SpgMth::Real liquid_density) :
        m_max_depth{max_depth}, m_volume{volume}, m_water_height{water_height},m_liquid_density{liquid_density}
        {}

      virtual void UpdateForce(Particle *particle, SpgMth::Real time_step) override;

    private:
      SpgMth::Real m_max_depth;  
      SpgMth::Real m_volume;  
      SpgMth::Real m_water_height;  
      SpgMth::Real m_liquid_density;  
  };


  class ParticleForceRegistry
  {
    public:
      void Add(Particle* particle, ParticleForceGenerator *fg);
      void Remove(Particle* particle, ParticleForceGenerator *fg);
      void Clear();
      void UpdateForces(SpgMth::Real time_step);

    protected:

      struct ParticleForceRegistration
      {
        Particle *particle;
        ParticleForceGenerator *fg;
      };

      std::vector<ParticleForceRegistration> m_registrations;

  };
  
  



}