#pragma once

// {}
namespace Spg
{
  class Registry;

  class SceneVisuals
  {
    public:
      SceneVisuals();
      ~SceneVisuals();

    private:
      Registry* m_registry = nullptr;
  };
}