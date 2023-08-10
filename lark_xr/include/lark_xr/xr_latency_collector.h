//
// Created by fcx on 2020/3/9.
//

#ifndef CLOUDLARKXR_XR_LATENCY_COLLECTOR_H
#define CLOUDLARKXR_XR_LATENCY_COLLECTOR_H

#include <memory>
#include <vector>
#include "lark_xr/lark_xr.h"

namespace lark {
/**
 * 延时收集类。大部分延时手机在 sdk 内部完成。但最终提交和渲染应在应用实际渲染时调用。
 * Rendered2 和 Submit 两个延时收集点应在应用中调用
 */
class LARK_XR_API XRLatencyCollector {
public:
    static XRLatencyCollector &Instance();

    uint64_t GetLatency(uint32_t i, uint32_t j);
    uint64_t packets_lost_total();
    uint64_t packets_lost_in_second();
    uint64_t fec_failure_total();
    uint64_t fec_failure_in_second();
    uint32_t frames_in_second();

    void PacketLoss(int64_t lost);
    void FecFailure();

    void Tracking(uint64_t frameIndex);
    void EstimatedSent(uint64_t frameIndex, uint64_t offset);
    void ReceivedFirst(uint64_t frameIndex);
    void ReceivedLast(uint64_t frameIndex);
    void DecoderInput(uint64_t frameIndex);
    void DecoderOutput(uint64_t frameIndex);
    void Rendered1(uint64_t frameIndex);
    // opengl 渲染时调用，一般为渲染到 framebuffer 时。
    void Rendered2(uint64_t frameIndex);
    // 提交给 vr 头盔sdk 时调用。
    void Submit(uint64_t frameIndex, float blackDegree);

    void ResetAll();
private:
    static XRLatencyCollector instance_;
    XRLatencyCollector();
};
}

#endif //CLOUDLARKXR_XR_LATENCY_COLLECTOR_H
