#ifndef GAME_H
#define GAME_H

class Game { 
private:

public:
    Game();
    ~Game();
    void Initialize();
    void Run();
    void Destroy();
    void ProcessInput();
    void Update();
    void Render();
    
};

#endif