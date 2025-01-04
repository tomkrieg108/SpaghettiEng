
#include <glad/gl.h>
#include <string>
#include <fstream>
#include "Core/Base.h"
#include "GL32Shader.h"

using namespace std::string_literals;

namespace Spg
{
  static std::string ReadSource(const std::string& filepath);
  static GLShaderInfo Compile(uint32_t type, const std::string& filepath, const std::string& source_code);

  static std::string ReadSource(const std::string& filepath)
  {
    std::ifstream stream(filepath);
    if (stream.fail())
    {
        SPG_ERROR("Could not open shader file: {}", filepath);
        return ""s;
    }
    std::string line;
    std::ostringstream ss;
    while (getline(stream, line))
        ss << line << "\n";
      
    return ss.str();
  }

  static GLShaderInfo Compile(uint32_t type, const std::string& filepath, const std::string& source_code)
  {
    
  }

}