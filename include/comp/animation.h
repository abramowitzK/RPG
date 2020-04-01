#pragma once

struct AnimationTimer
{
    int StartFrame;
    int EndFrame;
    float Timer;
    float Speed;
};

int GetAnimFrame(AnimationTimer *anim, double dt)
{
    anim->Timer += anim->Speed + dt;
    return (int)anim->Timer % (anim->EndFrame - anim->StartFrame);
}