#include <scene/scene.h>
#include <SDL3/SDL.h>

MenuScreen::MenuScreen(std::shared_ptr<ApplicationContext> ctx) noexcept : IScene::IScene(ctx) {
    
}

void MenuScreen::tick(std::shared_ptr<ApplicationContext> ctx) {
    // Do nothing for now
}

void MenuScreen::draw(std::shared_ptr<ApplicationContext> ctx) noexcept {
    SDL_Renderer& renderer = ctx->frames().renderer();

    SDL_SetRenderDrawColor(&renderer, 0, 0, 0, 255);
    SDL_RenderClear(&renderer);
}

