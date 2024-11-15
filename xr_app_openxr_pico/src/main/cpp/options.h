// Copyright (c) 2017-2020 The Khronos Group Inc
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

struct Options {
    std::string GraphicsPlugin;

    std::string FormFactor{"Hmd"};

    std::string ViewConfiguration{"Stereo"};

    std::string EnvironmentBlendMode{"Opaque"};

    std::string LocalSpace{"Local"};
    std::string ViewSpace{"View"};
    std::string StateSpace{"Stage"};
};
