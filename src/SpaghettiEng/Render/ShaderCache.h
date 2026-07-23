#pragma once

#include "SpaghettiEng/Render/Backends/OpenGL/GLShader.h"

/*
  {} []
*/

// Initialised on startup 

  //Todo - Similar thing for other resource types - make a templated class, 
  //todo - Can also look into entt::resource_cache, entt::resource
namespace Spg
{
  class ShaderCache
  {
     public:

      static void Init();

      static GLShader& Get(ShaderId mesh_id);
      static GLShader& Get(const std::string& shader_name);

      static ShaderId GetId(const std::string& shader_name);
      static ShaderId NextId();

      //static MeshComponent GetShaderComponent(const std::string& shader_name);

    private:

      static void AddShader(const std::string& shader_name);

    private:  
      //static ShaderId s_next_id; // using m_program_id
      static std::unordered_map<ShaderId ,GLShader> s_shader_map;
      static std::unordered_map<std::string, ShaderId> s_name_to_id_map;

  };


}