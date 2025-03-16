#include <SDL3/SDL.h>
#include <memory>

#include <scene/scene.h>
#include <player/player.cpp>

class TestScreen : public IScene {
private:
    std::unique_ptr<Player> m_player;
    std::shared_ptr<SceneContext> m_sceneCtx;

public:
    TestScreen(std::shared_ptr<ApplicationContext> appCtx) noexcept : IScene::IScene(appCtx) {
        m_player = std::make_unique<Player>(appCtx);
        m_sceneCtx = std::make_shared<SceneContext>();
    };
    ~TestScreen() = default;

    void tick(std::shared_ptr<ApplicationContext> appCtx) noexcept override {
        m_player->tick(appCtx, m_sceneCtx);
    }
    
    void draw(std::shared_ptr<ApplicationContext> appCtx) noexcept override {
        SDL_Renderer& renderer = appCtx->frames().renderer();
    
        SDL_SetRenderDrawColor(&renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(&renderer);

        m_player->draw(appCtx);
    }
};
