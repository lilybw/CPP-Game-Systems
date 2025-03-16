#pragma once

#include <entities/entity.h>
#include <entities/components.h>
#include <glm/glm.hpp>

class ICollider : public IDependentEntityComponent {
protected:
    TransformComponent* m_transform;
public:
    ICollider(ComponentRetriever compRet) : IDependentEntityComponent(compRet) {
        m_transform = requireComponent<TransformComponent>("Any collider requires a base TransformComponent");
    }
    virtual bool overlaps(const glm::fvec3* point) const noexcept = 0;
};

class SphereCollider : public ICollider {
private:
    //Squared radius
    float m_radiusSQ;
public:
    SphereCollider(ComponentRetriever compRet, float radius) : ICollider(compRet), m_radiusSQ(radius*radius) {}

    bool overlaps(const glm::fvec3* pointB) const noexcept override {
        glm::fvec3 pointA = m_transform->position;
        float deltaX = pointA.x - pointB->x;
        float deltaY = pointA.y - pointB->y;
        float deltaZ = pointA.z - pointB->z;
        return deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ <= m_radiusSQ;
    }
};

class BoxCollider : public ICollider {
private:
    glm::fvec3 m_size;
public:
    BoxCollider(ComponentRetriever compRet, glm::fvec3 size) : ICollider(compRet), m_size(size) {}

    bool overlaps(const glm::fvec3* pointB) const noexcept override {
        glm::fvec3 pointA = m_transform->position;
        return pointA.x - m_size.x <= pointB->x && pointA.x + m_size.x >= pointB->x &&
               pointA.y - m_size.y <= pointB->y && pointA.y + m_size.y >= pointB->y &&
               pointA.z - m_size.z <= pointB->z && pointA.z + m_size.z >= pointB->z;
    }
};

class PolygonCollider : public ICollider {
private:
    std::vector<glm::fvec3> m_points;
public:
    PolygonCollider(ComponentRetriever compRet, std::vector<glm::fvec3> points) : ICollider(compRet), m_points(points) {}

    bool overlaps(const glm::fvec3* pointB) const noexcept override {
        glm::fvec3 pointA = m_transform->position;

        //TODO: 3D raycasting algorithm

        return false;
    }
};

