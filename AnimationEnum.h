#pragma once

#include <string>

enum AnimationState
{
    IDLE,
    WALKING
};

inline const std::string getAnimationString(AnimationState state)
{
    switch (state)
    {
    case IDLE:
        return "Armature|Idle";
    case WALKING:
        return "Armature|walking";
    default:
        return "";
    }
}
