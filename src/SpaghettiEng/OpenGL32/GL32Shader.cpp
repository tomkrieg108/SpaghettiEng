
#include <glad/gl.h>
#include <Common/Common.h>
#include "Core/Application.h"
#include "GL32ShaderUtils.h"
#include "GL32Shader.h"

using namespace std::string_literals;

namespace Spg
{
  /* **************************************************************************************
    BUILDER
  * **************************************************************************************/

  GLShaderBuilder::GLShaderBuilder() 
  {
    m_shader_path = fs::current_path() / fs::path{"Shaders"};
    m_shader_path = fs::absolute(m_shader_path);
    SPG_INFO("Absolute shaders path: {}", m_shader_path.string());

    if (!fs::exists(m_shader_path)) {
        SPG_ERROR("Shaders path does not exist: {}", m_shader_path.string());
        return;
    }

    if(!fs::is_directory(m_shader_path))
      SPG_ERROR("Shaders path is not a directory: {}", m_shader_path.string());
  }

  std::string GLShaderBuilder::ReadSource(const std::string& file_name)
  {
    fs::path file_path =  fs::absolute( m_shader_path / fs::path{file_name} );
    std::ifstream stream(file_path.c_str());
    if (stream.fail())
    {
        SPG_ERROR("Could not open shader file: {}", file_path.string());
        return ""s;
    }
    std::string line;
    std::ostringstream ss;
    while (getline(stream, line))
        ss << line << "\n";
      
    return ss.str();
  }

  GLShaderInfo GLShaderBuilder::Compile(ShaderType type, const std::string& file_name, const std::string& source_code)
  {
    GLShaderInfo info;
    auto gl_shader_type = ShaderUtils::GetGLShaderType(type);
    info.id = glCreateShader(gl_shader_type);
    info.file_name = file_name;
    info.type = type;
    const char* src = source_code.c_str();
    glShaderSource(info.id, 1, &src, nullptr);
    glCompileShader(info.id);
    int32_t result;
    glGetShaderiv(info.id, GL_COMPILE_STATUS, &result);
    info.compile_success = (result == GL_TRUE ? true : false);
    return info;
  }

  GLShaderBuilder& GLShaderBuilder::Add(ShaderType type, const std::string& file_name)
  {
    auto source_code = ReadSource(file_name);
    auto info = Compile(type, file_name, source_code);
    m_shader_list.push_back(info);
    return *this;
  }

  Scope<GLShader> GLShaderBuilder::Build(const std::string& name)
  {
    auto shader = CreateScope<GLShader>();
    uint32_t program = glCreateProgram();
    shader->m_name = name;
    shader->m_program_id = program;
    bool success = true;

    bool compile_errors = false;
    for (auto& shader_info : m_shader_list)
    {
      if (shader_info.compile_success)
        glAttachShader(program, shader_info.id);
      else
        compile_errors = true;
    }
    if (compile_errors)
    {
      SPG_ERROR("Compilation errors in 1 or more shaders: {}", name);
      PrintBuildLog();
      success = false;
    }
    glLinkProgram(program);
    int32_t result = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
      SPG_ERROR("Linking error for shader: {}", name);
      success = false;
    }

  #ifdef SPG_DEBUG
		result = GL_FALSE;
		glValidateProgram(program);
		glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
		if (result == GL_FALSE)
		{
			SPG_ERROR("Validation error for shader: {}", name);
			success = false;
		}
		PrintBuildLog();
  #endif
    shader->m_build_success = success;

    for (auto& shader_info : m_shader_list)
      glDeleteShader(shader_info.id);

    m_shader_list.clear();

