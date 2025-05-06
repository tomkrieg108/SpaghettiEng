#include <glm/glm.hpp>
#include "GL32Shader.h"
#include <SpaghettiEng/Camera/Camera2D.h>
#include <Common/Common.h>
#include <map>

namespace Spg
{
  namespace fs = std::filesystem;

  class GLTextRenderer
  {
    public:
      GLTextRenderer(Camera2D& camera);
      void Render(std::string text, float x, float y, float scale, glm::vec3 color);
      void UpdateView();

    private:
      // Holds all state information relevant to a character as loaded using FreeType
      struct Character {
        uint32_t TextureID; // ID handle of the glyph texture
        glm::ivec2   Size;      // Size of glyph
        glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
        uint32_t Advance;   // Horizontal offset to advance to next glyph
      };

      //std::map<GLchar, Character> m_characters;
      std::map<char, Character> m_characters;
      uint32_t m_VAO = 0, m_VBO = 0;
      Scope<GLShader> m_shader;
      fs::path m_font_path{""};
      Camera2D& m_camera;
  };

}