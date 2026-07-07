#include "GL32TextRenderer.h"

#include "ft2build.h"
#include <freetype/freetype.h>

namespace Spg
{
  GLTextRenderer::GLTextRenderer(Camera2D& camera) : m_camera{camera}
  {
    GLShaderBuilder shader_builder;
    m_shader = shader_builder.Add(ShaderType::Vertex, "text.vs").Add(ShaderType::Fragment, "text.fs").Build("Text Shader");

    m_shader->Bind();
    m_shader->SetUniform1i("u_text", 0);
    auto model = glm::mat4(1.0f);
    m_shader->SetUniformMat4f("u_model", model);
    m_shader->SetUniformMat4f("u_view", m_camera.GetViewMatrix());
    m_shader->SetUniformMat4f("u_proj", m_camera.GetProjMatrix());
    m_shader->Unbind();

    m_font_path = fs::current_path() / fs::path{"Fonts"};
    m_font_path = fs::absolute(m_font_path);
    SPG_INFO("Absolute fonts path: {}", m_font_path.string());

    if (!fs::exists(m_font_path)) {
        SPG_ERROR("Font path does not exist: {}", m_font_path.string());
        return;
    }
    if(!fs::is_directory(m_font_path))
      SPG_ERROR("Font path is not a directory: {}", m_font_path.string());

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
      SPG_ERROR("Could not init FreeType Library");
      return;
    }
    fs::path file_path =  fs::absolute(m_font_path / fs::path{"Opensans/OpenSans-Regular.ttf"} );
    if (!fs::exists(file_path)) {
        SPG_ERROR("Font does not exist: {}", m_font_path.string());
        return;
    }
    std::string font_name = file_path.string();
    FT_Face face;
    if (FT_New_Face(ft, file_path.string().c_str(), 0, &face)) {
      std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
      return;
    }
    else {
      // set size to load glyphs as
      FT_Set_Pixel_Sizes(face, 0, 48);

      // disable byte-alignment restriction
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
      // Load character glyph 
      if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      {
          std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
          continue;
      }
      // generate texture
      unsigned int texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(
          GL_TEXTURE_2D,
          0,
          GL_RED,
          face->glyph->bitmap.width,
          face->glyph->bitmap.rows,
          0,
          GL_RED,
          GL_UNSIGNED_BYTE,
          face->glyph->bitmap.buffer
      );
      // set texture options
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // now store character for later use
      Character character = {
          texture,
          glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
          glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
          static_cast<uint32_t>(face->glyph->advance.x)
      };
      m_characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  
  void GLTextRenderer::UpdateView()
  {
    m_shader->Bind();
    m_shader->SetUniformMat4f("u_view", m_camera.GetViewMatrix());
    m_shader->SetUniformMat4f("u_proj", m_camera.GetProjMatrix());
    m_shader->Unbind();
  }

  // render line of text
  void GLTextRenderer::Render(std::string text, float x, float y, float scale, glm::vec3 color)
  {
    m_shader->Bind();
    glUniform3f(glGetUniformLocation(m_shader->GetProgramID(), "u_text_color"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = m_characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f }, //bottom left
            { xpos,     ypos,       0.0f, 1.0f }, //top left
            { xpos + w, ypos,       1.0f, 1.0f }, //top right

            { xpos,     ypos + h,   0.0f, 0.0f }, //bottom left
            { xpos + w, ypos,       1.0f, 1.0f }, //top right
            { xpos + w, ypos + h,   1.0f, 0.0f }  //bottom right
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_shader->Unbind();
  }

}