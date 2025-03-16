#pragma once

#include <memory>

/** Source meta/ApplicationContext.h */
class ApplicationContext;
/** Source scene/scene.h */
class SceneContext;

class ITickable {
public:
    virtual void tick(std::shared_ptr<ApplicationContext> ctx) = 0;
    void tick(std::shared_ptr<ApplicationContext> ctx, std::shared_ptr<SceneContext> sceneCtx) {
        tick(ctx);
    }
};

class IDrawable {
public:
    virtual void draw(std::shared_ptr<ApplicationContext> ctx) noexcept = 0;
    // Lower is "earlier" in the draw order
    double getZIndex() const noexcept { return 0; }
};