    shader->ReadAttributes();
    shader->ReadUniforms();
    shader->ReadUniformBlocks();
    return shader;
  }

  void GLShaderBuilder::PrintBuildLog()
  {
    for (auto& shader_info : m_shader_list)
    {
      int length = 0;
      glGetShaderiv(shader_info.id, GL_INFO_LOG_LENGTH, &length);
      if(length == 0)
        continue;
      SPG_TRACE("   Shader info log: {}", shader_info.file_name);
      char* message = new char[length];
      glGetShaderInfoLog(shader_info.id, length, &length, message);
      SPG_TRACE("   filepath: {}, Compiled status: {}, id: {} message: {}", shader_info.file_name, shader_info.compile_success, shader_info.id, message);
      delete[] message;
    } 
  }

/* **************************************************************************************
SHADER
* **************************************************************************************/

  void GLShader::Bind() const
  {
    glUseProgram(m_program_id);
  }

  void GLShader::Unbind() const
  {
    glUseProgram(0);
  }

  void GLShader::SetUniform1f(const std::string& name, float v0)
  {
    glUniform1f(GetUniformLocation(name), v0);
  }

  void GLShader::SetUniform2f(const std::string& name, float v0, float v1)
  {
    glUniform2f(GetUniformLocation(name), v0, v1);
  }

  void GLShader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
  {
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
  }

  void GLShader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
  {
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
  }

  void GLShader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
  {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
  }

  void GLShader::SetUniform2f(const std::string& name, const glm::vec2& data)
  {
    glUniform2f(GetUniformLocation(name), data[0], data[1]);
  }

  void GLShader::SetUniform3f(const std::string& name, const glm::vec3& data)
  {
    glUniform3f(GetUniformLocation(name), data[0], data[1], data[2]);
  }

  void GLShader::SetUniform4f(const std::string& name, const glm::vec4& data)
  {
    glUniform4f(GetUniformLocation(name), data[0], data[1], data[2], data[3]);
  }

  void GLShader::SetUniform1i(const std::string& name, int v0)
  {
    glUniform1i(GetUniformLocation(name), v0);
  }

  int32_t GLShader::GetUniformLocation(const std::string& name)
  {
    if (m_uniforms.find(name) != m_uniforms.end())
		  return m_uniforms[name].location;
    SPG_WARN("Uniform name: {} Doesn't exist.  Program ID: {} ", name, m_program_id);
    return -1;
  }

  void GLShader::ReadUniforms()
  {
    int32_t params = -1;
    const int kMaxLength = 100;
    glGetProgramiv(m_program_id, GL_ACTIVE_UNIFORMS, &params);
    for (int32_t i = 0; i < params; i++)
    {
      GLShader::Uniform uniform;
      char name[kMaxLength] = { 0 };
      int actual_length = 0;
      int size = 0;
      GLenum type;
      glGetActiveUniform(m_program_id, i, kMaxLength, &actual_length, &size, &type, name);
      uniform.type = ShaderUtils::GLTypeToShaderDataType(type);
      uniform.size = size;
      for (int32_t element = 0; element < size; element++)
      {
        if (size == 1)
        {
          uniform.location = glGetUniformLocation(m_program_id, name);
          uniform.name = std::string(name);
        }
        else
        {
          std::string item_name = std::string{ name };
          item_name = item_name.substr(0, item_name.size() - 3) + std::string("[") + std::to_string(element) + "]";
          uniform.location = glGetUniformLocation(m_program_id, item_name.c_str());
          uniform.name = item_name;
        }
        m_uniforms[uniform.name] = uniform;
      }
    }      
  }

  void GLShader::ReadUniformBlocks()
  {
		int32_t params = -1;
		const int kMaxLength = 100;
		glGetProgramiv(m_program_id, GL_ACTIVE_UNIFORM_BLOCKS, &params);
		for (int32_t i = 0; i < params; i++)
		{
			GLShader::UniformBlock uniform_block;
			char name[kMaxLength] = { 0 };
			int actual_length = 0;
			glGetActiveUniformBlockName(m_program_id, i, kMaxLength, &actual_length, name);
			uniform_block.name = std::string{ name };
			uniform_block.index = glGetUniformBlockIndex(m_program_id, name); //returns GL_INVALID_INDEX if can't find
			glGetActiveUniformBlockiv(m_program_id, i, GL_UNIFORM_BLOCK_BINDING, &(uniform_block.binding));
			glGetActiveUniformBlockiv(m_program_id, i, GL_UNIFORM_BLOCK_DATA_SIZE, &(uniform_block.size));
			glGetActiveUniformBlockiv(m_program_id, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &(uniform_block.active_uniforms));

			m_uniform_blocks[uniform_block.name] = uniform_block;
		}    
  }
  
  void GLShader::ReadAttributes()
  {
		int32_t params = -1;
		const int kMaxLength = 100;
		glGetProgramiv(m_program_id, GL_ACTIVE_ATTRIBUTES, &params);
		for (int32_t i = 0; i < params; i++)
		{
			GLShader::Attribute attribute;
			char name[kMaxLength] = { 0 };
			int actual_length = 0;
			int size = 0;
			GLenum type;
			glGetActiveAttrib(m_program_id, i, kMaxLength, &actual_length, &size, &type, name);
			attribute.type = ShaderUtils::GLTypeToShaderDataType(type);
			attribute.size = size;
			for (int32_t element = 0; element < size; element++)
			{
				if (size == 1)
				{
					attribute.location = glGetAttribLocation(m_program_id, name);
					attribute.name = std::string(name);
				}
				else
				{
					std::string item_name = std::string{ name };
					item_name = item_name.substr(0, item_name.size() - 3) + std::string("[") + std::to_string(element) + "]";
					attribute.location = glGetAttribLocation(m_program_id, item_name.c_str());
					attribute.name = item_name;
				}
				m_attributes[attribute.name] = attribute;
			}
		}
  } 

  void GLShader::PrintInfo()
  {
#ifndef SPG_DEBUG
		return;
#endif

    int length = 0;
		glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &length);
    if(length > 0) 
    {
      char* message = new char[length];
      glGetProgramInfoLog(m_program_id, length, &length, message);
      SPG_TRACE("SHADER {} - GL_INFO_LOG: {}", m_name, message);
      delete[] message;
    }

		if (!m_build_success)
		{
			SPG_ERROR("SHADER BUILD FAILED: {}", m_name);
			return;
		}

		int32_t params = -1;

		SPG_INFO("SHADER CREATED -------------------------------------");
		SPG_TRACE("   Program Name: {}", m_name);
		SPG_TRACE("   Program Id: {}", m_program_id);
		
		glGetProgramiv(m_program_id, GL_LINK_STATUS, &params);
		SPG_TRACE("   GL_LINK_STATUS {}", params);
		glGetProgramiv(m_program_id, GL_ATTACHED_SHADERS, &params);
		SPG_TRACE("   GL_ATTACHED_SHADERS {}", params);
	
		SPG_INFO("GL_ACTIVE_ATTRIBUTES {}", m_attributes.size());
		for (auto& item : m_attributes)
		{
			SPG_TRACE("   {}: {}, size: {}, loc: {}", item.second.name, ShaderUtils::ShaderDataTypeToString(item.second.type), item.second.size, item.second.location);
		}
		SPG_INFO("GL_ACTIVE_UNIFORMS {}", m_uniforms.size());
		for (auto& item : m_uniforms)
		{
			SPG_TRACE("   {}: {}, size: {}, loc: {}", item.second.name, ShaderUtils::ShaderDataTypeToString(item.second.type), item.second.size, item.second.location);
		}
		SPG_INFO("GL_ACTIVE_UNIFORM_BLOCKS {}", m_uniform_blocks.size());
		for (auto& item : m_uniform_blocks)
		{
			SPG_TRACE("   {}: index: {} binding: {}, data size: {}, active uniforms: {}", item.second.name, item.second.index, item.second.binding, item.second.size, item.second.active_uniforms);
		}         
  }
}