#pragma once

/**
 * @file a_math.hpp
 * @brief Math primitives and camera logic for the Andromeda engine.
 */

#include "a_primitives.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <type_traits>
#include <nlohmann/json.hpp>

 /**
  * @namespace Andromeda::amath
  * @brief Andromeda specific math structures and wrappers.
  */
    namespace Andromeda::amath {
    /**
     * @struct Ray
     * @brief Represents a mathematical ray in 3D space.
     */
    struct Ray {
        vec3 origin;    /**< Start point of the ray. */
        vec3 direction; /**< Direction vector of the ray. */
    };

    /**
     * @struct CameraData
     * @brief Container for camera state, projection, and transformation data.
     */
    struct CameraData {
        vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);            /**< Up vector for view matrix calculation. */
        vec3 cameraPos = glm::vec3(0.0f, 5.0f, 0.0f);     /**< Position of the camera in world space. */
        vec3 cameraTarget = glm::vec3(0.0f);              /**< Point the camera is looking at. */

        float fov = 45.0f;                                /**< Current vertical Field of View. */
        float fovMin = 10.0f, fovMax = 50.0f;             /**< Clamping range for FOV zoom. */

        mat4 projection = { 1.0f };                       /**< Projection matrix. */
        bool canRotate = false;                           /**< Flag for input rotation state. */

        float lastMouseY = 0.0f;                          /**< Previous Y mouse coordinate for delta calculation. */
        float lastMouseX = 0.0f;                          /**< Previous X mouse coordinate for delta calculation. */

        float yaw = -90.0f;                               /**< Horizontal rotation angle in degrees. */
        float pitch = 0.0f;                               /**< Vertical rotation angle in degrees. */
        float sensitivity = 0.1f;                         /**< Mouse sensitivity factor. */

        vec3 target = vec3(0.0f);                         /**< Interpolated or specific look-at target. */
        vec3 forward = vec3(0.0f);                        /**< Forward direction vector derived from rotation. */
        vec3 right = vec3(0.0f);                          /**< Right direction vector derived from rotation. */

        float speed = 25.0f;                              /**< Movement speed. */
        mat4 viewMatrix = { 1.0f };                       /**< Calculated view transformation matrix. */

        bool hasValidPickRay = false;                     /**< True if the cursor-to-world ray is valid. */
        vec2 framebufferSize = vec2(0.0f);                /**< Size of the full window framebuffer. */
        vec2 viewportSize = vec2(0.0f);                   /**< Size of the active rendering viewport. */
        vec2 viewportPos = vec2(0.0f);                    /**< Top-left offset of the viewport. */

        float imGuiMouseX = 0.0f;                         /**< Mouse X position relative to ImGui context. */
        float imGuiMouseY = 0.0f;                         /**< Mouse Y position relative to ImGui context. */

        Ray cursorToWorldRay{ vec3(0.0f), vec3(0.0f) };   /**< Ray projected from mouse position into 3D space. */
    };

    /** @brief Generic matrix/vector inversion wrapper. */
    template<typename T>
    [[nodiscard]] constexpr inline T inverse(const T& matrix) noexcept {
        return glm::inverse(matrix);
    }

    /** @brief Converts degrees to radians. */
    template<typename T>
    [[nodiscard]] constexpr inline T radians(const T degrees) noexcept {
        return glm::radians(degrees);
    }

    /** @brief Returns a unit vector in the same direction as the input. */
    template<typename T>
    [[nodiscard]] constexpr inline T normalize(const T& value) noexcept {
        return glm::normalize(value);
    }

    /** @brief Computes the cross product of two vectors. */
    template<typename T>
    [[nodiscard]] constexpr inline T cross(const T& x, const T& y) noexcept {
        return glm::cross(x, y);
    }

    /** @brief Creates a perspective projection matrix. */
    template<typename T>
    [[nodiscard]] inline mat4 perspective(T fov, T aspect, T zNear, T zFar) noexcept {
        return glm::perspective(fov, aspect, zNear, zFar);
    }

    /** @brief Creates a view matrix for a camera looking at a specific point. */
    [[nodiscard]] inline mat4 lookAt(const vec3& eye, const vec3& center, const vec3& up) noexcept {
        return glm::lookAt(eye, center, up);
    }

    /** @brief Generic absolute value function. */
    template<typename T>
    [[nodiscard]] constexpr inline T abs(T value) noexcept {
        return glm::abs(value);
    }

    /** @brief String conversion for logging; handles arithmetic types and GLM types. */
    template<typename T>
    [[nodiscard]] inline std::string to_string(const T& value) {
        if constexpr (std::is_arithmetic_v<T>) {
            return std::to_string(value);
        }
        else {
            return glm::to_string(value);
        }
    }

    /**
     * @brief Calculates intersection point of a ray with the XZ plane.
     * @param ray The ray to test.
     * @param planeY Height of the XZ plane.
     * @param outHitPoint Output for the intersection coordinates.
     * @return true if an intersection exists (ray is not parallel).
     */
    inline bool RayIntersectsXZPlane(const amath::Ray& ray, float planeY, vec3& outHitPoint)
    {
        if (amath::abs(ray.direction.y) < 1e-6f) {
            return false;
        }
        float t = (planeY - ray.origin.y) / ray.direction.y;
        if (t < 0.0f) {
            return false;
        }
        outHitPoint = ray.origin + t * ray.direction;
        return true;
    }
}

