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
SpriteRenderer    *Renderer;
SpriteBatchRenderer    *BatchRenderer;
GameObject        *Player;
BallObject        *Ball;
ParticleGenerator *Particles;
PostProcessor     *Effects;
ISoundEngine      *SoundEngine = createIrrKlangDevice();
TextRenderer      *Text;

float ShakeTime = 0.0f;


Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height), Level(0), Lives(3)
{ 
    SoundEngine->setSoundVolume(0.5);
}

Game::~Game()
{
    delete Renderer;
    delete Player;
    delete Ball;
    delete Particles;
    delete Effects;
    delete Text;
    SoundEngine->drop();
}

std::wstring PreLoadString()
{
    std::wstring ret;
    for (wchar_t c = 32; c < 127; ++c)
    {
        ret += c;
    }
    ret += L"，。？！【】（）《》“”；：、·";
    return ret;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("shaders/particle.vs", "shaders/particle.frag", nullptr, "particle");
    ResourceManager::LoadShader("shaders/post_processing.vs", "shaders/post_processing.frag", nullptr, "postprocessing");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite")->Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite")->SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle")->Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle")->SetMatrix4("projection", projection);
    // load textures
    ResourceManager::LoadTexture("textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("textures/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("textures/block.png", false, "block");
    ResourceManager::LoadTexture("textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("textures/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("textures/particle.png", true, "particle");
    ResourceManager::LoadTexture("textures/powerup_speed.png", true, "powerup_speed");
    ResourceManager::LoadTexture("textures/powerup_sticky.png", true, "powerup_sticky");
    ResourceManager::LoadTexture("textures/powerup_increase.png", true, "powerup_increase");
    ResourceManager::LoadTexture("textures/powerup_confuse.png", true, "powerup_confuse");
    ResourceManager::LoadTexture("textures/powerup_chaos.png", true, "powerup_chaos");
    ResourceManager::LoadTexture("textures/powerup_passthrough.png", true, "powerup_passthrough");
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    BatchRenderer = new SpriteBatchRenderer(Renderer);
    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
    Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
    Text = new TextRenderer(this->Width, this->Height);
    Text->PreLoad("fonts/WenQuanWeiMiHei-1.TTF", 24, PreLoadString());
    // load levels
    GameLevel one; one.Load("levels/one.lvl", this->Width, this->Height / 2);
    GameLevel two; two.Load("levels/two.lvl", this->Width, this->Height /2 );
    GameLevel three; three.Load("levels/three.lvl", this->Width, this->Height / 2);
    GameLevel four; four.Load("levels/four.lvl", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;
    // configure game objects
    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
    
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
    Ball->Spr->TexSize = glm::vec2(0.5f);
    // audio
    SoundEngine->play2D("audio/breakout.mp3", true);
}

void Game::Update(float dt)
{
    // update objects
    Ball->Move(dt, this->Width);
    // check for collisions
    this->DoCollisions();
    // update particles
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));
    // update PowerUps
    this->UpdatePowerUps(dt);
    // reduce shake time
    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
            Effects->Shake = false;
    }
    // check loss condition
    if (Ball->Position.y >= this->Height) // did ball reach bottom edge?
    {
        --this->Lives;
        // did the player lose all his lives? : game over
        if (this->Lives == 0)
        {
            this->ResetLevel();
            this->State = GAME_MENU;
        }
        this->ResetPlayer();
    }
    // check win condition
    if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
    {
        this->ResetLevel();
        this->ResetPlayer();
        Effects->Chaos = true;
        this->State = GAME_WIN;
    }

    static float rateUpdate = 0;
    rateUpdate += dt;
    if (rateUpdate > 1)
    {
        rateUpdate = 0;
        wchar_t buf[128];
        swprintf_s(buf, L"%.2f", 1 / dt);
        FrameRate = buf;
    }
}


void Game::ProcessInput(float dt)
{
    if (this->State == GAME_MENU)
    {
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
        }
        if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
        {
            this->Level = (this->Level + 1) % 4;
            this->KeysProcessed[GLFW_KEY_W] = true;
        }
        if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
        {
            if (this->Level > 0)
                --this->Level;
            else
                this->Level = 3;
            //this->Level = (this->Level - 1) % 4;
            this->KeysProcessed[GLFW_KEY_S] = true;
        }
    }
    if (this->State == GAME_WIN)
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
            Effects->Chaos = false;
            this->State = GAME_MENU;
        }
    }
    if (this->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_SPACE])
            Ball->Stuck = false;
    }
}

