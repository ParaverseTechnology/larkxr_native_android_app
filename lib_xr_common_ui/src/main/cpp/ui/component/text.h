//
// Created by fcx@pingxingyun.com on 2019/11/11.
//

#ifndef MY_APPLICATION_TEXT_H
#define MY_APPLICATION_TEXT_H

#include <string>
#include <map>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// free type
#include <ft2build.h>
#include FT_FREETYPE_H
#include "object.h"
#include "base.h"

const static std::string ANDROID_FONT_BASE = "/system/fonts/";
class Text: public lark::Object, public component::Base {
public:
    enum OverflowMode {
        TEXT_OVERFLOW_MODE_HIDEN,
        TEXT_OVERLOW_MODE_SHOW,
    };

    /// Holds all state information relevant to a character as loaded using FreeType
    struct Character {
        GLuint TextureID;    // ID handle of the glyph texture
        glm::ivec2 Size;     // Size of glyph
        glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
        GLuint Advance;      // Horizontal offset to advance to next glyph
        bool visible;
    };

    explicit Text(std::wstring  text);
    Text(std::wstring  text,
         uint size,
         std::string  fontName,
         const glm::vec4 & color,
         const glm::vec3 & position,
         float scale);

    ~Text();

    void Release();

    // set text and update now.
    // must called in render thread.
    int SetText(const std::wstring & text);
    int SetText(const std::wstring & text, bool updateNow);
    inline std::wstring text() { return text_; };
    inline bool IsEmpty() { return  text_.empty(); };
    int SetFontSize(uint fontSize);
    inline uint font_size() { return font_size_; };
    int SetFontName(const std::string & fontName);
    inline std::string font_name() { return font_name_; };
    glm::vec2 GetSize() override;
    inline float width() { return total_width_; }
    // overflow mode
    inline void set_overflow_mode(OverflowMode mode) { overflow_mode_ = mode; };
    inline OverflowMode overflow_mode() { return overflow_mode_; };

    // render
    void Draw(Eye eye, const glm::mat4& projection, const glm::mat4& view) override;
private:
    void Init();
    bool InitVao();
    void ClearCharacters();
    bool UpdateChar();
private:
    std::map<wchar_t , Character> characters_;

    FT_Library ft_;
    FT_Face face_;
    GLuint t_vao_;
    GLuint t_vbo_;

    FT_UInt font_size_;
    std::wstring text_;
    std::string font_name_;

    float total_width_;
    bool need_update_;

    OverflowMode overflow_mode_;
    int limit_charsize_;

    int model_location_ = 0;
    int view_location_ = 0;
    int projection_location_ = 0;

    int color_loaction_ = 0;
};


#endif //MY_APPLICATION_TEXT_H
