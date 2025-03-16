#pragma once

#include <memory>

#include <scene/scene.h>
#include <meta/ApplicationContext.h>

class SceneManager {
public:
    SceneManager() = default;
    ~SceneManager() = default;

    void onTick(std::shared_ptr<ApplicationContext> ctx) const;
    void onDraw(std::shared_ptr<ApplicationContext> ctx) const noexcept;
    void onDrawCallRisingEdge() const noexcept;
    void onEvent(SDL_Event* event) const noexcept;
    void changeScene(IScene* scene) noexcept;
};
