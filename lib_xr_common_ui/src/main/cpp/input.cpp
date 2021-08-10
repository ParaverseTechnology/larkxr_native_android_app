//
// Created by fcx on 2020/5/6.
//

#include "input.h"

// default ray cast type.
Input::RayCastType Input::s_currrent_ray_ = Input::RayCast_Right;
Input::InputState Input::s_input_state_[Input::RayCast_Count] = {
        {false, false, false},
        {false, false, false}
};

void Input::ResetInput() {
    s_currrent_ray_ = RayCast_Right;
    for(auto & state : s_input_state_) {
        state.enterShortPressed = false;
        state.triggerShortPressed = false;
        state.backShortPressed = false;
        state.backButtonDown = false;
        state.enterButtonDown = false;
        state.triggerButtonDown = false;
    }
}
