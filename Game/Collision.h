#pragma once
#include <glm/glm.hpp>


struct Rect
{
    float left, up, right, bottom;
};

struct Circle
{
    float ox, oy, r;
};

class Collision
{
public:
    enum Direction {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };
public :
    bool collide = false;
    Direction dir;
    glm::vec2 diff;

    operator bool()
    {
        return this->collide;
    }

public:
    template<typename T1, typename T2>
    static Collision CheckAABBAABB(const T1 &t1, const T2 &t2)
    {
        return CheckAABBAABB(t1.GetAABB(), t2.GetAABB());
    }

    template<typename T1, typename T2>
    static Collision CheckCircleAABB(const T1 &t1, const T2 &t2)
    {
        return CheckCircleAABB(t1.GetCircle(), t2.GetAABB());
    }

    static Collision CheckAABBAABB(const Rect &r1, const Rect &r2)
    {
        bool collisionX = r1.right >= r2.left &&
            r2.right >= r1.left;
        bool collisionY = r1.bottom >= r2.up &&
            r2.bottom >= r1.up;
        return { collisionX && collisionY, DOWN, glm::vec2{0.0f} };
    }

    static Collision CheckCircleAABB(const Circle &c, const Rect &r)
    {
        glm::vec2 center(c.ox, c.oy);
        glm::vec2 aabb_half_extents((r.right - r.left) / 2.0f, (r.bottom - r.up) / 2.0f);
        glm::vec2 aabb_center(r.left + aabb_half_extents.x, r.up + aabb_half_extents.y);
        glm::vec2 difference = center - aabb_center;
        glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
        glm::vec2 closest = aabb_center + clamped;
        difference = closest - center;

        if (glm::length(difference) < c.r) 
            return { true, VectorDirection(difference), difference };
        else
            return { false, UP, glm::vec2(0.0f, 0.0f) };
    }

    static Direction VectorDirection(glm::vec2 target)
    {
        glm::vec2 compass[] = {
            glm::vec2(0.0f, 1.0f),	// up
            glm::vec2(1.0f, 0.0f),	// right
            glm::vec2(0.0f, -1.0f),	// down
            glm::vec2(-1.0f, 0.0f)	// left
        };
        float max = 0.0f;
        unsigned int best_match = -1;
        for (unsigned int i = 0; i < 4; i++)
        {
            float dot_product = glm::dot(glm::normalize(target), compass[i]);
            if (dot_product > max)
            {
                max = dot_product;
                best_match = i;
            }
        }
        return (Direction)best_match;
    }
};

