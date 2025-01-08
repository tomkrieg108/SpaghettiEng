#pragma once
#include <filesystem>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <Common/Common.h>
#include "GL32ShaderUtils.h"

namespace Spg
{

  namespace fs = std::filesystem;

  class GLShader;

  struct GLShaderInfo
  {
    std::string file_name{ "" };
    //fs::path filepath{""};
    ShaderType type = ShaderType::Unknown;
    uint32_t id = 0;
    bool compile_success = false;
  };  

  class GLShaderBuilder
  {
  public: 
    GLShaderBuilder();
    ~GLShaderBuilder() = default;
    GLShaderBuilder& Add(ShaderType type, const std::string& file_name);
    //GLShaderBuilder& Add(const std::string& filename);
    Scope<GLShader> Build(const std::string& shader_name);

  private:
    void PrintBuildLog();
    std::string ReadSource(const std::string& file_name);
    GLShaderInfo Compile(ShaderType type, const std::string& file_name, const std::string& source_code);
    
  private:
    fs::path m_shader_path{""};
    std::vector<GLShaderInfo> m_shader_list;
  };

  
  class GLShader
  {
    friend class GLShaderBuilder;

  public:

    //Same for Attribute or uniform
    struct DataItem
    {
      std::string name = "";
      int32_t location = -1;
      ShaderDataType type = ShaderDataType::Float;
      int32_t size = 0; //array => greater than 1
    };
    using Uniform = DataItem;
    using Attribute = DataItem;

    struct UniformBlock
    {
      std::string name = "";
      int32_t index = -1;
      int32_t binding = -1;
      int32_t size = 0;
      int32_t active_uniforms = 0;
    };

  public:
    GLShader() = default;
    ~GLShader() = default;

    void Bind() const;
    void Unbind() const;  

    void SetUniform1f(const std::string& name, float v0);
    void SetUniform2f(const std::string& name, float v0, float v1);
    void SetUniform3f(const std::string& name, float v0, float v1, float v2);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniform2f(const std::string& name, const glm::vec2& data);
    void SetUniform3f(const std::string& name, const glm::vec3& data);
    void SetUniform4f(const std::string& name, const glm::vec4& data);
    void SetUniform1i(const std::string& name, int v0);
    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

    uint32_t GetProgramID() const { return m_program_id; }
    std::string GetName() const { return m_name; }
    bool BuildSuccess() const { return m_build_success; }
    void PrintInfo();

  private:
    int32_t GetUniformLocation(const std::string& name);
    void ReadUniforms();
    void ReadUniformBlocks();
    void ReadAttributes();  

  private:
    std::string m_name{"Unnamed Shader"};
    bool m_build_success = false;
    uint32_t m_program_id = 0;

    std::unordered_map<std::string, DataItem> m_uniforms;
    std::unordered_map<std::string, UniformBlock> m_uniform_blocks;
    std::unordered_map<std::string, DataItem> m_attributes;
  };

}