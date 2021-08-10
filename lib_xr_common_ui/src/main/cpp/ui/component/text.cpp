//
// Created by fcx@pingxingyun.com on 2019/11/11.
//

#include "log.h"
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>
#include <utility>
#include <utils.h>
#include <env_context.h>
#include "text.h"

#define LOG_TAG "Text"
using namespace std;

Text::Text(std::wstring  text) :
    Object(),
    text_(std::move(text)),
    font_size_(36),
    font_name_("NotoSansCJK-Regular.ttc"),
    total_width_(0.0F),
    need_update_(false),
    overflow_mode_(TEXT_OVERFLOW_MODE_HIDEN),
    limit_charsize_(0),
    ft_(),
    face_(),
    t_vao_(0),
    t_vbo_(0) {
    name_ = LOG_TAG;
    Init();
}

Text::Text(std::wstring  text,
           uint size,
           std::string  fontName,
           const glm::vec4 & color,
           const glm::vec3 & position,
           float scale):
    Object(),
    text_(std::move(text)),
    font_size_(size),
    font_name_(std::move(fontName)),
    total_width_(0.0F),
    overflow_mode_(TEXT_OVERFLOW_MODE_HIDEN),
    ft_(),
    limit_charsize_(0),
    face_(),
    t_vao_(0),
    t_vbo_(0) {
    name_ = LOG_TAG;
    color_ = color;
    position_ = position;
    scale_ = scale;
    need_update_ = false;
    Init();
}

Text::~Text() {
    Text::Release();
}

void Text::Release() {
    if (face_ != nullptr) {
        FT_Done_Face(face_);
    }
    if (ft_ != nullptr) {
        FT_Done_FreeType(ft_);
    }
    ClearCharacters();
}

void Text::Init() {
    enable_ = false;
    int res;

    /* 初始化 FreeType 库 */
    res = FT_Init_FreeType(&ft_);
    /* 加载字体 */
    string fontPath = ANDROID_FONT_BASE + font_name_;
    res = FT_New_Face(ft_, fontPath.c_str(), 0, &face_);
//    res = FT_New_Face(ft_, "/system/fonts/Roboto-Black.ttf", 0, &face_);
    /* 设定为 UNICODE，默认也是 */
    res = FT_Select_Charmap(face_, FT_ENCODING_UNICODE);
    /* 定义字体大小 */
    // 尺寸中的任一个为0意味着“与另一个尺寸值相等”
    res = FT_Set_Pixel_Sizes(face_, 0, font_size_);

    if (res != 0) {
        LOGW("Init free type font err: %d;", res);
        return;
    }

    LoadShaderFromAsset(Context::instance()->asset_manager(),
            "shader/vertex/text_vertex.glsl", "shader/fragment/text_fragment.glsl");
    if (has_error_) {
        LOGW("loadShaderFromAsset text vertex has error");
        return;
    }

    model_location_ = shader_->GetUniformLocation("uModel");
    view_location_ = shader_->GetUniformLocation("uView");
    projection_location_ = shader_->GetUniformLocation("uProjection");

    color_loaction_ = shader_->GetUniformLocation("uColor");

    if (!UpdateChar()) {
        LOGW("texture render loadchar failed");
        return;
    }
    if (!InitVao()) {
        LOGW("texture render Init vao failed");
        return;
    }
    enable_ = true;
}

bool Text::InitVao() {
    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &t_vao_);
    glGenBuffers(1, &t_vbo_);
    glBindVertexArray(t_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, t_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 5, nullptr, GL_DYNAMIC_DRAW);

    int stride = (2 + 3) * sizeof(GLfloat);
    GLuint offset = 0;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (const void*) offset);
    offset += sizeof(float) * 3;

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void*) offset);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    return !HasGLError();

}

int Text::SetText(const std::wstring & text) {
    return SetText(text, true);
};

int Text::SetText(const std::wstring &text, bool updateNow) {
    if (text != text_) {
        // update
        text_.assign(text);
        if (updateNow) {
            UpdateChar();
        } else {
            need_update_ = true;
        }
    }
    return 0;
}

int Text::SetFontName(const std::string & fontName) {
    font_name_.assign(fontName);
    /* 加载字体 */
    string fontPath = ANDROID_FONT_BASE + fontName;
    return FT_New_Face(ft_, fontPath.c_str(), 0, &face_);
}

int Text::SetFontSize(uint fontSize) {
    /* 定义字体大小 */
    // 尺寸中的任一个为0意味着“与另一个尺寸值相等”
    font_size_ = fontSize;
    FT_Set_Pixel_Sizes(face_, 0, font_size_);
//    UpdateChar();
    need_update_ = true;
    return 0;
}

void Text::ClearCharacters() {
    if (characters_.empty())
        return;
    for(auto & mCharacter : characters_) {
        glDeleteTextures(1, &mCharacter.second.TextureID);
    }
    characters_.clear();
}

