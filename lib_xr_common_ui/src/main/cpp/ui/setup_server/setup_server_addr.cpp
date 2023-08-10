//
// Created by fcx@pingixngyun.com on 2019/12/4.
//

#include <sstream>
#include <log.h>
#include <env_context.h>
#include <utils.h>
#include <ui/localization.h>
#include <application.h>
#include <ui/home/home.h>
#include <ui/navigation.h>
#include "setup_server_addr.h"
#include "lark_xr/xr_client.h"
#include "lark_xr/request/get_appli_list.h"
#include "lark_xr/request/vrclient_heartbeat.h"
#include "lark_xr/xr_config.h"
#include "lark_xr/request/get_core_server_version_info.h"

namespace {
    // 3.0 change to 8181
    const uint16_t DEFAULT_PORT            = 8181;
    const uint64_t INPUT_LINE_COLOR        = 0xcfd9ff7F;
    const uint64_t INPUT_LINE_COLOR_ACTIVE = 0xcfd9ffFF;
    const float INPUT_BOX_WIDTH            = 1.0F;
    const float INPUT_BOX_HEIGHT           = 0.12F;
}

using namespace lark;

SetupServerAddr::SetupServerAddr(Navigation *navigation):
    View(navigation),
    get_applist_adapter_(std::bind(&SetupServerAddr::GetAppList, this))
{
    SetupServerAddr::Init();
}

SetupServerAddr::~SetupServerAddr() = default;

void SetupServerAddr::Init() {
    title_ = std::make_shared<Text>(localization::Loader::getResource().ui_home_setup_serveraddr);
    title_->Move(-title_->GetSize().x / 2.0F, 1.5F - title_->GetSize().y / 2.0F, 0);
    AddChild(title_);

    keyboard_ = std::make_shared<Keyboard>(this);
    keyboard_->Move(-1.8F, -1.0F, 0);
    keyboard_->SetAABBPositon(glm::vec2(-1.8F, -1.0F));
    PushAABB(keyboard_.get());
    AddChild(keyboard_);
    keyboard_->set_active(false);

    {
        label_server_ip_ = std::make_shared<Text>(localization::Loader::getResource().ui_setup_serveraddr_ip);
        label_server_ip_->Move(-1.5F, 1, 0);
        label_server_ip_->SetFontSize(26);
        AddChild(label_server_ip_);

        server_ip_button_ = std::make_shared<InputButton>(L"");
        server_ip_button_->Move(-0.6F, 1, 0);
        server_ip_button_->SetAABBPositon(glm::vec2(-0.6F, 1 - INPUT_BOX_HEIGHT * 0.5));
        PushAABB(server_ip_button_.get());
        AddChild(server_ip_button_);
    }

    {
        label_server_port_ = std::make_shared<Text>(localization::Loader::getResource().ui_setup_serveraddr_port);
        label_server_port_->Move(-1.5F, 0.8, 0);
        label_server_port_->SetFontSize(26);
        AddChild(label_server_port_);

        server_port_button_ = std::make_shared<InputButton>(L"");
        server_port_button_->Move(-0.6F, 0.8, 0);
        server_port_button_->SetAABBPositon(glm::vec2(-0.6F, 0.8 - INPUT_BOX_HEIGHT * 0.5));
        PushAABB(server_port_button_.get());
        AddChild(server_port_button_);
    }

    {
        label_mode_ = std::make_shared<Text>(localization::Loader::getResource().ui_setup_serveraddr_currentmode);
        label_mode_->Move(-1.5F, 0.6, 0);
        label_mode_->SetFontSize(26);
        AddChild(label_mode_);

        mode_text_ = std::make_shared<Text>(localization::Loader::getResource().ui_setup_serveraddr_hand_mode);
        mode_text_->Move(-0.6F, 0.6F, 0);
        mode_text_->SetFontSize(26);
        AddChild(mode_text_);
    }

    {
        submit_button_ = std::make_shared<TextButton>(localization::Loader::getResource().ui_setup_serveraddr_submit);
        submit_button_->SetFontSize(24);
        submit_button_->Move(-0.6F, 0.4, 0);
        submit_button_->SetAABBPositon(glm::vec2(-0.6F, 0.4 - submit_button_->GetSize().y / 2));
        submit_button_->SetAABBSize(submit_button_->GetSize());
        PushAABB(submit_button_.get());
        AddChild(submit_button_);
    }

    {
        status_messsage_ = std::make_shared<Text>(L"");
        status_messsage_->Move(-1.5F, -1, 0);
        status_messsage_->SetFontSize(26);
        AddChild(status_messsage_);
    }

    {
        region_title_ = std::make_shared<Text>(localization::Loader::getResource().ui_setup_serveraddr_region_title);
        region_title_->Move(-1.5f, 0.1f, 0);
        region_title_->SetFontSize(22);
        region_title_->set_active(false);
        AddChild(region_title_);

        // region list max 10 region
        for(int i = 0; i < 10; i++) {
            std::shared_ptr<TextButton> button = std::make_shared<TextButton>(L"");
            button->Move(-0.6f, 0.1f - i * 0.15f, 0);
            button->SetFontSize(22);
            button->set_active(false);
            button->SetAABBPositon(glm::vec2(-0.6F, 0.1f - i * 0.15f - button->GetSize().y / 2));
            button->SetAABBSize(button->GetSize());
            PushAABB(button.get());
            AddChild(button);
            region_list_.push_back(button);
        }

        get_region_list_ = std::make_shared<lark::GetRegionList>();
        get_region_list_->set_listener(this);

        region_rtt_test_ = std::make_shared<lark::RegionRttTest>();
        region_rtt_test_->set_listener(this);

        get_region_list_->SendAsync();
    }

    // sync mode from java.
    getServerAddr();
    onInputModeChange(InputMode_None);
    View::Init();
}