/**
 * @namespace glm
 * @brief Extension of the glm namespace to support nlohmann::json serialization.
 */
namespace glm {
    /** @brief vec3 to JSON array conversion. */
    void to_json(nlohmann::json& json, const vec3& v) {
        json = { v.x, v.y, v.z };
    }

    /** @brief JSON array to vec3 conversion. @throws std::invalid_argument if format is incorrect. */
    void from_json(const nlohmann::json& json, vec3& v) {
        if (!json.is_array() || json.size() != 3) {
            throw std::invalid_argument("JSON is not a 3d vector array");
        }
        v.x = json.at(0).get<float>();
        v.y = json.at(1).get<float>();
        v.z = json.at(2).get<float>();
    }

    /** @brief vec2 to JSON array conversion. */
    void to_json(nlohmann::json& json, const vec2& v) {
        json = { v.x, v.y };
    }

    /** @brief JSON array to vec2 conversion. @throws std::invalid_argument if format is incorrect. */
    void from_json(const nlohmann::json& json, vec2& v) {
        if (!json.is_array() || json.size() != 2) {
            throw std::invalid_argument("JSON is not a 2d vector array");
        }
        v.x = json.at(0).get<float>();
        v.y = json.at(1).get<float>();
    }

    /** @brief vec4 to JSON array conversion. */
    void to_json(nlohmann::json& json, const vec4& v) {
        json = { v.x, v.y, v.z, v.w };
    }

    /** @brief JSON array to vec4 conversion. @throws std::invalid_argument if format is incorrect. */
    void from_json(const nlohmann::json& json, vec4& v) {
        if (!json.is_array() || json.size() != 4) {
            throw std::invalid_argument("JSON is not a 4d vector array");
        }
        v.x = json.at(0).get<float>();
        v.y = json.at(1).get<float>();
        v.z = json.at(2).get<float>();
        v.w = json.at(3).get<float>();
    }

    /** @brief mat4 to nested JSON array conversion (column-major). */
    void to_json(nlohmann::json& j, const mat4& m) {
        j = { m[0], m[1], m[2], m[3] };
    }

    /** @brief Nested JSON array to mat4 conversion. */
    void from_json(const nlohmann::json& j, mat4& m) {
        if (!j.is_array() || j.size() != 4) {
            throw std::invalid_argument("JSON is not a mat4 array (expected 4 column vectors)");
        }
        m[0] = j.at(0).get<vec4>();
        m[1] = j.at(1).get<vec4>();
        m[2] = j.at(2).get<vec4>();
        m[3] = j.at(3).get<vec4>();
    }

    /** @brief mat3 to nested JSON array conversion (column-major). */
    void to_json(nlohmann::json& j, const mat3& m) {
        j = { m[0], m[1], m[2] };
    }

    /** @brief Nested JSON array to mat3 conversion. */
    void from_json(const nlohmann::json& j, mat3& m) {
        if (!j.is_array() || j.size() != 3) {
            throw std::invalid_argument("JSON is not a mat3 array (expected 3 column vectors)");
        }
        m[0] = j.at(0).get<vec3>();
        m[1] = j.at(1).get<vec3>();
        m[2] = j.at(2).get<vec3>();
    }
}