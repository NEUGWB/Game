/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include <algorithm>
#include <sstream>
#include <iostream>

#include "Collision.h"
#include <irrklang/irrKlang.h>
using namespace irrklang;

#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "ball_object.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "text_renderer.h"


// Game-related State data

Game::Game(unsigned int width, unsigned int height) 
    : Keys(), KeysProcessed(), Width(width), Height(height)
{
    assert(!Instance);
    Instance = this;
}

Game::~Game()
{

}

void Game::SetFrameRate(float rate, float delta)
{
    wchar_t buf[128];
    swprintf_s(buf, L"%.2f, %.2f", rate, delta);
    FrameRate = buf;
}

Game *Game::Instance;