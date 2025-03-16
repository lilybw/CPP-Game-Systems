#pragma once

#include <meta/ApplicationContext.h>
#include <entities/entity.h>
#include <meta/processing.h>

class IScene : public IDrawable, public ITickable {
private:
    // Simple id for each scene
    static inline int s_id = 0;
    int m_id;
public:
    IScene(std::shared_ptr<ApplicationContext> ctx) noexcept : m_id(s_id++) {};
    virtual ~IScene() = default;
    void onTearDown() noexcept {};
    void onDrawCallRisingEdge() noexcept {};

    int getSceneId() const noexcept { return m_id; }
};

class MenuScreen : public IScene {
public:
    MenuScreen(std::shared_ptr<ApplicationContext> ctx) noexcept;
    ~MenuScreen() = default;

    void tick(std::shared_ptr<ApplicationContext> ctx) override;
    void draw(std::shared_ptr<ApplicationContext> ctx) noexcept override;
};

//Forward declaration
class SceneContext;

/** Anything in a scene, that is drawn and or ticked. Could be the player. Could be a wall. Who knows? */
class IGameplayEntity : public IEntity, public IDrawable {
public:
    IGameplayEntity() = default;
    virtual ~IGameplayEntity() = default;

    void tick(std::shared_ptr<ApplicationContext> appCtx, std::shared_ptr<SceneContext> ctx) noexcept {
        forEachComponent<ITickable>([&](ITickable* tickable) {
            tickable->tick(appCtx, ctx);
        });
    };
    void draw(std::shared_ptr<ApplicationContext> ctx) noexcept {
        forEachComponent<IDrawable>([&](IDrawable* drawable) {
            drawable->draw(ctx);
        });
    };
};

class SceneContext : public IEntity {
public:
    SceneContext() = default;
    ~SceneContext() = default;

    bool registerEntity(IGameplayEntity* entity) {
        if (entity == nullptr) {
            throw std::runtime_error("Cannot register null entity");
        }

        entities.push_back(entity);
        entity->onDestruction(std::bind(&SceneContext::handleEntityDestruction, this, std::placeholders::_1));
        return true;
    }

    std::vector<IGameplayEntity*> getEntities() {
        return entities;
    }
private:
    std::vector<IGameplayEntity*> entities = {};
    std::vector<IDrawable*> ui = {};
    std::vector<ITickable*> otherwiseTickable = {};

    void handleEntityDestruction(IEntity* entity) {
        entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
    }
};