void SetupServerAddr::HandleInput(Ray *rays, int rayCount) {
    View::HandleInput(rays, rayCount);
    bool isEnter = Input::IsInputEnter();
    // change input mode.
    if (server_ip_button_->picked() && isEnter) {
        onInputModeChange(InputMode_Ip);
    } else if (server_port_button_->picked() && isEnter) {
        onInputModeChange(InputMode_Port);
    }

    // 设置服务器地址
    if (submit_button_->picked() && isEnter) {
        setServerAddr();
    }

    for(int i = 0; i < region_list_.size(); i ++) {
        if (region_list_[i]->active() && region_list_[i]->picked() && isEnter) {
            if (i < results_.size()) {
                LOGV("region [%s] picked rtt %ld", results_[i].info.regionName.c_str(), results_[i].rtt);
                for (int j = 0; j < results_.size(); j++) {
                    results_[j].selected = i == j;
                }
                selected_result_ = results_[i];
                need_update_region_ = true;
            }
        }
    }
}

void SetupServerAddr::onInput(const Keyboard::Key &key) {
    LOGV("=========on input %s", key.keyValue.c_str());
    bool isInput = false;
    bool isEnter = false;
    if (input_mode_ == InputMode_Ip) {
        std::string ip = utils::WStringToString(server_ip_button_->GetText());
        if (key.keyType == Keyboard::KeyType_Input) {
            ip += key.keyValue;
        } else {
            // edl
            if (key.keyType == Keyboard::KeyTpye_Del) {
                if (!ip.empty())
                    ip.pop_back();
            } else if (key.keyType == Keyboard::KeyTpye_Enter) {
                isEnter = true;
            }
        }
        server_ip_button_->SetText(ip.empty() ? L"" : utils::StringToWstring(ip));
    } else if (input_mode_ == InputMode_Port) {
        std::string port = utils::WStringToString(server_port_button_->GetText());
        if (key.keyType == Keyboard::KeyType::KeyType_Input) {
            port += key.keyValue;
        } else {
            if (key.keyType == Keyboard::KeyTpye_Del) {
                if (!port.empty())
                    port.pop_back();
            } else if (key.keyType == Keyboard::KeyTpye_Enter) {
                isEnter = true;
            }
        }
        server_port_button_->SetText(port.empty() ? L"" : utils::StringToWstring(port));
    }

    if (isEnter) {
        keyboard_->set_active(false);
        // save to change.
        setServerAddr();
    } else {
        if (currnet_mode_ != Mode_Manual) {
            currnet_mode_ = Mode_Manual;
            // setmode to manual.
//            setMode();
            mode_text_->SetText(currnet_mode_ == Mode_Audo ?
                               localization::Loader::getResource().ui_setup_serveraddr_auto_mode
                               : localization::Loader::getResource().ui_setup_serveraddr_hand_mode);
        }
    }
}

