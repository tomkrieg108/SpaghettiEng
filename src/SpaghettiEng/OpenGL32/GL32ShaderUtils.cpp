#include <glad/gl.h>
#include "Core/Application.h"
#include "GL32ShaderUtils.h"

namespace Spg
{
  uint32_t ShaderUtils::GetGLShaderType(ShaderType type)
  {
    switch (type)
    {
      case ShaderType::Vertex: return GL_VERTEX_SHADER;
      case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
      case ShaderType::TesselationControl: return GL_TESS_CONTROL_SHADER;
      case ShaderType::TesselationEvaluation: return GL_TESS_EVALUATION_SHADER;
      case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
      case ShaderType::Compute: return GL_COMPUTE_SHADER;
    };
    return 0;
  }

  uint32_t ShaderUtils::ShaderDataTypeSize(ShaderDataType type)
  {
    switch (type)
    {
      case ShaderDataType::Float:    return 4;
      case ShaderDataType::Float2:   return 4 * 2;
      case ShaderDataType::Float3:   return 4 * 3;
      case ShaderDataType::Float4:   return 4 * 4;
      case ShaderDataType::Mat3:     return 4 * 3 * 3;
      case ShaderDataType::Mat4:     return 4 * 4 * 4;
      case ShaderDataType::Int:      return 4;
      case ShaderDataType::Int2:     return 4 * 2;
      case ShaderDataType::Int3:     return 4 * 3;
      case ShaderDataType::Int4:     return 4 * 4;
      case ShaderDataType::Bool:     return 1;
    }
    return 0;
  }

  uint32_t ShaderUtils::GetComponentCount(ShaderDataType type)
  {
    switch (type)
    {
      case ShaderDataType::Float:   return 1;
      case ShaderDataType::Float2:  return 2;
      case ShaderDataType::Float3:  return 3;
      case ShaderDataType::Float4:  return 4;
      case ShaderDataType::Mat3:    return 3; // 3* float3
      case ShaderDataType::Mat4:    return 4; // 4* float4
      case ShaderDataType::Int:     return 1;
      case ShaderDataType::Int2:    return 2;
      case ShaderDataType::Int3:    return 3;
      case ShaderDataType::Int4:    return 4;
      case ShaderDataType::Bool:    return 1;
    }
    return 0; 
  }

  uint32_t ShaderUtils::ShaderDataTypeToGLBaseType(ShaderDataType type)
  {  //return type is GLenum which is an unsigned int 
    switch (type)
    {
      case ShaderDataType::Float:
      case ShaderDataType::Float2:
      case ShaderDataType::Float3:
      case ShaderDataType::Float4:
      case ShaderDataType::Mat3:
      case ShaderDataType::Mat4:
        return GL_FLOAT;
      case ShaderDataType::Int:
      case ShaderDataType::Int2:
      case ShaderDataType::Int3:
      case ShaderDataType::Int4:
      case ShaderDataType::Sampler2D:
        return GL_INT;
      case ShaderDataType::Bool:
        return GL_BOOL;
    }
    return 0;
  }
    
  bool ShaderUtils::GLBaseTypeIsFloat(ShaderDataType type)
  {
    return ShaderDataTypeToGLBaseType(type) == GL_FLOAT;
  }

  bool ShaderUtils::GLBaseTypeIsInt(ShaderDataType type)
  {
    return ShaderDataTypeToGLBaseType(type) == GL_INT;
  }

  bool ShaderUtils::GLBaseTypeIsBool(ShaderDataType type)
  {
    return ShaderDataTypeToGLBaseType(type) == GL_BOOL;
  }
  
  void ShaderUtils::PrintUniformValue(uint32_t program, int location, uint32_t type)
  {
    if (type == GL_INT)
    {
        int val = 0;
        glGetUniformiv(program, location, &val);
        SPG_INFO("Val: {}", val);
    }
    if (type == GL_SAMPLER_2D)
    {
        int val = 0;
        glGetUniformiv(program, location, &val);
        SPG_INFO("Val: {}", val);
    }
    if (type == GL_FLOAT)
    {
        float val = 0;
        glGetUniformfv(program, location, &val);
        SPG_INFO("Val: {}", val);
    }
  }

  std::string ShaderUtils::ShaderDataTypeToString(ShaderDataType type)  
  { //param type is GLenum which is an unsigned int
    switch (type)
    {
      case ShaderDataType::Bool: return std::string("GL_BOOL");
      case ShaderDataType::Int: return std::string("GL_INT");
      case ShaderDataType::Int2: return std::string("GL_INT_VEC2");
      case ShaderDataType::Int3: return std::string("GL_INT_VEC3");
      case ShaderDataType::Int4: return std::string("GL_INT_VEC4");
      case ShaderDataType::Float: return std::string("GL_FLOAT");
      case ShaderDataType::Float2: return std::string("GL_FLOAT_VEC2");
      case ShaderDataType::Float3: return std::string("GL_FLOAT_VEC3");
      case ShaderDataType::Float4: return std::string("GL_FLOAT_VEC4");
      case ShaderDataType::Mat2: return std::string("GL_FLOAT_MAT2");
      case ShaderDataType::Mat3: return std::string("GL_FLOAT_MAT3");
      case ShaderDataType::Mat4: return std::string("GL_FLOAT_MAT4");
      case ShaderDataType::Sampler2D: return std::string("GL_SAMPLER_2D");
      case ShaderDataType::Sampler3D: return std::string("GL_SAMPLER_3D");
      case ShaderDataType::SamplerCube: return std::string("GL_SAMPLER_CUBE");
      case ShaderDataType::Sample2DShadow: return std::string("GL_SAMPLER_2D_SHADOW");
      default: return std::string("OTHER");
    }
  }

  ShaderDataType ShaderUtils::GLTypeToShaderDataType(uint32_t gl_type) 
  { //param type is GLenum which is an unsigned int 
    switch (gl_type)
    {
      case GL_BOOL: return ShaderDataType::Bool;
      case GL_INT: return ShaderDataType::Int;
      case GL_INT_VEC2: return ShaderDataType::Int2;
      case GL_INT_VEC3: return ShaderDataType::Int3;
      case GL_INT_VEC4: return ShaderDataType::Int4;
      case GL_FLOAT: return ShaderDataType::Float;
      case GL_FLOAT_VEC2: return ShaderDataType::Float2;
      case GL_FLOAT_VEC3: return ShaderDataType::Float3;
      case GL_FLOAT_VEC4: return ShaderDataType::Float4;
      case GL_FLOAT_MAT2: return ShaderDataType::Mat2;
      case GL_FLOAT_MAT3: return ShaderDataType::Mat3;
      case GL_FLOAT_MAT4: return ShaderDataType::Mat4;
      case GL_SAMPLER_2D: return ShaderDataType::Sampler2D;
      case GL_SAMPLER_3D: return ShaderDataType::Sampler3D;
      case GL_SAMPLER_CUBE: return ShaderDataType::SamplerCube;
      case GL_SAMPLER_2D_SHADOW: return ShaderDataType::Sample2DShadow;
      default: return ShaderDataType::None;
    }
  }

}