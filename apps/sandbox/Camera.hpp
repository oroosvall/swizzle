#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class PerspectiveCamera
{
public:
    PerspectiveCamera(float fov, float width, float heigth);
    ~PerspectiveCamera();

    void changeFov(float fov);
    void changeAspect(float width, float height);

    void setPosition(glm::vec3 pos);
    void setRotation(glm::vec3 rot);

    void lookAt(glm::vec3 pos, glm::vec3 dir, glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F));

    const glm::vec3& getPosition();
    const glm::vec3& getRotation();

    const glm::mat4& getView() const;
    const glm::mat4& getProjection() const;

    const glm::mat4& getViewProjection() const;

private:
    void recalculatePerspective();
    void recalculateViewProj();

    float mFov;
    float mWidth;
    float mHeight;

    glm::mat4 mProjectionMatrix;
    glm::mat4 mViewMatrix;
    glm::mat4 mProjView;

    glm::vec3 mPosition;
    glm::vec3 mRotation;
};

#endif