void SetupServerAddr::Enter() {
    View::Enter();

    getServerAddr();

    if (std::string(lark::XRClient::GetServerHost()).empty()) {
        status_messsage_->SetText(localization::Loader::getResource().ui_setup_serveraddr_request);
    }
    is_detecting_ = false;
}

void SetupServerAddr::Leave() {
    View::Leave();

    onInputModeChange(InputMode_None);

    status_messsage_->SetText(L"");
    is_detecting_ = false;
}

void SetupServerAddr::onInputModeChange(InputMode mode) {
    input_mode_ = mode;
    server_ip_button_->setSelected(input_mode_ == InputMode_Ip);
    server_port_button_->setSelected(input_mode_ == InputMode_Port);
    if (input_mode_ != InputMode_None) {
        keyboard_->set_active(true);
        keyboard_->SetTitle(input_mode_ == InputMode_Ip ? localization::Loader::getResource().ui_setup_serveraddr_input_ip
                                                       : localization::Loader::getResource().ui_setup_serveraddr_input_port);
    } else {
        keyboard_->set_active(false);
        keyboard_->SetTitle(localization::Loader::getResource().ui_keyboard_number);
    }
}

void SetupServerAddr::getServerAddr() {
    // TODO 服务器地址
    //server_ip_button_->SetText(utils::StringToWstring(lark::XRClient::GetServerHost()));
    //label_server_ip_->SetText(utils::StringToWstring("222.128.6.137"));
    server_ip_button_->SetText(utils::StringToWstring(lark::XRClient::GetServerHost()));
    //server_ip_button_->SetText(utils::StringToWstring("222.128.6.137"));
    uint16_t port = lark::XRClient::GetServerPort();
    std::string s_port = port == 0 ? std::to_string(DEFAULT_PORT) : std::to_string(port);
    server_port_button_->SetText(utils::StringToWstring(s_port));
}

void SetupServerAddr::setServerAddr() {
    std::string ip = utils::WStringToString(server_ip_button_->GetText());
    uint16_t port = (uint16_t)std::stoi(utils::WStringToString(server_port_button_->GetText()));

    if (Home::instance()) {
        Home::instance()->ResetAppPageInfo();
    }

    Application* app =  Application::instance();
    if (app != nullptr) {
        app->SetServerAddr(ip, port);
    }
    if (!is_detecting_) {
        std::thread thread(get_applist_adapter_);
        thread.detach();
    } else {
        // 正在检测中
        Navigation::ShowToast(utils::WStringToString(localization::Loader::getResource().ui_setup_serveraddr_detecting));
    }

    // update region
    if (!ip.empty() && port != 0 && get_region_list_) {
        get_region_list_->SendAsync();
    }
    {
        std::lock_guard<std::mutex> lock(region_mutex_);
        results_.clear();
        current_test_ = 0;
        selected_result_ = { 0, false };
        need_update_region_ = true;
        if (callback_) {
            callback_->OnUpdateRegion(selected_result_);
        }
    }
}

