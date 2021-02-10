/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef GAME_H
#define GAME_H
#include <vector>
#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_level.h"
#include "power_up.h"

class Game
{
public:
    bool                    Keys[1024];
    bool                    KeysProcessed[1024];
    unsigned int            Width, Height;

    Game(unsigned int width, unsigned int height);
    ~Game();

    virtual void Init() {}
    virtual void ProcessInput(float dt) {}
    virtual void Update(float dt) {}
    virtual void Render() {}

    static Game *GetInstance() { return Instance; }
    static Game *Instance;
};

// should be implemented in concrete game
void GameInit();

#endif