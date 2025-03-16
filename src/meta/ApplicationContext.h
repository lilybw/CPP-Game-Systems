#pragma once

#include <memory>
#include <SDL3/SDL.h>
#include <input/input.h>
#include <glm/glm.hpp>

#include <scene/sceneManager.h> 
#include <meta/processing.h>

class ViewportState {
public:
    ViewportState(glm::fvec2* bounds, SDL_WindowFlags settings, SDL_Window* window);
    ~ViewportState();

    SDL_Window& window() const noexcept;
    /** Display bounds, may not actually be viewport dimensions */
    glm::fvec2* bounds() const noexcept;

private:
    glm::fvec2* m_bounds;
    SDL_Window* m_window;
    SDL_WindowFlags m_settings;
};

using ms = uint64_t;
class FrameData {
public:
    FrameData(SDL_Renderer* renderer, double gameStartTime);
    ~FrameData();

    void onDrawCallRisingEdge();
    SDL_Renderer& renderer() const noexcept;

    /** Assuming target FPS of 60. What fraction of the expected time per frame,
     *  did the last frame take. I.e. (1000 / 60) / (nowMs - lastFrameMs) 
     */
    float deltaT() const noexcept;

private:
    uint32_t m_number;
    float m_fps;
    double m_deltaT;
    ms m_gameStartTime;
    ms m_lastFrameTime;
    SDL_Renderer* m_renderer;
    double m_expectedTimePerFrame = 1000.0 / 60.0;
};

class ApplicationContext : public std::enable_shared_from_this<ApplicationContext> {
public:
    // Destructor
    ~ApplicationContext();

    static std::shared_ptr<ApplicationContext> create(
        glm::fvec2* bounds, SDL_WindowFlags settings, 
        SDL_Window* window, SDL_Renderer* renderer
    ) {
        return std::make_shared<ApplicationContext>(bounds, settings, window, renderer);
    }

    ApplicationContext(
        glm::fvec2* bounds, SDL_WindowFlags settings, 
        SDL_Window* window, SDL_Renderer* renderer
    ) {
        // Allocate memory for viewport and frames
        m_viewport = std::make_unique<ViewportState>(
            bounds, settings, window
        );
        m_frames = std::make_unique<FrameData>(
            renderer, SDL_GetTicks()
        );
        m_input = std::make_unique<InputManager>();
    };

    ViewportState& viewport() const noexcept;
    FrameData& frames() const noexcept;
    InputManager& input() const noexcept;
    SceneManager& scene() noexcept;
    void onDrawCallRisingEdge() const noexcept;
    void onTick();
    void onDraw() noexcept;
    
private:
    std::unique_ptr<ViewportState> m_viewport;
    std::unique_ptr<FrameData> m_frames;
    std::unique_ptr<InputManager> m_input;
    std::unique_ptr<SceneManager> m_sceneManager;
};