bool batch = false;
void Game::Render()
{
    if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN)
    {
        // begin rendering to postprocessing framebuffer
        Effects->BeginRender();
        // draw background
        Renderer->DrawSprite(*ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        if (batch)
        {
            BatchRenderer->ClearData();
            this->Levels[this->Level].BatchDraw(*BatchRenderer);
            Player->RegistDraw(*BatchRenderer);
            for (PowerUp &powerUp : this->PowerUps)
                if (!powerUp.Destroyed)
                    powerUp.RegistDraw(*BatchRenderer);
            Ball->RegistDraw(*BatchRenderer);
            Particles->Draw();
            BatchRenderer->Render();     
        }
        else
        {
            this->Levels[this->Level].Draw(*Renderer);
            Player->Draw(*Renderer);
            for (PowerUp &powerUp : this->PowerUps)
                if (!powerUp.Destroyed)
                    powerUp.Draw(*Renderer);
            Ball->Draw(*Renderer);
        }
        // end rendering to postprocessing framebuffer
        Effects->EndRender();
        // render postprocessing quad
        Effects->Render(glfwGetTime());
        // render text (don't include in postprocessing)
        std::wstringstream ss; ss << this->Lives;
        Text->RenderText(L"Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
        Text->RenderText(FrameRate, 740.0f, 3.0f, 0.5f);
    }
    if (this->State == GAME_MENU)
    {
        Text->RenderText(L"Press ENTER to 开始", 250.0f, this->Height / 2.0f, 1.0f);
        Text->RenderText(L"按下 W or S to select 关卡", 250.0f, this->Height / 2.0f + 40.0f, 0.75f, { 1.0f, 0.0f, 1.0f });
    }
    if (this->State == GAME_WIN)
    {
        Text->RenderText(L"You WON!!!", 320.0f, this->Height / 2.0f - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        Text->RenderText(L"按下 ENTER to retry or ESC to quit", 130.0f, this->Height / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
}

void Game::ResetLevel()
{
    if (this->Level == 0)
        this->Levels[0].Load("levels/one.lvl", this->Width, this->Height / 2);
    else if (this->Level == 1)
        this->Levels[1].Load("levels/two.lvl", this->Width, this->Height / 2);
    else if (this->Level == 2)
        this->Levels[2].Load("levels/three.lvl", this->Width, this->Height / 2);
    else if (this->Level == 3)
        this->Levels[3].Load("levels/four.lvl", this->Width, this->Height / 2);

    this->Lives = 3;
}

void Game::ResetPlayer()
{
    // reset player/ball stats
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
    // also disable all active powerups
    Effects->Chaos = Effects->Confuse = false;
    Ball->PassThrough = Ball->Sticky = false;
    Player->Color = glm::vec3(1.0f);
    Ball->Color = glm::vec3(1.0f);
}


// powerups
bool IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);

void Game::UpdatePowerUps(float dt)
{
    for (PowerUp &powerUp : this->PowerUps)
    {
        powerUp.Position += powerUp.Velocity * dt;
        if (powerUp.Activated)
        {
            powerUp.Duration -= dt;

            if (powerUp.Duration <= 0.0f)
            {
                // remove powerup from list (will later be removed)
                powerUp.Activated = false;
                // deactivate effects
                if (powerUp.Type == "sticky")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
                    {	// only reset if no other PowerUp of type sticky is active
                        Ball->Sticky = false;
                        Player->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "pass-through")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "pass-through"))
                    {	// only reset if no other PowerUp of type pass-through is active
                        Ball->PassThrough = false;
                        Ball->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "confuse")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
                    {	// only reset if no other PowerUp of type confuse is active
                        Effects->Confuse = false;
                    }
                }
                else if (powerUp.Type == "chaos")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
                    {	// only reset if no other PowerUp of type chaos is active
                        Effects->Chaos = false;
                    }
                }
            }
        }
    }
    // Remove all PowerUps from vector that are destroyed AND !activated (thus either off the map or finished)
    // Note we use a lambda expression to remove each PowerUp which is destroyed and not activated
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
        [](const PowerUp &powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
    ), this->PowerUps.end());
}