void SetupServerAddr::GetAppList() {
    is_detecting_ = true;
    // L"检测服务器版本号"
    status_messsage_->SetText(localization::Loader::getResource().ui_setup_serveraddr_check_server_version, false);
    lark::GetCoreServerVersionInfo getCoreServerVersionInfo;
    getCoreServerVersionInfo.Send();
    if (getCoreServerVersionInfo.is_success()) {
        lark::CoreServerVersionInfo info = getCoreServerVersionInfo.core_server_version_info();
        LOGV("core server version info %d.%d.%d.%d; sdk version %d.%d.%d.%d",
             info.marjor, info.minor, info.revise, info.build,
             LARK_NATIVE_CLIENT_SDK_VERSION_MARJOR,
             LARK_NATIVE_CLIENT_SDK_VERSION_MINOR,
             LARK_NATIVE_CLIENT_SDK_VERSION_REVISE,
             LARK_NATIVE_CLIENT_SDK_VERSION_BUILD);
        if (info.marjor == LARK_NATIVE_CLIENT_SDK_VERSION_MARJOR && info.minor == LARK_NATIVE_CLIENT_SDK_VERSION_MINOR) {
            // L"检测服务器版本号成功"
            status_messsage_->SetText(localization::Loader::getResource().ui_setup_serveraddr_server_version_success, false);
        } else {
            char buffer[100];
            // 检测到与服务器版本号不匹配，服务器版本
            // 客户端 SDK 版本
            sprintf(buffer, "Server [%d.%d] Client [%d.%d]",
                                              info.marjor, info.minor,
                                              LARK_NATIVE_CLIENT_SDK_VERSION_MARJOR,
                                              LARK_NATIVE_CLIENT_SDK_VERSION_MINOR);
            // "检测到与服务器版本号不匹配，服务器版本"
            std::string msg = utils::WStringToString(localization::Loader::getResource().ui_setup_serveraddr_server_version_missmatch)
                    + std::string(buffer);
            status_messsage_->SetText(utils::StringToWstring(msg), false);
            // reset status
            is_detecting_ = false;
            return;
        }
    } else {
        // "检测服务器版本号请求失败，与服务器版本可能不匹配, 请检查地址是否正确或联系服务器管理员获得帮助。Err:"
        std::string errMsg = utils::WStringToString(localization::Loader::getResource().ui_setup_serveraddr_server_version_failed)
                + getCoreServerVersionInfo.message();
        status_messsage_->SetText(utils::StringToWstring(errMsg), false);
        return;
    }

    // L"发送请求获取应用列表检测服务器地址"
    status_messsage_->SetText(localization::Loader::getResource().ui_setup_serveraddr_detect_applist, false);

    lark::GetAppliList getAppliList;
    getAppliList.Send();
    if (getAppliList.is_success()) {
        // L"当前地址检测成功。点击返回按钮可返回首页。"
        status_messsage_->SetText(localization::Loader::getResource().ui_setup_serveraddr_detect_applist_success, false);
    } else {
        // "应用列表请求失败，请检查地址是否正确或联系服务器管理员获得帮助。Err:"
        std::string errMsg = utils::WStringToString(localization::Loader::getResource().ui_setup_serveraddr_detect_applist_failed)
                + getAppliList.message();
        status_messsage_->SetText(utils::StringToWstring(errMsg), false);
    }
    // reset status
    is_detecting_ = false;
}

// GetRegion callback
void SetupServerAddr::OnSuccess(bool detectRttFlag, const std::vector<lark::RegionInfo> &regionInfo) {
    LOGV("On region list %ld", regionInfo.size());

    {
        std::lock_guard<std::mutex> lock(region_mutex_);
        results_.clear();
        current_test_ = 0;
        selected_result_ = { 0, false };
        need_update_region_ = true;
    }

    // server close detect
    if (!detectRttFlag) {
        return;
    }

    for (const auto& region: regionInfo) {
        LOGV("find region name %s ip %s port %s publicIp %s", region.regionName.c_str(), region.serverIp.c_str(), region.serverPort.c_str(), region.publicIp.c_str());
         results_.push_back({0, false, region});
    }
    // connect
    if (region_rtt_test_ && !regionInfo.empty()) {
        const auto region = regionInfo.front();
        if (!region.publicIp.empty()) {
            int port;
            std::istringstream (region.serverPort) >> port;
            region_rtt_test_->Connect(region.publicIp, port,  "", region.regionId);
            LOGV("region test [%s] rrt with public %s:%d", region.regionName.c_str(), region.publicIp.c_str(), port);
        } else if (!region.preferPublicIp.empty()) {
            std::string ip = lark::XRClient::GetServerHost();
            int port = lark::XRClient::GetServerPort();
            region_rtt_test_->Connect(ip, port,
                                      "/websocket/" + region.serverIp + "/" + region.serverPort,
                                       region.regionId);
            LOGV("region test [%s] with preferPublicIp %s:%d %s:%s id %s", region.regionName.c_str(), ip.c_str(), port,
                 region.serverIp.c_str(), region.serverPort.c_str(), region.regionId.c_str());
        } else {
            int port;
            std::istringstream (region.serverPort) >> port;
            region_rtt_test_->Connect(region.serverIp, port, "", region.regionId);
            LOGV("regin test [%s] with %s:%d id %s", region.regionName.c_str(), region.serverIp.c_str(), port, region.regionId.c_str());
        }
    }
}

