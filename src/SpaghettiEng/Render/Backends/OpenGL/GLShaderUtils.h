#pragma once

namespace Spg
{

  enum class ShaderType : uint32_t
  {
    Vertex, Fragment, Geometry, TesselationControl, TesselationEvaluation, Compute, Unknown
  };

  enum class ShaderDataType
  {
    None = 0, Float, Float2, Float3, Float4, Mat2, Mat3, Mat4, Int, Int2, Int3, Int4, Bool, Sampler2D, Sampler3D, SamplerCube, Sample2DShadow
  };

  class ShaderUtils
  {
  public:
    static uint32_t GetGLShaderType(ShaderType type);
    static uint32_t ShaderDataTypeSize(ShaderDataType type);
    static uint32_t GetComponentCount(ShaderDataType type);
    static uint32_t ShaderDataTypeToGLBaseType(ShaderDataType type); //return type is GLenum which is an unsigned int 
    static bool GLBaseTypeIsFloat(ShaderDataType type);
    static bool GLBaseTypeIsInt(ShaderDataType type);
    static bool GLBaseTypeIsBool(ShaderDataType type);
    
    static void PrintUniformValue(uint32_t program, int location, uint32_t type);

    static std::string  ShaderDataTypeToString(ShaderDataType type); //param type is GLenum which is an unsigned int 
    static ShaderDataType GLTypeToShaderDataType(uint32_t gl_type); //param type is GLenum which is an unsigned int 
  };
}