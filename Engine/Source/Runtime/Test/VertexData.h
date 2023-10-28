
#pragma once

#include <glm.hpp>

static const Vector3 kPositions[36] = {
    //Front
    Vector3(-1.0f, -1.0f, 1.0f),
    Vector3(1.0f, -1.0f, 1.0f),
    Vector3(1.0f, 1.0f, 1.0f),

    Vector3(-1.0f, -1.0f, 1.0f),
    Vector3(1.0f, 1.0f, 1.0f),
    Vector3(-1.0f, 1.0f, 1.0f),

    //back
    Vector3(1.0f, -1.0f, -1.0f),
    Vector3(-1.0f, -1.0f, -1.0f),
    Vector3(-1.0f, 1.0f, -1.0f),

    Vector3(1.0f, -1.0f, -1.0f),
    Vector3(-1.0f, 1.0f, -1.0f),
    Vector3(1.0f, 1.0f, -1.0f),

    //left
    Vector3(-1.0f, -1.0f, -1.0f),
    Vector3(-1.0f, -1.0f, 1.0f),
    Vector3(-1.0f, 1.0f, 1.0f),

    Vector3(-1.0f, -1.0f, -1.0f),
    Vector3(-1.0f, 1.0f, 1.0f),
    Vector3(-1.0f, 1.0f, -1.0f),

    //right
    Vector3(1.0f, -1.0f, 1.0f),
    Vector3(1.0f, -1.0f, -1.0f),
    Vector3(1.0f, 1.0f, -1.0f),

    Vector3(1.0f, -1.0f, 1.0f),
    Vector3(1.0f, 1.0f, -1.0f),
    Vector3(1.0f, 1.0f, 1.0f),

    //up
    Vector3(-1.0f, 1.0f, 1.0f),
    Vector3(1.0f, 1.0f, 1.0f),
    Vector3(1.0f, 1.0f, -1.0f),

    Vector3(-1.0f, 1.0f, 1.0f),
    Vector3(1.0f, 1.0f, -1.0f),
    Vector3(-1.0f, 1.0f, -1.0f),

    //down
    Vector3(-1.0f, -1.0f, -1.0f),
    Vector3(1.0f, -1.0f, -1.0f),
    Vector3(1.0f, -1.0f, 1.0f),

    Vector3(-1.0f, -1.0f, -1.0f),
    Vector3(1.0f, -1.0f, 1.0f),
    Vector3(-1.0f, -1.0f, 1.0f),
};

static const Vector4 kColors[36] = {
    //Front
    Vector4(1, 0, 0, 1),
    Vector4(1, 0, 0, 1),
    Vector4(1, 0, 0, 1),

    Vector4(1, 0, 0, 1),
    Vector4(1, 0, 0, 1),
    Vector4(1, 0, 0, 1),

    //back
    Vector4(1, 0, 0, 1),
    Vector4(1, 0, 0, 1),
    Vector4(1, 0, 0, 1),

    Vector4(1, 0, 0, 1),
    Vector4(1, 0, 0, 1),
    Vector4(1, 0, 0, 1),

    //left
    Vector4(0, 1, 0, 1),
    Vector4(0, 1, 0, 1),
    Vector4(0, 1, 0, 1),

    Vector4(0, 1, 0, 1),
    Vector4(0, 1, 0, 1),
    Vector4(0, 1, 0, 1),

    //right
    Vector4(0, 1, 0, 1),
    Vector4(0, 1, 0, 1),
    Vector4(0, 1, 0, 1),

    Vector4(0, 1, 0, 1),
    Vector4(0, 1, 0, 1),
    Vector4(0, 1, 0, 1),

    //up
    Vector4(0, 0, 1, 1),
    Vector4(0, 0, 1, 1),
    Vector4(0, 0, 1, 1),

    Vector4(0, 0, 1, 1),
    Vector4(0, 0, 1, 1),
    Vector4(0, 0, 1, 1),

    //down
    Vector4(0, 0, 1, 1),
    Vector4(0, 0, 1, 1),
    Vector4(0, 0, 1, 1),

    Vector4(0, 0, 1, 1),
    Vector4(0, 0, 1, 1),
    Vector4(0, 0, 1, 1),
};