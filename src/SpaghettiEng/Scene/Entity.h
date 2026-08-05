#pragma once

#include <entt/fwd.hpp> //light weight forward declarations

namespace Spg
{
  struct Entity
  {
    Entity(); // Defer definition to source file
    Entity(entt::entity handle); // Defer definition to source file

    explicit operator bool() const;
    operator entt::entity() const;
    operator std::uint32_t() const;

    entt::entity handle;
  };
}
