#include <glm/glm.hpp>
#include <SDL3/SDL.h>

#include <entities/entity.h>
#include <entities/components.h>
#include <scene/scene.h>
#include <input/input.h>
#include <meta/processing.h>

class Player : public IGameplayEntity {
private:
    //ref cached locally for performance
    TransformComponent* m_transform;
public:
    Player(std::shared_ptr<ApplicationContext> appCtx) noexcept {
        auto screenBounds = appCtx->viewport().bounds();
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Screen bounds: %d %d", screenBounds->x, screenBounds->y);
        glm::fvec3 position = glm::fvec3(
            screenBounds->x * 0.5,
            screenBounds->y * 0.5, 
            0.0f
        );
        m_transform = addComponentAndGetRawPtr<TransformComponent>(position);
    }

    void tick(std::shared_ptr<ApplicationContext> appCtx, std::shared_ptr<SceneContext> ctx) {
        IGameplayEntity::tick(appCtx, ctx);

        // Move the player
        auto input = appCtx->input();
        if (input.isDown(SDL_SCANCODE_W)) {
            m_transform->position.y -= 1.0f;
        }
        if (input.isDown(SDL_SCANCODE_S)) {
            m_transform->position.y += 1.0f;
        }
        if (input.isDown(SDL_SCANCODE_A)) {
            m_transform->position.x -= 1.0f;
        }
        if (input.isDown(SDL_SCANCODE_D)) {
            m_transform->position.x += 1.0f;
        }
    }

    void draw(std::shared_ptr<ApplicationContext> appCtx) noexcept {
        IGameplayEntity::draw(appCtx);

        SDL_Renderer& renderer = appCtx->frames().renderer();

        SDL_SetRenderDrawColor(&renderer, 0, 0, 255, 255);
        SDL_FRect rect{
            m_transform->position.x, 
            m_transform->position.y, 
            100.0f, 100.0f
        };
        bool drawSuccess = SDL_RenderFillRect(&renderer, &rect);
        if (!drawSuccess) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error: %s", SDL_GetError());
        }
    }

    float getZIndex() const noexcept { 
        return m_transform->position.z; 
    }
};
