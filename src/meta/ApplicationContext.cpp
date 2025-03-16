#include <SDL3/SDL.h>
#include <memory>
#include <iostream>

#include <scene/scene.h>
#include <meta/ApplicationContext.h>

ViewportState::ViewportState(glm::fvec2* bounds, SDL_WindowFlags settings, SDL_Window* window) {
    m_bounds = bounds;
    m_window = window;
    m_settings = settings;
}
ViewportState::~ViewportState() {
    SDL_DestroyWindow(m_window);
}
SDL_Window& ViewportState::window() const noexcept { return *m_window; }
glm::fvec2* ViewportState::bounds() const noexcept { return m_bounds; }

FrameData::FrameData(SDL_Renderer* renderer, double gameStartTime) {
    this->m_number = 0;
    this->m_fps = 0.0f;
    this->m_deltaT = 0.0;
    this->m_gameStartTime = gameStartTime;
    this->m_lastFrameTime = gameStartTime;
    this->m_renderer = renderer;
}
FrameData::~FrameData() {
    SDL_DestroyRenderer(m_renderer);
}
SDL_Renderer& FrameData::renderer() const noexcept { return *m_renderer; }
void FrameData::onDrawCallRisingEdge() {
    this->m_number++;

    ms now = SDL_GetTicks();
    ms delta = now - this->m_lastFrameTime;
    
    //Preventing division by zero
    delta = delta == 0 ? 1 : delta;

    this->m_deltaT = this->m_expectedTimePerFrame / delta;
    this->m_fps = 1.0f / this->m_deltaT;
    this->m_lastFrameTime = now;
}
float FrameData::deltaT() const noexcept { return m_deltaT; }

ApplicationContext::~ApplicationContext() { }

ViewportState& ApplicationContext::viewport() const noexcept { return *m_viewport; }
FrameData& ApplicationContext::frames() const noexcept { return *m_frames; }
InputManager& ApplicationContext::input() const noexcept { return *m_input; }
IScene& ApplicationContext::currentScene() const noexcept { return *m_currentScene; }
void ApplicationContext::changeScene(IScene* scene) noexcept { 
    if (m_currentScene != nullptr) {
        m_currentScene->onTearDown();
    }
    m_currentScene.reset(scene); 
}

void ApplicationContext::onDrawCallRisingEdge() const noexcept {
    m_frames->onDrawCallRisingEdge();
    m_currentScene->onDrawCallRisingEdge();
}

void ApplicationContext::onDraw() noexcept {
    m_currentScene->draw(shared_from_this());
}

void ApplicationContext::onTick() {
    m_input->onTickRisingEdge();
    m_currentScene->tick(shared_from_this());
}

