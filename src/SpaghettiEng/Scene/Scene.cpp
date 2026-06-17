
#include "SpaghettiEng/Scene/Scene.h"
#include "SpaghettiEng/Scene/Registry.h"

namespace Spg
{
   Scene::Scene() : m_registry(std::make_unique<Registry>())
   {
   }

  Scene::~Scene() = default;
  

}