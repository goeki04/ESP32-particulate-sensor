#pragma once
#include "a_primitives.hpp"
#include "nlohmann/json.hpp"
/**
 * @namespace glm
 * @brief Extension of the glm namespace to support nlohmann::JSON serialization.
 */
namespace glm {
    /** @brief vec3 to JSON array conversion. */
    inline void to_json(nlohmann::json& json, const vec3& v) {
        json = { v.x, v.y, v.z };
    }

    /** @brief JSON array to vec3 conversion. @throws std::invalid_argument if format is incorrect. */
    inline void from_json(const nlohmann::json& json, vec3& v) {
        if (!json.is_array() || json.size() != 3) {
            throw std::invalid_argument("JSON is not a 3d vector array");
        }
        v.x = json.at(0).get<float>();
        v.y = json.at(1).get<float>();
        v.z = json.at(2).get<float>();
    }

    /** @brief vec2 to JSON array conversion. */
    inline void to_json(nlohmann::json& json, const Andromeda::vec2& v) {
        json = { v.x, v.y };
    }

    /** @brief JSON array to vec2 conversion. @throws std::invalid_argument if format is incorrect. */
    inline void from_json(const nlohmann::json& json, vec2& v) {
        if (!json.is_array() || json.size() != 2) {
            throw std::invalid_argument("JSON is not a 2d vector array");
        }
        v.x = json.at(0).get<float>();
        v.y = json.at(1).get<float>();
    }

    /** @brief vec4 to JSON array conversion. */
    inline void to_json(nlohmann::json& json, const vec4& v) {
        json = { v.x, v.y, v.z, v.w };
    }

    /** @brief JSON array to vec4 conversion. @throws std::invalid_argument if format is incorrect. */
    inline void from_json(const nlohmann::json& json, vec4& v) {
        if (!json.is_array() || json.size() != 4) {
            throw std::invalid_argument("JSON is not a 4d vector array");
        }
        v.x = json.at(0).get<float>();
        v.y = json.at(1).get<float>();
        v.z = json.at(2).get<float>();
        v.w = json.at(3).get<float>();
    }

    /** @brief mat4 to nested JSON array conversion (column-major). */
    inline void to_json(nlohmann::json& j, const mat4& m) {
        j = { m[0], m[1], m[2], m[3] };
    }

    /** @brief Nested JSON array to mat4 conversion. */
    inline void from_json(const nlohmann::json& j, mat4& m) {
        if (!j.is_array() || j.size() != 4) {
            throw std::invalid_argument("JSON is not a mat4 array (expected 4 column vectors)");
        }
        m[0] = j.at(0).get<vec4>();
        m[1] = j.at(1).get<vec4>();
        m[2] = j.at(2).get<vec4>();
        m[3] = j.at(3).get<vec4>();
    }

    /** @brief mat3 to nested JSON array conversion (column-major). */
    inline void to_json(nlohmann::json& j, const mat3& m) {
        j = { m[0], m[1], m[2] };
    }

    /** @brief Nested JSON array to mat3 conversion. */
    inline void from_json(const nlohmann::json& j, mat3& m) {
        if (!j.is_array() || j.size() != 3) {
            throw std::invalid_argument("JSON is not a mat3 array (expected 3 column vectors)");
        }
        m[0] = j.at(0).get<vec3>();
        m[1] = j.at(1).get<vec3>();
        m[2] = j.at(2).get<vec3>();
    }

    inline void to_json(nlohmann::json& j, const quat& q) {
        j = nlohmann::json{ {"x", q.x}, {"y", q.y}, {"z", q.z}, {"w", q.w} };
    }
    inline void from_json(const nlohmann::json& j, quat& q) {
        q.x = j.at("x").get<float>();
        q.y = j.at("y").get<float>();
        q.z = j.at("z").get<float>();
        q.w = j.at("w").get<float>();
    }
}