bool ShouldSpawn(unsigned int chance)
{
    unsigned int random = rand() % chance;
    return random == 0;
}
void Game::SpawnPowerUps(GameObject &block)
{
    if (ShouldSpawn(75)) // 1 in 75 chance
        this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
    if (ShouldSpawn(15)) // Negative powerups should spawn more often
        this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
    if (ShouldSpawn(15))
        this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));
}

void ActivatePowerUp(PowerUp &powerUp)
{
    if (powerUp.Type == "speed")
    {
        Ball->Velocity *= 1.2;
    }
    else if (powerUp.Type == "sticky")
    {
        Ball->Sticky = true;
        Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.Type == "pass-through")
    {
        Ball->PassThrough = true;
        Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.Type == "pad-size-increase")
    {
        Player->Size.x += 50;
    }
    else if (powerUp.Type == "confuse")
    {
        if (!Effects->Chaos)
            Effects->Confuse = true; // only activate if chaos wasn't already active
    }
    else if (powerUp.Type == "chaos")
    {
        if (!Effects->Confuse)
            Effects->Chaos = true;
    }
}

bool IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type)
{
    // Check if another PowerUp of the same type is still active
    // in which case we don't disable its effect (yet)
    for (const PowerUp &powerUp : powerUps)
    {
        if (powerUp.Activated)
            if (powerUp.Type == type)
                return true;
    }
    return false;
}


void Game::DoCollisions()
{
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = Collision::CheckCircleAABB(*Ball, box);
            if (collision.collide) // if collision is true
            {
                // destroy block if not solid
                if (!box.IsSolid)
                {
                    box.Destroyed = true;
                    this->SpawnPowerUps(box);
                    SoundEngine->play2D("audio/bleep.mp3", false);
                }
                else
                {   // if block is solid, enable shake effect
                    ShakeTime = 0.05f;
                    Effects->Shake = true;
                    SoundEngine->play2D("audio/bleep.mp3", false);
                }
                // collision resolution
                Collision::Direction dir = collision.dir;
                glm::vec2 diff_vector = collision.diff;
                if (!(Ball->PassThrough && !box.IsSolid)) // don't do collision resolution on non-solid bricks if pass-through is activated
                {
                    if (dir == Collision::LEFT || dir == Collision::RIGHT) // horizontal collision
                    {
                        Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity
                                                              // relocate
                        float penetration = Ball->Radius - std::abs(diff_vector.x);
                        if (dir == Collision::LEFT)
                            Ball->Position.x += penetration; // move ball to right
                        else
                            Ball->Position.x -= penetration; // move ball to left;
                    }
                    else // vertical collision
                    {
                        Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
                                                              // relocate
                        float penetration = Ball->Radius - std::abs(diff_vector.y);
                        if (dir == Collision::UP)
                            Ball->Position.y -= penetration; // move ball bback up
                        else
                            Ball->Position.y += penetration; // move ball back down
                    }
                }
            }
        }    
    }

    // also check collisions on PowerUps and if so, activate them
    for (PowerUp &powerUp : this->PowerUps)
    {
        if (!powerUp.Destroyed)
        {
            // first check if powerup passed bottom edge, if so: keep as inactive and destroy
            if (powerUp.Position.y >= this->Height)
                powerUp.Destroyed = true;

            if (Collision::CheckAABBAABB(Player->GetAABB(), powerUp.GetAABB()))
            {	// collided with player, now activate powerup
                ActivatePowerUp(powerUp);
                powerUp.Destroyed = true;
                powerUp.Activated = true;
                SoundEngine->play2D("audio/powerup.wav", false);
            }
        }
    }

    // and finally check collisions for player pad (unless stuck)
    Collision result = Collision::CheckCircleAABB(Ball->GetCircle(), Player->GetAABB());
    if (!Ball->Stuck && result)
    {
        // check where it hit the board, and change velocity based on where it hit the board
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        // then move accordingly
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength; 
        //Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); // keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
                                                                                    // fix sticky paddle
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);

        // if Sticky powerup is activated, also stick ball to paddle once new velocity vectors were calculated
        Ball->Stuck = Ball->Sticky;

        SoundEngine->play2D("audio/bleep.wav", false);
    }
}
