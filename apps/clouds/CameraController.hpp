
#pragma once

#include <common/Common.hpp>
#include "Camera.hpp"
#include <glm/glm.hpp>

class CameraController
{
public:
    CameraController(PerspectiveCamera& cam);
    ~CameraController();

    void update(float dt);

private:

    void keyPan(float dt);
    void mousePan(float dx, float dy);

    PerspectiveCamera& mCamera;

    glm::vec3 mPosition;
    glm::vec3 mDirection;
    glm::vec3 mStartDir;

    F32 mAngleH;
    F32 mAngleV;

    F32 mOldX;
    F32 mOldY;

};