//
// Created by fcx on 2020/5/6.
//

#ifndef CLOUDLARKXR_INPUT_H
#define CLOUDLARKXR_INPUT_H


class Input {
public:
    enum RayCastType {
        RayCast_left  = 0,
        RayCast_Right = 1,
        RayCast_Hmd   = 2,
        RayCast_Count = 3,
    };
    struct InputState {
        bool backShortPressed;
        bool enterShortPressed;
        bool triggerShortPressed;
        bool backButtonDown;
        bool enterButtonDown;
        bool triggerButtonDown;
    };
    // statics
    static inline RayCastType GetCurrentRayCastType() { return s_currrent_ray_; }
    static inline InputState GetCurrentInputState() { return s_input_state_[s_currrent_ray_]; }
    static inline bool IsInputEnter() {
        return s_input_state_[s_currrent_ray_].enterShortPressed || s_input_state_[s_currrent_ray_].triggerShortPressed;
    }
    static void ResetInput();
    // staic faild.
    static inline void SetCurrentRayCastType(RayCastType type) { s_currrent_ray_ = type; }
    static inline InputState* GetInputState() { return s_input_state_; };
    static inline int GetInputStateCount() { return RayCast_Count; };

private:
    // static failed.
    static RayCastType s_currrent_ray_;
    static InputState  s_input_state_[RayCast_Count];
};


#endif //CLOUDLARKXR_INPUT_H
