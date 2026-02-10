#include "Physics/ParticleContacts.h"
#include "Physics/Particle.h"

namespace Phys
{
  void ParticleContact::Resolve(mx::Real time_step) {
    ResolveVelocity(time_step);
    ResolveInterpenetration(time_step);
  }

  mx::Real ParticleContact::CalculateSeparatingVelocity() const {
    mx::Vec3 relative_velocity = particle[0]->GetVelocity();
    if(particle[1] != nullptr)
      relative_velocity -= particle[1]->GetVelocity();
    return glm::dot(relative_velocity,contact_normal);  
  }

  void ParticleContact::ResolveVelocity(mx::Real time_step) {
    // Find the velocity in the direction of the contact
    mx::Real separating_velocity = CalculateSeparatingVelocity();

    // Check if it needs to be resolved
    if(separating_velocity > 0)
      return; //contect is separating or stationaty - no impulse needed

     // Calculate the new separating velocity
     mx::Real new_sep_velocity = -separating_velocity * restitution;

    // Check the velocity build-up due to acceleration only
    mx::Vec3 acc_caused_velocity = particle[0]->GetAcceleration();
    if (particle[1] != nullptr) 
      acc_caused_velocity -= particle[1]->GetAcceleration();
    mx::Real acc_caused_sep_velocity = glm::dot(acc_caused_velocity,contact_normal) * time_step;

    // If we've got a closing velocity due to acceleration build-up,
    // remove it from the new separating velocity
    if (acc_caused_sep_velocity < 0){
        new_sep_velocity += restitution * acc_caused_sep_velocity;
        // Make sure we haven't removed more than was there to remove.
        if (new_sep_velocity < 0) 
          new_sep_velocity = 0;
    }
    
    mx::Real delta_velocity = new_sep_velocity - separating_velocity;
    
    // We apply the change in velocity to each object in proportion to
    // their inverse mass (i.e. those with lower inverse mass [higher
    // actual mass] get less change in velocity)..
    mx::Real total_inverse_mass = particle[0]->GetInverseMass();

    // If all particles have infinite mass, then impulses have no effect
    if(total_inverse_mass < 0)
      return;

     // Calculate the impulse to apply  
    mx::Real impulse = delta_velocity / total_inverse_mass;
    
    // The amount of impulse per unit of inverse mass
    mx::Vec3 impulse_per_imass = contact_normal*impulse;

    // Apply impulses: they are applied in the direction of the contact,
    // and are proportional to the inverse mass.
    particle[0]->SetVelocity(particle[0]->GetVelocity() + 
      impulse_per_imass * -particle[0]->GetInverseMass());

    if(particle[1] != nullptr) {
      // Particle 1 goes in the opposite direction
      particle[1]->SetVelocity(particle[1]->GetVelocity() + 
      impulse_per_imass * -particle[1]->GetInverseMass());
    }
  }

  void ParticleContact::ResolveInterpenetration(mx::Real time_step) {
    // If we don't have any penetration, skip this step.
    if (penetration <= 0) 
      return;

    // The movement of each object is based on their inverse mass, so
    // total that.
    mx::Real total_inverse_mass = particle[0]->GetInverseMass();
    if (particle[1] != nullptr) 
      total_inverse_mass += particle[1]->GetInverseMass();  

    // If all particles have infinite mass, then we do nothing
    if (total_inverse_mass <= 0) 
      return;

     // Find the amount of penetration resolution per unit of inverse mass
    mx::Vec3 move_per_imass = contact_normal * (penetration / total_inverse_mass);

    // Calculate the the movement amounts
    particle_movement[0] = move_per_imass * particle[0]->GetInverseMass();
    if (particle[1]) 
        particle_movement[1] = move_per_imass * -particle[1]->GetInverseMass();
    else 
        particle_movement[1] = mx::Vec3(0);
    
  }

  void ParticleContactResolver::ResolveContacts(ParticleContact *contact_array, uint32_t num_contacts,
            mx::Real time_step) {

    m_iterations_used = 0;
    while(m_iterations_used < m_iterations) {
      // Find the contact with the largest closing velocity;
      mx::Real max = mx::REAL_MAX;
      uint32_t max_index = num_contacts;
      for(auto i = 0; i< num_contacts; i++) {
        mx::Real sep_vel = contact_array[i].CalculateSeparatingVelocity();
        if((sep_vel < max) && (sep_vel < 0 || contact_array[i].penetration > 0)) {
          max = sep_vel;
          max_index = i;
        }
      }

      // Do we have anything worth resolving?
      if (max_index == num_contacts) 
        break;

      contact_array[max_index].Resolve(time_step);
      
      // Update the interpenetrations for all particles
      mx::Vec3* move = contact_array[max_index].particle_movement;

      for (auto i = 0; i < num_contacts; i++) {
        if (contact_array[i].particle[0] == contact_array[max_index].particle[0])
          contact_array[i].penetration -= glm::dot(move[0], contact_array[i].contact_normal);
        
        else if (contact_array[i].particle[0] == contact_array[max_index].particle[1])
          contact_array[i].penetration -= glm::dot(move[1], contact_array[i].contact_normal);
        
        if (contact_array[i].particle[1]) {
          if (contact_array[i].particle[1] == contact_array[max_index].particle[0])
            contact_array[i].penetration += glm::dot(move[0], contact_array[i].contact_normal);
          
          else if (contact_array[i].particle[1] == contact_array[max_index].particle[1])
            contact_array[i].penetration += glm::dot(move[1], contact_array[i].contact_normal);
        }
      }

      m_iterations_used++;
    }         
  }

}