#ifndef GAME_H
#define GAME_H

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"
#include <SDL2/SDL.h>

const int FPS = 30;
const int MILLISECS_PER_FRAME = 1000/FPS;

class Game { 
private:
    bool isRunning;
    bool isDebug;
    int millisecsPreviousFrame = 0;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Rect camera;

    std::unique_ptr<Registry> registry;
    std::unique_ptr<AssetStore> assetStore;
    std::unique_ptr<EventBus> eventBus;

public:
    Game();
    ~Game();
    void Initialize();
    void Run();
    void Destroy();
    void ProcessInput();
    void LoadLevel(int level);
    void Setup();
    void Update();
    void Render();

    static int windowWidth;
    static int windowHeight;    
    static int mapWidth;
    static int mapHeight;
};

#endif