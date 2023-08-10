//
// Created by fcx on 2020/7/28.
//

#ifndef CLOUDLARKXR_GET_TASK_H
#define CLOUDLARKXR_GET_TASK_H

#include <iostream>
#include "../common.h"

namespace lark
{
class GetTaskImp;
class LARK_COMMON_API GetTask {
public:
    GetTask();
    ~GetTask();
    // send sync
    void Send(const std::string& taskId);
    // send async
    void SendAsync(const std::string& taskId);

    bool is_task_ready() const;
    bool is_ar_alpha_mask() const;
    bool is_audio_input() const;
    int http_code() const;
    int api_result_code() const;
    std::string message() const;
    bool is_success() const;
private:
    GetTaskImp* get_task_imp_;
};
}

#endif //CLOUDLARKXR_GET_TASK_H
