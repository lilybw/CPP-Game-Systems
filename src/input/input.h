#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include <deque>
#include <map>
#include <glm/vec2.hpp>

using OnInputCallbackId = int;
/** Milliseconds */
using ms = uint64_t;
typedef struct {
    SDL_Keycode code;
    ms timestampOfInsertion;
} TimeStampedInputEvent;

enum class InputState {
    Down,
    Held,
    Up
};

class InputManager {
public:
    InputManager();
    ~InputManager();

    void onEvent(SDL_Event* event);
    void onTickRisingEdge();
    /** Static check for key or mouse-button state */
    bool isDown(SDL_Scancode code) const;
    bool isHeld(SDL_Scancode code);
    bool hasBeenDownFor(SDL_Scancode code, ms duration);

private:
    std::map<SDL_Scancode, InputState> m_keyStates;
    std::map<SDL_Scancode, ms> m_keyTimeOfDown;
    /** All inputs for the last <m_trackedDuration> seconds, in order, most recent first */
    std::deque<TimeStampedInputEvent> m_eventTracker;
    const ms m_trackedDuration = 10000;
    /** After how many milliseconds should a key be concidered "held" rather than "down" */ 
    const ms m_heldStateThreshold = 200;
    
    uint32_t m_nextCallbackId = 0;
    glm::vec2 m_mousePosition;
    /** Not normalized */
    glm::vec2 m_mouseDirection;

    void trackEvent(SDL_Event* event, ms now);
};
