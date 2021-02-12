/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef BREAKOUT_H
#define BREAKOUT_H
#include <vector>
#include <tuple>

#include <glad/glad.h>
#include "Game.h"

#include "game_level.h"
#include "power_up.h"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;

class BreakOut : public Game
{
public:
    // game state
    GameState               State;	
    std::vector<GameLevel>  Levels;
    std::vector<PowerUp>    PowerUps;
    unsigned int            Level;
    unsigned int            Lives;

    // constructor/destructor
    BreakOut(unsigned int width, unsigned int height);
    ~BreakOut();
    void Init();
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();
    // reset
    void ResetLevel();
    void ResetPlayer();
    // powerups
    void SpawnPowerUps(GameObject &block);
    void UpdatePowerUps(float dt);
};

#endif