#include "SpaghettiEng/Render/ShaderCache.h"

#include <unordered_map>
#include <string>

#include "CoreLib/Core.h"
#include "SpaghettiEng/Render/Backends/OpenGL/GLShader.h"

/*
{} []
*/
namespace Spg
{
 // ShaderId ShaderCache::s_next_id = 0; // using m_program_id
  std::unordered_map<ShaderId, GLShader> ShaderCache::s_shader_map{};
  std::unordered_map<std::string, ShaderId> ShaderCache::s_name_to_id_map{};

  void ShaderCache::Init()
  {
    //SPG_INFO("Adding basic shader....")
    AddShader("BasicShader");

    auto& shader = ShaderCache::Get("BasicShader");
    shader.PrintInfo();
  }

  ShaderId ShaderCache::GetId(const std::string& shader_name)
  {
    auto it = s_name_to_id_map.find(shader_name);
    SPG_ASSERT( it != s_name_to_id_map.end());
    return it->second;
  }

  GLShader& ShaderCache::Get(ShaderId shader_id)
  {
    auto it = s_shader_map.find(shader_id);
    SPG_ASSERT( it != s_shader_map.end());
    return it->second;
  }

  GLShader& ShaderCache::Get(const std::string& shader_name)
  {
    auto id = GetId(shader_name);
    return Get(id);
  }

  //Using m_program_id
  // ShaderId ShaderCache::NextId() 
  // {
  // }

  void ShaderCache::AddShader(const std::string& shader_name)
  {
    GLShaderBuilder shader_builder;
    auto shader = shader_builder.Add(ShaderType::Vertex, "basic.vs").Add(ShaderType::Fragment, "basic.fs").Build(shader_name);

    //use m_program_id OpenGL as the ShaderID
    auto id = shader.GetProgramID();
    s_shader_map.insert({id, std::move(shader)});
    s_name_to_id_map[shader_name] = id;
  }
}