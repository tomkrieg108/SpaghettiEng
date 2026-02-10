#pragma once

#include "Math/MathX.h"

namespace Phys
{
  namespace mx = MathX;
  class Particle;
  class ParticleContactResolver;

  /**
    * @brief 
    * A Contact represents two objects in contact (in this case
    * ParticleContact representing two Particles). Resolving a
    * contact removes their interpenetration, and applies sufficient
    * impulse to keep them apart. Colliding bodies may also rebound.
    *
    * The contact has no callable functions, it just holds the
    * contact details. To resolve a set of contacts, use the particle
    * contact resolver class.
   */
   
  struct ParticleContact
  {
    friend class ParticleContactResolver;

    // Particles involved in the contact
    Particle* particle[2];

    // Normal restitution coefficient at the contact.
    mx::Real restitution; 

    // The direction of the contact in world coordinates from the first particles perspective
    mx::Vec3 contact_normal; 

    // The depth of penetration at the contact.
    mx::Real penetration; 

    // the amount each particle is moved by during interpenetration resolution
    mx::Vec3 particle_movement[2]; 

    protected:
      //Resolves this contact, for both velocity and interpenetration.
      void Resolve(mx::Real time_step);

      //Calculates the separating velocity at this contact.
      mx::Real CalculateSeparatingVelocity() const;


    private:
      // Handles the impulse calculations for this collision.
      void ResolveVelocity(mx::Real time_step);

      // Handles the interpenetration resolution for this contact.
      void ResolveInterpenetration(mx::Real time_step);
  };

  /**
    * @brief 
    * The contact resolution routine for particle contacts. One
    * resolver instance can be shared for the whole simulation.
   */
  class ParticleContactResolver
  {
    public:
      ParticleContactResolver(uint32_t iterations) : m_iterations{iterations} {}
      void SetIterations(uint32_t iterations) {
        m_iterations = iterations;
      }
      void ResolveContacts(ParticleContact *contact_array, uint32_t num_contacts,
            mx::Real time_step);

    protected:
      uint32_t m_iterations; // Iterations allowed.
      uint32_t m_iterations_used; //Iterations used
  };

  /**
    * @brief 
    * This is the basic polymorphic interface for contact generators
    * applying to particles.
   */
  class ParticleContactGenerator
  {
    public:
      virtual unsigned AddContact(ParticleContact *contact,
                                    uint32_t limit) const = 0;
  };
}