#pragma once
#include "components.h"
#include "camera.h"
namespace Andromeda::Collision {
    inline bool RayIntersectAABB(const Andromeda::Camera& cam,const Andromeda::ECS::Component::AABB& aabb, const glm::mat4& modelMatrix)
    {
        const Andromeda::Ray& rayW = cam.m_CursorToWorldRay;

        glm::mat4 invModel = glm::inverse(modelMatrix);

        glm::vec3 o = glm::vec3(invModel * glm::vec4(rayW.origin, 1.0f));
        glm::vec3 d = glm::normalize(glm::vec3(invModel * glm::vec4(rayW.direction, 0.0f)));

        float tMin = 0.001f;
        float tMax = 1e30f;

        for (int i = 0; i < 3; ++i)
        {
            float oi = o[i];
            float di = d[i];

            if (std::fabs(di) < 1e-8f) {
                if (oi < aabb.min[i] || oi > aabb.max[i]) return false;
                continue;
            }

            float invD = 1.0f / di;
            float t0 = (aabb.min[i] - oi) * invD;
            float t1 = (aabb.max[i] - oi) * invD;
            if (t0 > t1) std::swap(t0, t1);

            tMin = std::max(tMin, t0);
            tMax = std::min(tMax, t1);
            if (tMin > tMax) return false;
        }
        return true;
    }
}