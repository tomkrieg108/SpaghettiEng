#include "SpaghettiEng/Scene/Entity.h"

namespace Spg
{
  Entity::Entity() : handle(entt::null) {}
  Entity::Entity(entt::entity handle) : handle(handle) {} 
  
  Entity::operator entt::entity() const{ return handle; }
  Entity::operator std::uint32_t() const { return entt::to_integral(handle); }
  Entity::operator bool() const { return handle != entt::null; }
}