bool Text::UpdateChar() {
    need_update_ = false;
    ClearCharacters();
    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Iterate through all characters
    float totalW = 0.0F;
    int charNum = 0;
    for (wchar_t c : text_)
    {
        charNum ++;
        // Load character glyph
        if (FT_Load_Char(face_, static_cast<FT_ULong>(c), FT_LOAD_RENDER))
        {
            LOGW("ERROR::FREETYTPE: Failed to load Glyph %d", c);
            continue;
        }
        if (face_->glyph->bitmap.buffer == nullptr) {
            if (c == 0x20) { // 空格
                FT_Pos advanceX = face_->glyph->advance.x;
                totalW += (advanceX >> 6) * scale_ * component::UNIT_PIXEL_SCALE;

                // Now store character for later use
                Character character = {
                        0,
                        glm::ivec2(advanceX, advanceX),
                        glm::ivec2(advanceX, advanceX),
                        static_cast<GLuint>(advanceX),
                        false,
                };
                characters_.insert(pair<wchar_t , Character>(c, character));
            } else {
                LOGW("font face error %d, %ld", c, face_->glyph->advance.x >> 6);
            }
            continue;
        }
        // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        totalW += (face_->glyph->advance.x >> 6) * scale_ * component::UNIT_PIXEL_SCALE;
        if (overflow_mode_ == TEXT_OVERFLOW_MODE_HIDEN && container_size_.x > 0 && totalW > container_size_.x) {
            break;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        if (HasGLError()) {
            return false;
        }
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_LUMINANCE , //  **文字渲染使用** GL_LUMINANCE
                face_->glyph->bitmap.width,
                face_->glyph->bitmap.rows,
                0,
                GL_LUMINANCE , //  **文字渲染使用** GL_LUMINANCE
                GL_UNSIGNED_BYTE,
                face_->glyph->bitmap.buffer
        );
        if (HasGLError()) {
            return false;
        }
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (HasGLError()) {
            return false;
        }
        // Now store character for later use
        Character character = {
                texture,
                glm::ivec2(face_->glyph->bitmap.width, face_->glyph->bitmap.rows),
                glm::ivec2(face_->glyph->bitmap_left, face_->glyph->bitmap_top),
                static_cast<GLuint>(face_->glyph->advance.x),
                true // 可见字符
        };
        characters_.insert(pair<wchar_t , Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    total_width_ = totalW;
    limit_charsize_ = charNum;

    if (HasGLError()) {
        LOGW("render text has error");
        return false;
    }
    return true;
}

void Text::Draw(Eye eye, const glm::mat4& projection, const glm::mat4& view)  {
    Object::Draw(eye, projection, view);

    if (!enable_)
        return;

    if (text_.empty())
        return;

    if (need_update_)
        UpdateChar();

    // Activate corresponding render state
    shader_->UseProgram();
    // mvp
    glUniformMatrix4fv(model_location_, 1, GL_FALSE, glm::value_ptr(GetTransforms()));
    glUniformMatrix4fv(view_location_, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection_location_, 1, GL_FALSE, glm::value_ptr(projection));
    // color
    glUniform4fv(color_loaction_, 1, glm::value_ptr(color_));
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(t_vao_);

    GLfloat x = position_.x;
    GLfloat y = position_.y;
    GLfloat scale = scale_;
    int charNum = 0;
    // Iterate through all characters
    for (wchar_t & c : text_)
    {
        charNum++;
        if (overflow_mode_ == TEXT_OVERFLOW_MODE_HIDEN && limit_charsize_ > 0 && charNum > limit_charsize_) {
            break;
        }
        auto cit = characters_.find(c);
        if (cit == characters_.end()) {
            continue;
        }
        Character ch = cit->second;

        GLfloat xpos = (x + ch.Bearing.x * component::UNIT_PIXEL_SCALE) * scale;
        GLfloat ypos = (y - component::UNIT_PIXEL_SCALE * (ch.Size.y - ch.Bearing.y)) * scale;
//        GLfloat ypos = y * scale;

        GLfloat w = ch.Size.x * scale * component::UNIT_PIXEL_SCALE;
        GLfloat h = ch.Size.y * scale * component::UNIT_PIXEL_SCALE;
        GLfloat z = position_.z;

        // Update VBO for each character
        GLfloat vertices[] = {
                xpos,     ypos + h, z, 0.0, 0.0,
                xpos,     ypos,     z, 0.0, 1.0,
                xpos + w, ypos,     z, 1.0, 1.0,

                xpos,     ypos + h, z, 0.0, 0.0,
                xpos + w, ypos,     z, 1.0, 1.0,
                xpos + w, ypos + h, z, 1.0, 0.0
        };

        if (ch.visible) { // 去掉不可见字符。
            // Render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // Update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, t_vbo_);
            // Be sure to use glBufferSubData and not glBufferData
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        x += (ch.Advance >> 6) * scale * component::UNIT_PIXEL_SCALE;
//        x += w;
    }

    shader_->UnUseProgram();
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

//    HasGLError();
}

glm::vec2 Text::GetSize() {
    return glm::vec2(total_width_, font_size_ * component::UNIT_PIXEL_SCALE * scale_);
}
