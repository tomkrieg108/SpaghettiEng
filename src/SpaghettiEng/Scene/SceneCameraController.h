#pragma once

#include "SpaghettiEng/Scene/Components.h"

namespace Spg
{
  class TransformController
  {

    public:

      TransformController(Transform& transform) : m_transform(transform) {}
      ~TransformController() = default;

      void MoveForward(float amount) {
        glm::vec3 direction = m_transform.LocalForward();
        m_transform.Translate(direction*amount);    
      }

      void MoveRight(float amount) {
          m_transform.Translate(glm::vec3(1,0,0)*amount);  
    
      }

      void MoveVertically(float amount) {
        m_transform.Translate(glm::vec3(0,1,0)*amount);  
      }

      void SetEnabled(bool enabled) {m_enabled = enabled;}
      bool IsEnabled() const {return m_enabled;}

    private:
      bool m_enabled = true;
      Transform& m_transform;

  };
}
