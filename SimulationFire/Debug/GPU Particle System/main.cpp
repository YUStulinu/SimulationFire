#include "Renderer.h"   

int main(int argc, char* args[]) {
    SDL_Init(SDL_INIT_EVERYTHING);
    Renderer renderer;
    ParticleSystem* particleSystem = new ParticleSystem;
    renderer.particleSystem = particleSystem;

    int mods = 0;
    float yaw = -90.0f;
    float pitch = 0.0f;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false; // Iesire din bucla daca primim un eveniment de inchidere a ferestrei
            }
            // Tratarea evenimentelor de tastatura si mouse
            switch (event.type) {
            case SDL_KEYDOWN:
                renderer.OnKeyPress(event.key.keysym.sym, event.key.keysym.mod);
                break;
            case SDL_KEYUP:
                renderer.OnKeyRelease(event.key.keysym.sym, event.key.keysym.mod);
                break;
            case SDL_MOUSEBUTTONDOWN:
                mods = SDL_GetModState();
                renderer.OnMouseBtnPress(event.button.x, event.button.y, event.button.button, mods);
                break;
            case SDL_MOUSEBUTTONUP:
                mods = SDL_GetModState();
                renderer.OnMouseBtnRelease(event.button.x, event.button.y, event.button.button, mods);
                break;
            case SDL_MOUSEMOTION:
                renderer.OnMouseMove(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
                break;
            case SDL_MOUSEWHEEL:
                renderer.OnMouseScroll(event.wheel.x, event.wheel.y, event.wheel.x, event.wheel.y);
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    renderer.OnWindowResize(event.window.data1, event.window.data2);
                }
                break;
            default:
                break;
            }
        }

        renderer.update(); // Actualizare renderer

        SDL_Delay(10); // O scurta intarziere pentru a elibera CPU-ul intre actualizari
    }

    delete particleSystem;
    SDL_Quit();
    return 0;
}