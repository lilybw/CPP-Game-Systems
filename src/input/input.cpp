#include <SDL3/SDL.h>
#include <functional>
#include <map>
#include <deque>
#include <glm/vec2.hpp>

#include <input/input.h>

InputManager::InputManager() {
    m_keyStates = std::map<SDL_Scancode, InputState>();
    m_keyTimeOfDown = std::map<SDL_Scancode, ms>();
    m_mousePosition = glm::vec2(0.0f, 0.0f);
    m_mouseDirection = glm::vec2(0.0f, 0.0f);
}
InputManager::~InputManager() { }

bool InputManager::isDown(SDL_Scancode code) const {
    auto state = m_keyStates.find(code);
    return state != m_keyStates.end() && state->second == InputState::Down;
}

bool InputManager::isHeld(SDL_Scancode code) {
    auto stateIter = m_keyStates.find(code);
    //No check for Down here, as we reevaluate Down below
    if (stateIter == m_keyStates.end() || stateIter->second == InputState::Up) {
        return false;
    } 
    
    if (stateIter->second == InputState::Held) {
        return true;
    }

    // Then it must be Down, and if so, reevalute threshold
    ms now = SDL_GetTicks();

    auto timeOfDownIter = m_keyTimeOfDown.find(code);
    if (timeOfDownIter == m_keyTimeOfDown.end() || stateIter->second != InputState::Down) {
        m_keyTimeOfDown[code] = now; //if somehow...
        return false;
    }

    if (this->hasBeenDownFor(code, m_heldStateThreshold)) {
        m_keyStates[code] = InputState::Held;
        return true;
    }

    return false;
}

bool InputManager::hasBeenDownFor(SDL_Scancode code, ms duration) {
    auto state = m_keyStates.find(code);
    if (state == m_keyStates.end() || state->second != InputState::Down) {
        return false;
    }

    auto timeOfDown = m_keyTimeOfDown.find(code);
    if (timeOfDown == m_keyTimeOfDown.end()) {
        return false;
    }

    return SDL_GetTicks() - timeOfDown->second > duration;
}

void InputManager::onEvent(SDL_Event* event) {
    ms now = SDL_GetTicks();
    auto timeOfDown = m_keyTimeOfDown.end();

    switch (event->type) {
    //KEYBOARD
        case SDL_EVENT_KEY_DOWN:
            m_keyStates[event->key.scancode] = InputState::Down;
            timeOfDown = m_keyTimeOfDown.find(event->key.scancode);
            if (timeOfDown != m_keyTimeOfDown.end()) {
                m_keyTimeOfDown[event->key.scancode] = now;
            }
            break;
        case SDL_EVENT_KEY_UP:
            m_keyStates[event->key.scancode] = InputState::Up;
            timeOfDown = m_keyTimeOfDown.find(event->key.scancode);
            if (timeOfDown != m_keyTimeOfDown.end()) {
                m_keyTimeOfDown.erase(event->key.scancode);
            }
            break;
    
    //MOUSE
        case SDL_EVENT_MOUSE_MOTION:
            glm::vec2 prevPosition = glm::vec2(m_mousePosition);
            glm::vec2 newPosition = glm::vec2(event->motion.x, event->motion.y);
            m_mouseDirection = newPosition - prevPosition;
            m_mousePosition = newPosition;
            //TODO
            break;
    }

    this->trackEvent(event, now);
}

void InputManager::trackEvent(SDL_Event* event, ms now) {
    TimeStampedInputEvent newEvent = {event->key.key, now};

    // Add the new event to the front of the deque
    m_eventTracker.push_front(newEvent);
    // Clean up happens on Tick
}

void InputManager::onTickRisingEdge() {
    ms now = SDL_GetTicks();

    //Look through all entries that isDown
    //If its surpassed m_heldStateThreshold, change it to isHeld
    for (auto& [code, state] : m_keyStates) {
        if (state == InputState::Down) {
            auto timeOfDown = m_keyTimeOfDown.find(code);
            if (timeOfDown != m_keyTimeOfDown.end() && now - timeOfDown->second > m_heldStateThreshold) {
                m_keyStates[code] = InputState::Held;
            }
        }
    }

    // Cleanup old events
    while (!m_eventTracker.empty() && (now - m_eventTracker.back().timestampOfInsertion > m_trackedDuration)) {
        m_eventTracker.pop_back();
    }
}