void SetupServerAddr::OnFailed(const std::string &err) {
    LOGW("Fetch Region list failed %s", err.c_str());
    {
        std::lock_guard<std::mutex> lock(region_mutex_);
        results_.clear();
        current_test_ = 0;
        selected_result_ = { 0, false };
        need_update_region_ = true;
    }
}

// region_rtt_test callback
void SetupServerAddr::OnRegionRttTestResult(uint64_t rtt, const std::string& regionId) {
    std::lock_guard<std::mutex> lock(region_mutex_);
    LOGV("get region rtttest result %ld id %s total regionsize %ld", rtt, regionId.c_str(), results_.size());

    if (results_.empty()) {
        return;
    }

    if (current_test_ < results_.size() && results_[current_test_].info.regionId == regionId) {
        results_[current_test_].rtt = rtt;

        // finish
        if (current_test_ == results_.size() - 1) {
            for(const auto& res : results_) {
                LOGV("got region %s rtt %ld", res.info.regionName.c_str(), res.rtt);
                if (res.rtt != 0 && (selected_result_.rtt == 0 || res.rtt < selected_result_.rtt)) {
                    selected_result_ = res;
                    selected_result_.selected = true;
                }
            }
            for(auto& res: results_) {
                if (res.info.regionId == selected_result_.info.regionId) {
                    res.selected = true;
                    LOGV("selected region %s rtt %ld", res.info.regionName.c_str(), res.rtt);
                }
            }
            if (callback_) {
                callback_->OnUpdateRegion(selected_result_);
            }
            // need update ui
            need_update_region_ = true;
        } else {
            // next
            if (region_rtt_test_) {
                LOGV("get next rtt info total %ld current %d", results_.size(), current_test_);
                current_test_++;
                auto region = results_[current_test_].info;
                if (!region.publicIp.empty()) {
                    int port;
                    std::istringstream (region.serverPort) >> port;
                    region_rtt_test_->Connect(region.publicIp, port, "", region.regionId);
                    LOGV("region test [%s] rrt with public %s:%d", region.regionName.c_str(), region.publicIp.c_str(), port);
                } else if (!region.preferPublicIp.empty()) {
                    std::string ip = lark::XRClient::GetServerHost();
                    int port = lark::XRClient::GetServerPort();
                    region_rtt_test_->Connect(ip, port,
                                              "/websocket/" + region.serverIp + "/" + region.serverPort,
                                              region.regionId);
                    LOGV("region test [%s] with preferPublicIp %s:%d %s:%s id %s", region.regionName.c_str(), ip.c_str(), port,
                         region.serverIp.c_str(), region.serverPort.c_str(), region.regionId.c_str());
                } else {
                    int port;
                    std::istringstream (region.serverPort) >> port;
                    region_rtt_test_->Connect(region.serverIp, port, "", region.regionId);
                    LOGV("regin test [%s] with %s:%d id %s", region.regionName.c_str(), region.serverIp.c_str(), port, region.regionId.c_str());
                }
            }
        }
    }
}

