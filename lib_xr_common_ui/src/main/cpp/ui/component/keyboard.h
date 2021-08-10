//
// Created by fcx@pingixngyun.com on 2019/12/13.
//

#ifndef CLOUDLARKVRDEMO_KEYBOARD_H
#define CLOUDLARKVRDEMO_KEYBOARD_H

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <object.h>
#include <ui/aa_bb.h>
#include <list>
#include "base.h"
#include "button.h"

class Keyboard: public lark::Object, public component::Base, public AABB  {
public:
    enum KeyboardType {
        // number keyboard.
        KeyboardType_Number = 0,
    };
    enum KeyType {
        KeyType_Input = 0,
        KeyTpye_Enter = 1,
        KeyTpye_Del   = 2,
    };
    struct Key {
        KeyType      keyType;
        std::string  keyValue;
        std::wstring title;
    };

    // keyboarbutton
    class KeyBoardButton: public Button {
    public:
        explicit KeyBoardButton(const Key &key);
        inline Key key() { return key_;}

        void clearState();
        // component base.
        virtual glm::vec2 GetSize() override;
        // aabb
        virtual void HandleInput(glm::vec2 * point, int pointCount) override;
    protected:
        void Init();
    private:
        Key key_;
        std::shared_ptr<ColorBox> bg_;
    };
    // keyboard callback
    class Callback {
    public:
        virtual void onInput(const Key &key) = 0;
    };
    //
    explicit Keyboard(Callback *callback);
    ~Keyboard();

    void Show();
    void Hide();

    virtual void SetAABBPositon(const glm::vec2 &aabbPosition) override;
    virtual void HandleInput(glm::vec2 * point, int pointCount) override;
    inline bool picked() { return picked_; }

    void SetTitle(const std::wstring &title);
private:
    std::shared_ptr<Text> title_;
    std::list<std::shared_ptr<KeyBoardButton>> key_list_;
    bool picked_;
    Callback* callback_;
};


#endif //CLOUDLARKVRDEMO_KEYBOARD_H
