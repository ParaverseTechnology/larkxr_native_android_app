//
// Created by fcx@pingixngyun.com on 2019/12/13.
//

#include <log.h>
#include <utils.h>
#include "keyboard.h"
#include "input.h"

#define LOG_TAG "keyboard"

//
// number keys.
//
namespace {
    const float KEY_GAP = 0.025;
    const float FUNC_KEY_WIDTH = 0.65;
    const float KEY_WIDTH = 0.28;
    const float KEY_HEIGHT = 0.28;
    const uint64_t KEY_COLOR_BG = 0x000530A2;
    const uint64_t KEY_COLOR_BG_ACTIVE = 0xFFFFFFA2;
    const float BG_HEIGHT = 0.3;
    const int NUMBER_KEY_SIZE = 13;
    const Keyboard::Key KEYS_NUMBER[NUMBER_KEY_SIZE] = {
            { Keyboard::KeyType_Input, "0", L"0" },
            { Keyboard::KeyType_Input, "1", L"1" },
            { Keyboard::KeyType_Input, "2", L"2" },
            { Keyboard::KeyType_Input, "3", L"3" },
            { Keyboard::KeyType_Input, "4", L"4" },
            { Keyboard::KeyType_Input, "5", L"5" },
            { Keyboard::KeyType_Input, "6", L"6" },
            { Keyboard::KeyType_Input, "7", L"7" },
            { Keyboard::KeyType_Input, "8", L"8" },
            { Keyboard::KeyType_Input, "9", L"9" },
            { Keyboard::KeyType_Input, ".", L"." },
//            { Keyboard::KeyType_Input, ":", L":" },
            { Keyboard::KeyTpye_Del,   "",  L"Delete" },
            { Keyboard::KeyTpye_Enter, "",  L"Enter" },
    };
}
// end number keys.

//
// keyboard button.
//
Keyboard::KeyBoardButton::KeyBoardButton(const Keyboard::Key &key): Button(key.title), key_(key) {
    Init();
}

void Keyboard::KeyBoardButton::Init() {
    if (key_.keyType == KeyTpye_Enter || key_.keyType == KeyTpye_Del)
        Base::size_ = glm::vec2(FUNC_KEY_WIDTH, KEY_HEIGHT);
    else
        Base::size_ = glm::vec2(KEY_WIDTH, KEY_HEIGHT);

    bg_ = std::make_shared<ColorBox>();
    bg_->set_size(Base::size_);
    bg_->set_color(KEY_COLOR_BG);
    bg_->set_position(glm::vec3(0, 0, -0.01));
    AddChild(bg_);

    title_->set_color(0xd7e1ffff);
    glm::vec3 titlePosition(0,0,0);
    titlePosition.x = (Base::size_.x - title_->GetSize().x) * 0.5F;
    titlePosition.y = (Base::size_.y - title_->GetSize().y) * 0.8F;
    titlePosition.z = 0;
    title_->set_position(titlePosition);
    AddChild(title_);

    AABB::SetAABBSize(Base::size_);
}

void Keyboard::KeyBoardButton::HandleInput(glm::vec2 *point, int pointCount) {
    bool pickedNow = false;
    for (int i = 0;i < pointCount; i ++) {
        if (i == Input::GetCurrentRayCastType()) {
            pickedNow = CheckPointIn(point[i]);
            break;
        }
    };

    if (!picked_ && pickedNow) {
        // change to picked.
        bg_->set_color(KEY_COLOR_BG_ACTIVE);
    } else if (picked_ && !pickedNow){
        // change to unpicked.
        bg_->set_color(KEY_COLOR_BG);
    }

    picked_ = pickedNow;
}

glm::vec2 Keyboard::KeyBoardButton::GetSize() {
    return Base::size_;
}

void Keyboard::KeyBoardButton::clearState() {
    picked_ = false;
    // change to unpicked.
    bg_->set_color(KEY_COLOR_BG);
}
// end keyboard button

//
//
//
Keyboard::Keyboard(Callback *callback):
    AABB(Object::id())
    , key_list_()
    , picked_(false)
    , callback_(callback) {

    float width = KEY_GAP / 2.0F;
    for(int i = 0; i < NUMBER_KEY_SIZE; i++) {
        std::shared_ptr<KeyBoardButton> button = std::make_shared<KeyBoardButton>(KEYS_NUMBER[i]);

        button->Move(width, 0, 0);
        button->SetAABBPositon(glm::vec2(width, 0));
        key_list_.push_back(button);
        AddChild(button);

        width += button->GetSize().x + KEY_GAP;
    }
    SetAABBSize(glm::vec2(width, KEY_HEIGHT));

    title_ = std::make_shared<Text>(L"数字键盘");
    title_->SetFontSize(22);
    title_->Move(0, KEY_HEIGHT + 0.08F, 0);
    AddChild(title_);
}

Keyboard::~Keyboard() = default;

void Keyboard::Show() {
}

void Keyboard::Hide() {
}

void Keyboard::HandleInput(glm::vec2 *point, int pointCount) {
    bool pickedNow = false;
    for (int i = 0;i < pointCount; i ++) {
        if (i == Input::GetCurrentRayCastType()) {
            pickedNow = CheckPointIn(point[i]);
            break;
        }
    };

    bool keyPickd = false;
    Key pickedKey = {};
    if (pickedNow) {
        for(const auto& key:key_list_) {
            key->HandleInput(point, pointCount);
            if (key->picked()) {
                // LOGV("picked: %s", key->getKey().keyValue.c_str());
                keyPickd = true;
                pickedKey = key->key();
            }
        }
    } else {
        // move out.
        if (picked_) {
            for(const auto& key:key_list_) {
                key->clearState();
            }
        }
    }
    picked_ = pickedNow;
    // find key picked callback
    if (keyPickd && Input::IsInputEnter() && callback_ != nullptr) {
        callback_->onInput(pickedKey);
    }
}

void Keyboard::SetAABBPositon(const glm::vec2 &aabbPosition) {
    AABB::SetAABBPositon(aabbPosition);

    float width = 0;
    for (const auto &key: key_list_) {
        key->SetAABBPositon(glm::vec2(aabbPosition.x + width, aabbPosition.y));
        width += key->GetSize().x + KEY_GAP;
    }
}

void Keyboard::SetTitle(const std::wstring &title) {
    if (title_)
        title_->SetText(title, true);
}


