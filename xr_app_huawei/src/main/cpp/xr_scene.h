//
// Created by fcx@pingxingyung.com on 2022/11/14.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_XR_SCENE_H
#define LARKXR_XR_SCENE_H

#include <vector>
#include <object.h>
#include "input_state.h"

namespace hxr {
class XrScene {
public:
    XrScene();
    ~XrScene();

    virtual void InitGL();
    virtual void Render(lark::Object::Eye eye, const XrCompositionLayerProjectionView& layerView);
    virtual void HandleInput(const InputState& input_state);
    virtual void ReleaseGL();
protected:
    void AddObject(std::shared_ptr<lark::Object> object);
    void RemoveObject(std::shared_ptr<lark::Object> object);
    void ClearObject();
    // objects.
    std::vector<std::shared_ptr<lark::Object>> objects_{};
};
}
#endif //LARKXR_XR_SCENE_H
