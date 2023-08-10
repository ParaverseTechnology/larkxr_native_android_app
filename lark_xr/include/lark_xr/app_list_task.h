//
// Created by fcx on 2020/4/14.
//

#ifndef CLOUDLARKXR_APP_LIST_TASK_H
#define CLOUDLARKXR_APP_LIST_TASK_H

#include "lark_xr/lark_xr.h"
#include "request/get_appli_list.h"
#include "request/get_vrclient_run_mode.h"
#include <vector>

namespace lark {
/**
 * 获取应用列表和运行模式。
 * 内部 task 将定时调用，并在回调函数中更新。
 * 回调函数不在 opengl 线程调用，需要相关处理。
 */
class AppListTaskImp;
class LARK_XR_API AppListTask {
public:
    class AppListTaskListener {
    public:
        // 获取到应用列表
        virtual void OnAppListInfo(const std::vector<AppliInfo>& appliInfo) = 0;
        // 带分页数据应用列表，与 OnAppListInfo 同时回调，可以只关注一个
        virtual void OnAppListPageInfo(const AppliPageInfo& appliPageInfo) = 0;
        // 调用接口的错误信息
        virtual void OnFailed(const std::string& msg) = 0;
        // 运行模式
        // 当运行模式为教师集中控制时，应隐藏实际应用列表，从后台启动应用。
        virtual void OnRunMode(GetVrClientRunMode::ClientRunMode runMode) = 0;
    };

    static GetVrClientRunMode::ClientRunMode run_mode();

    /**
     * @param callback 回调函数
     * @param autoStart 是否自动开始
     */
    AppListTask(AppListTaskListener* callback, bool autoStart = false);
    ~AppListTask();

    // 开启任务循环
    void Start();
    // 关闭任务循环
    void Stop();

    void SetPage(int page);
    void SetPageSize(int pageSize);

    bool start();
private:
    AppListTaskImp* app_list_task_imp_;
};
}
#endif //CLOUDLARKXR_APP_LIST_TASK_IMP_H
