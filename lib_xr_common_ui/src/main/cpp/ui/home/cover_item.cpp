//
// Created by fcx@pingixngyun.com on 2019/11/15.
//

#include <env_context.h>
#include "cover_item.h"
#include "log.h"
#include "vertex_array_object.h"
#include "ui/navigation.h"
#include "application.h"
#include "input.h"
#define LOG_TAG "cover_item"

using namespace glm;
using namespace std;

CoverItem::CoverItem(Navigation * navigation, const std::wstring& title):
    Object(),
    AABB(id()), // init aabb after object
    navigation_(navigation),
    picked_(false),
//        , mOriPoint(0,0,0)
    title_(title),
    bg_color_(),
    tail_(L""),
    cover_url_(""),
//        , cover_(nullptr)
    need_update_cover_(false),
    cover_texture_id_(0),
    size_(),
    bg_color_dark_(0.0, 0.019, 0.117, 0.2),
    bg_color_active_(0.0, 0.7, 0.0, 0.8),
    is_empty_(true),
    cover_loader_(this)
{
    name_ = LOG_TAG;

    size_.x = 1.15F;
//    size_.y = 1.175F;
    size_.y = 0.946875F;
    float totalW = size_.x;
    float totalH = size_.y;
    float titleW = totalW;
    float titleH = 0.3F;
    // 16x9 image
    float coverW = totalW;
    float coverH = 0.646875;

    // setup aabb size.
    AABB::SetAABBSize(size_);

    bg_color_.set_color(bg_color_dark_);
    bg_color_.set_size(vec2(titleW, titleH));
    // TODO use same z as text
    bg_color_.set_position(vec3(0,0,-0.001));
    bg_color_.set_parent(this);

    title_.set_color(vec4(0.843, 0.882, 1.0F, 1.0F));
    title_.set_container_size(vec2(titleW - 0.03F, titleH));
    title_.SetFontSize(24);
    title_.set_position(vec3(0.0, 0.075, 0.0));
    title_.set_parent(this);

    tail_.SetFontSize(36);
    tail_.set_parent(this);
    tail_.set_position(vec3(0.04, 0.09, 0.0));

    // init cover texure id
    glGenTextures(1, &cover_texture_id_);

    cover_ = std::make_shared<Image>();
    cover_->set_parent(this);
    cover_->set_position(vec3(0.0, titleH, 0.0));
    cover_->set_size(vec2(coverW, coverH));
//    cover_->set_scale(0.1);

    border_ = std::make_shared<Image>();
    border_->set_parent(this);
    border_->set_position(vec3(0.0, titleH, 0.0));
    border_->set_size(vec2(coverW, coverH));
    border_->SetPath("textures/ui/border.png", true);

    test_border_ = std::make_shared<Border>();
    test_border_->set_color(0xcfd9ff7F);
    test_border_->setLineWidth(0.01F);
    test_border_->set_size(vec2(coverW, coverH));
    test_border_->set_parent(this);
    test_border_->set_position(vec3(0.0, titleH, 0.0));

    active_border_ = std::make_shared<Image>();
    active_border_->set_parent(this);
    active_border_->set_position(vec3(-0.0125, -0.0125, -0.001));
    active_border_->set_size(vec2(totalW + 0.025F, totalH  + 0.025F));
    active_border_->SetPath("textures/ui/border_selected.png", true);
}

CoverItem::~CoverItem() = default;

void CoverItem::SetTitle(const std::wstring& title) {
    title_.SetText(title);
    // 居中显示
    auto position = title_.GetPosition();
    position.x = (size_.x - title_.width()) * 0.5F;
    title_.set_position(position);
}

void CoverItem::SetTail(const std::wstring& tail) {
    tail_.SetText(tail);
    auto positon = tail_.GetPosition();
    positon.x = 2.5F - tail_.width() - 0.04F;
    tail_.set_position(positon);
}

void CoverItem::SetCoverUrl(const std::string& coverUrl, bool isLocal) {
    if (cover_url_ == coverUrl) {
        return;
    }
    LOGV("set cover url %s %d", coverUrl.c_str(), isLocal);
    cover_url_ = coverUrl;
    if (isLocal) {
        if (cover_ != nullptr) {
            cover_->SetPath(coverUrl, isLocal);
        }
    } else {
        cover_loader_.GetCoverAsync(coverUrl);
    }
}

void CoverItem::SetNormalBgColor(const glm::vec4& normalColor) {
    bg_color_dark_ = normalColor;
}

void CoverItem::SetActiveBgColor(const glm::vec4& activeColor) {
    bg_color_active_ = activeColor;
}

void CoverItem::Draw(Eye eye, const glm::mat4& projection, const glm::mat4& eyeView) {
    Object::Draw(eye, projection, eyeView);

    if (is_empty_) {
//        border_->draw(projection, eye, view, lightDir);
        test_border_->Draw(eye, projection, eyeView);
    } else {
        float z = 0.0F;
        if (picked_) {
            z = 0.05F;
            auto ap = active_border_->GetPosition();
            ap.z = z - 0.002F;
            active_border_->set_position(ap);
            active_border_->Draw(eye, projection, eyeView);
        }
        auto cp = cover_->GetPosition();
        cp.z = z;
        cover_->set_position(cp);

        auto bp = bg_color_.GetPosition();
        bp.z = z - 0.001F;
        bg_color_.set_position(bp);

        auto tp = title_.GetPosition();
        tp.z = z;
        title_.set_position(tp);

        bg_color_.Draw(eye, projection, eyeView);
        title_.Draw(eye, projection, eyeView);
        cover_->Draw(eye, projection, eyeView);
    }
}

void CoverItem::HandleInput(glm::vec2 *point, int pointCount) {
    if (!Object::active()) {
        return;
    }
    // skip empty block.
    if (is_empty_) {
        return;
    }
    for (int i = 0;i < pointCount; i ++) {
        if (i == Input::GetCurrentRayCastType()) {
            picked_ = CheckPointIn(point[i]);
            break;
        }
    };
    Input::InputState intpuState = Input::GetCurrentInputState();
    if (picked_ && intpuState.enterShortPressed && !app_id_.empty()) {
        LOGV("=============enter app: %s", app_id_.c_str());
        navigation_->SetRouter(Navigation::LOADING);
        // TODO 进入应用
        if (Application::instance() != nullptr)
            Application::instance()->EnterAppli(app_id_);
    }
    if (picked_ && (intpuState.triggerShortPressed || intpuState.backButtonDown) && !app_id_.empty()) {
        larkxrSystemInfo systemInfo = lark::XRClient::system_info();
        if (systemInfo.platFromType == larkxrPlatFromType::Larkxr_Platform_PICO_NEO_2 ||
            systemInfo.platFromType == larkxrPlatFromType::Larkxr_Platform_Oculus_Quest) {
            Navigation::ShowToast("使用手柄A或X键可进入应用");
        } else {
            Navigation::ShowToast("使用手柄Trackpad可进入应用");
        }
    }
}

void CoverItem::OnImageLoadSuccess(const char *data, int size) {
    if (cover_ != nullptr) {
        LOGV("load iamge success size %d", size);
        cover_->LoadTexture(data, size);
    }
}

void CoverItem::OnImageLoadFailed(const std::string &err) {
    LOGW("load image failde %s", err.c_str());
}
