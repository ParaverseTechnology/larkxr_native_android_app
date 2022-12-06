//
// Created by fcx@pingixngyun.com on 2019/11/15.
//

#include <jni.h>
#include "localization.h"
//#pragma pack(4)
// __declspec(align(4));

namespace localization {
    const LocalResource r_en = {
            ui_home_title:L"Application List",
            ui_home_device_name:L"Device Name",
            ui_home_mac_address:L"Mac Address",
            ui_home_client_id: L"Client Id",
            ui_home_app_version:L"App Version ",
            ui_home_support_server_version:L"Min ServerVersion",
            ui_home_setup_serveraddr:L"Setup Server Address",
            ui_home_advance_setting:L"Advanced Setting",
            ui_home_quick_3d_ui: L"Switch To 2D Applist",
            ui_home_setup:L"Setting",
            ui_home_back:L"Back",
            ui_home_status_normal:L"Normal Setup",
            ui_home_status_device:L"VR Device",
            ui_home_reset:L"Restore",
            ui_home_run_mode_teacher:L"Centralized management mode. Waiting for unified launch of the app.",
            ui_home_trigger_hit:L"Please use the handle Trackpad to confirm..",
            ui_home_trigger_hit_ax: L"Please use the handle A/X to confirm..",
            ui_home_empty_list: L"Nothing on the server.",

            ui_advence_predit_title:L"Prediction",
            ui_advence_nolosetup_switch:L"Nolo Setup",
            ui_advence_nolosetup_switch_normal:L"Normal Setup",
            ui_advence_predit_open:L"Enable",
            ui_advence_predit_close:L"Disable",
            ui_advance_scale_clipping_plane_boundary:L"ScaleClippingPlaneBoundary",
            ui_advence_set_nolo_hmd_trackcenter_z:L"Nolo Head Pose Center Z",
            ui_advence_set_nolo_hmd_trackcenter_y:L"Nolo Head Pose Center Y",
            ui_advence_set_nolo_predit:L"Nolo PredictionTime",
            ui_advence_set_nolo_use_hmd_position:L"Nolo HMD Position Offset",
            ui_setup_coderate:L"Coderate Setting",
            ui_setup_maincontroller:L"Main Controller",
            ui_setup_pitch:L"Pitch Setting",
            ui_setup_inited_height: L"Initial Height Setting",
            ui_setup_safe_zoon:L"Security Zone Setting",
            ui_setup_safe_vibrator:L"Virbrator",
            ui_setup_safe_zoon_auto:L"Auto",
            ui_setup_safe_zoon_close:L"Close",
            ui_setup_high_frame_rate:L"Frame Rate Mode",
            ui_setup_high_frame_rate_high:L"High",
            ui_setup_high_frame_rate_normal:L"Normal",

            ui_setup_serveraddr_current:L"Current IP",
            ui_setup_serveraddr_hand:L"Manually set the ip address",
            ui_setup_serveraddr_auto:L"Automatic Search Server IP",
            ui_setup_serveraddr_auto_mode:L"Automatic Search Mode",
            ui_setup_serveraddr_hand_mode:L"Handed Mode",
            ui_setup_serveraddr_waring_empey:L"Server address cannot be empty",
            ui_setup_serveraddr_auto_mode_start:L"Automatically Find Server Address Mode Started",
            ui_setup_serveraddr_ip: L"Server IP",
            ui_setup_serveraddr_port: L"Server Port",
            ui_setup_serveraddr_input_ip: L"Input Ip",
            ui_setup_serveraddr_input_port: L"Input Port",
            ui_setup_serveraddr_currentmode: L"Current Mode",
            ui_setup_serveraddr_submit: L"Submit Current",
            ui_setup_serveraddr_autofind: L"Autofind",
            ui_setup_serveraddr_region_title: L"Region List",

            ui_setup_serveraddr_request: L"Please setup server address",
            ui_setup_serveraddr_check_server_version: L"Checking server version",
            ui_setup_serveraddr_detecting: L"Detecting...",
            ui_setup_serveraddr_server_version_missmatch: L"Server version missmatch",
            ui_setup_serveraddr_server_version_success: L"Check server version success",
            ui_setup_serveraddr_server_version_failed: L"Check server version failed. Err:",
            ui_setup_serveraddr_detect_applist: L"Check applist",
            ui_setup_serveraddr_detect_applist_success: L"Check applist success",
            ui_setup_serveraddr_detect_applist_failed: L"Check applist failed",

            ui_setup_yes: L"YES",
            ui_setup_no: L"NO",

            ui_setup_advance: L"Advance Setup",
            ui_setup_normal: L"Normal Setup",
            ui_setup_advance_ffr_title: L"Fixed foveated rendering?",
            ui_setup_advance_report_fec_title: L"Report fec fail?",
            ui_setup_advance_use_h265_title: L"Enable H265?",
            ui_setup_advance_haptics_feedback_title: L"Enable haptics feedback?",
            ui_setup_advance_throttled_udp: L"Throttled udp",
            ui_setup_advance_kcp_title: L"Transport",
            ui_setup_advance_kcp: L"KCP",
            ui_setup_advance_use_10bit_encode: L"Enable 10bit encode",
            ui_setup_advance_quick_config: L"Quick Config",
            ui_setup_advance_quick_config_manual: L"Manual",
            ui_setup_advance_quick_config_fast: L"Fast",
            ui_setup_advance_quick_config_normal: L"Normal",
            ui_setup_advance_quick_config_extreme: L"Extreme",

            ui_setup_resolution_title: L"Resolution",

            ui_setup_skybox_title: L"SkyBox",
            ui_setup_skybox_0: L"Asset Night",
            ui_setup_skybox_1: L"Asset Star",

            ui_setup_skybox_xrspace: L"XrSpace",
            ui_setup_skybox_xrspace_local: L"XrSpace Local(Fixed room height)",
            ui_setup_skybox_xrspace_app: L"XrSpace App(Room height with system setup)",

            ui_keyboard_number:L"NumberKeyboard",

            ui_menu_view_title: L"Quit CloudApp Now?",
            ui_menu_view_submit: L"Quit",
            ui_menu_view_cancle: L"Containue",

            ui_loading_tips_3d_quest: L"Press trigger and short press B/Y back to applist.",
            ui_loading_tips_3d: L"Press trigger and short press APP button back to applist.",
            ui_loading_tips_2d: L"Click HOME button and use QUIT back to applist.",
    };
    const LocalResource r_zh = {
            ui_home_title: L"应用列表",
            ui_home_device_name: L"设备名称",
            ui_home_mac_address: L"Mac 地址",
            ui_home_client_id: L"客户端 Id",
            ui_home_app_version: L"App 版本号 ",
            ui_home_support_server_version: L"最小服务端版本",
            ui_home_setup_serveraddr: L"设置服务器地址",
            ui_home_advance_setting: L"高级设置",
            ui_home_quick_3d_ui: L"切换为 2D 应用列表",
            ui_home_setup: L"设置",
            ui_home_back: L"返回",
            ui_home_status_normal: L"通用设置",
            ui_home_status_device: L"一体机设置",
            ui_home_reset: L"恢复默认设置",
            ui_home_run_mode_teacher: L"集中管控模式。等待统一启动应用。",
            ui_home_trigger_hit: L"请使用手柄 Trackpad 键确认。",
            ui_home_trigger_hit_ax: L"使用手柄A或X键可进入应用",
            ui_home_empty_list: L"当前服务器上空空如也",

            ui_advence_predit_title:L"姿态预测设置",
            ui_advence_nolosetup_switch:L"Nolo设置",
            ui_advence_nolosetup_switch_normal:L"通用设置",
            ui_advence_predit_open:L"开启",
            ui_advence_predit_close:L"关闭",
            ui_advance_scale_clipping_plane_boundary:L"剪裁平面边界缩放",
            ui_advence_set_nolo_hmd_trackcenter_z:L"Nolo 头部定位器设置 Z",
            ui_advence_set_nolo_hmd_trackcenter_y:L"Nolo  Y",
            ui_advence_set_nolo_predit:L"Nolo 惯导预测时间头部定位器设置",
            ui_advence_set_nolo_use_hmd_position:L"Nolo 头部定位偏移",
            ui_setup_coderate:L"码率设置",
            ui_setup_maincontroller:L"主控手设置",
            ui_setup_pitch:L"瞳距设置",
            ui_setup_inited_height:L"初始高度设置",
            ui_setup_safe_zoon:L"安全区域设置",
            ui_setup_safe_zoon_auto:L"自动",
            ui_setup_safe_zoon_close:L"关闭",
            ui_setup_safe_vibrator:L"是否震动手柄",
            ui_setup_high_frame_rate:L"帧率模式",
            ui_setup_high_frame_rate_high:L"最高",
            ui_setup_high_frame_rate_normal:L"普通",

            ui_setup_serveraddr_current:L"当前服务器地址",
            ui_setup_serveraddr_hand:L"设置服务器地址",
            ui_setup_serveraddr_auto:L"自动搜索服务器",
            ui_setup_serveraddr_auto_mode:L"自动模式",
            ui_setup_serveraddr_hand_mode:L"手动模式",
            ui_setup_serveraddr_waring_empey:L"服务器地址不能为空",
            ui_setup_serveraddr_auto_mode_start:L"自动寻找地址模式开始",
            ui_setup_serveraddr_ip: L"服务器IP",
            ui_setup_serveraddr_port: L"服务器端口",
            ui_setup_serveraddr_input_ip: L"输入IP",
            ui_setup_serveraddr_input_port: L"输入端口号",
            ui_setup_serveraddr_currentmode: L"当前模式",
            ui_setup_serveraddr_submit: L"确认当前地址",
            ui_setup_serveraddr_autofind: L"自动查找",
            ui_setup_serveraddr_region_title: L"区域列表",

            ui_setup_serveraddr_request: L"请设置服务器地址",
            ui_setup_serveraddr_check_server_version: L"检测服务器版本号",
            ui_setup_serveraddr_detecting: L"正在检测中",
            ui_setup_serveraddr_server_version_missmatch: L"检测到与服务器版本号不匹配，服务器版本\"",
            ui_setup_serveraddr_server_version_success: L"检测服务器版本号成功",
            ui_setup_serveraddr_server_version_failed: L"检测服务器版本号请求失败，与服务器版本可能不匹配, 请检查地址是否正确或联系服务器管理员获得帮助。Err:",
            ui_setup_serveraddr_detect_applist: L"发送请求获取应用列表检测服务器地址",
            ui_setup_serveraddr_detect_applist_success: L"当前地址检测成功。点击返回按钮可返回首页。",
            ui_setup_serveraddr_detect_applist_failed: L"应用列表请求失败，请检查地址是否正确或联系服务器管理员获得帮助。Err:",

            ui_setup_yes: L"是",
            ui_setup_no: L"否",

            ui_setup_advance: L"高级设置",
            ui_setup_normal: L"普通设置",
            ui_setup_advance_ffr_title: L"是否开启固定注视点渲染？",
            ui_setup_advance_report_fec_title: L"开启FEC报告？",
            ui_setup_advance_use_h265_title: L"是否使用H265协议？",
            ui_setup_advance_haptics_feedback_title: L"是否开启手柄震动？",
            ui_setup_advance_throttled_udp: L"调速 UDP",
            ui_setup_advance_kcp_title: L"传输类型",
            ui_setup_advance_kcp: L" 增强 UDP",
            ui_setup_advance_use_10bit_encode: L"开启10Bit编码？",
            ui_setup_advance_quick_config: L"快速配置",
            ui_setup_advance_quick_config_manual: L"手动",
            ui_setup_advance_quick_config_fast: L"流畅",
            ui_setup_advance_quick_config_normal: L"标准",
            ui_setup_advance_quick_config_extreme: L"超清",

            ui_setup_resolution_title: L"分辨率",

            ui_setup_skybox_title: L"天空盒",
            ui_setup_skybox_0: L"Asset-Night",
            ui_setup_skybox_1: L"Asset-Star",

            ui_setup_skybox_xrspace: L"XrSpace",
            ui_setup_skybox_xrspace_local: L"本地-固定房间高度",
            ui_setup_skybox_xrspace_app: L"系统-系统设置房间高度",

            ui_keyboard_number:L"数字键盘",

            ui_menu_view_title: L"确定退出当前云端应用？",
            ui_menu_view_submit: L"退出",
            ui_menu_view_cancle: L"继续",

            ui_loading_tips_3d_quest: L"按住手柄扳机键并短按B或Y键可退出云端应用返回列表",
            ui_loading_tips_3d: L"按住手柄扳机键并短按APP键可退出云端应用返回列表",
            ui_loading_tips_2d: L"按手柄 Home 键选择退出返回应用列表",
    };

    // localization.
    LocalResource Loader::localResource = r_zh;
    void Loader::load(bool isEn) {
        if (isEn) {
            localResource = r_en;
        } else {
            localResource = r_zh;
        }
    }

    const LocalResource& Loader::getResource() {
        return localResource;
    }
}

//#pragma pack()