void SetupServerAddr::OnRegionRttTestError(const std::string &err, const std::string& regionId) {
    LOGW("Fetch Region Test Rtt failed %s", err.c_str());
    std::lock_guard<std::mutex> lock(region_mutex_);
    if (results_.empty()) {
        return;
    }
    if (current_test_ < results_.size()) {
        // TODO set to max when rtt check failed.
        results_[current_test_].rtt = 0;

        // finish
        if (current_test_ == results_.size() - 1) {
            for(const auto& res : results_) {
                LOGV("got region %s rtt %ld", res.info.regionName.c_str(), res.rtt);
                if (res.rtt != 0 && (selected_result_.rtt == 0 || res.rtt < selected_result_.rtt)) {
                    selected_result_ = res;
                    selected_result_.selected = true;
                }
            }
            for(auto& res: results_) {
                if (res.info.regionId == selected_result_.info.regionId) {
                    res.selected = true;
                    LOGV("selected region %s rtt %ld", res.info.regionName.c_str(), res.rtt);
                }
            }
            if (callback_) {
                callback_->OnUpdateRegion(selected_result_);
            }
            // need update ui
            need_update_region_ = true;
        } else {
            // next
            if (region_rtt_test_) {
                LOGV("get next rtt info total %ld current %d", results_.size(), current_test_);
                current_test_++;
                auto region = results_[current_test_].info;
                if (!region.publicIp.empty()) {
                    int port;
                    std::istringstream (region.serverPort) >> port;
                    region_rtt_test_->Connect(region.publicIp, port, "", region.regionId);
                } else if (!region.preferPublicIp.empty()) {
                    std::string ip = lark::XRClient::GetServerHost();
                    int port = lark::XRClient::GetServerPort();
                    region_rtt_test_->Connect(ip, port,
                                              "/websocket/" + region.serverIp + "/" + region.serverPort,
                                              region.regionId);
                } else {
                    int port;
                    std::istringstream (region.serverPort) >> port;
                    region_rtt_test_->Connect(region.serverIp, port, "", region.regionId);
                }
            }
        }
    }
}

void SetupServerAddr::OnRegionRttTestClose(const std::string &err, const std::string &regionId) {
    LOGW("Fetch Region Test Rtt Closed %s", err.c_str());
}

void SetupServerAddr::Update() {
    Object::Update();
    if (need_update_region_) {
        region_title_->set_active(!results_.empty());
        for (int i = 0; i < region_list_.size(); i++) {
            if (i < results_.size()) {
                std::ostringstream ss;
                ss << results_[i].info.regionName << " "
                   << results_[i].rtt << " ms"
                   << (results_[i].selected ? " current " : "");
                region_list_[i]->SetText(utils::StringToWstring(ss.str()));
                region_list_[i]->set_active(true);
                region_list_[i]->SetAABBPositon(glm::vec2(-0.6F, 0.1f - i * 0.15f - region_list_[i]->GetSize().y / 2));
                region_list_[i]->SetAABBSize(region_list_[i]->GetSize());
            } else {
                region_list_[i]->set_active(false);
            }
        }
        if (callback_) {
            callback_->OnUpdateRegion(selected_result_);
        }
        need_update_region_ = false;
    }
}

//
// input buttons
//
SetupServerAddr::InputButton::InputButton(const std::wstring &title):Button(title) {
    Init();
}

void SetupServerAddr::InputButton::Init() {
    Base::set_size(glm::vec2(INPUT_BOX_WIDTH, INPUT_BOX_HEIGHT));

    line_ = std::make_shared<ColorBox>();
    line_->set_size(glm::vec2(Base::size_.x, 0.01F));
    line_->set_color(INPUT_LINE_COLOR);
    AddChild(line_);

    title_->SetFontSize(26);
    title_->set_position(0, 0.025, 0);
    AddChild(title_);

    SetAABBSize(Base::size_);
}

void SetupServerAddr::InputButton::clearState() {
    picked_ = false;
    setSelected(false);
}

glm::vec2 SetupServerAddr::InputButton::GetSize() {
    return Base::size_;
}

void SetupServerAddr::InputButton::HandleInput(glm::vec2 *point, int pointCount) {
    Button::HandleInput(point, pointCount);
}

void SetupServerAddr::InputButton::setSelected(bool selected) {
    line_->set_color(selected ? INPUT_LINE_COLOR_ACTIVE : INPUT_LINE_COLOR);
}