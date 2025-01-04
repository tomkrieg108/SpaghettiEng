#pragma once
#include <cstdint>

namespace Spg
{
  enum class GLShaderType : uint32_t
  {
    Vertex, Fragment, Geometry, TesselationControl, TesselationEvaluation, Compute, Unknown
  };

  struct GLShaderInfo
  {
		std::string filepath{ "" };
		GLShaderType type = GLShaderType::Unknown;
		uint32_t id = 0;
		bool compile_success = false;
  };

  class GLShader
  {
    public:

    private:

  };

}