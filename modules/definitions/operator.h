#pragma once
#include <ostream>
#include <glm/vec4.hpp> 
namespace Andromeda::Util {

    inline bool operator==(const ImVec2& a, const ImVec2& b) {
        return a.x == b.x && a.y == b.y;
    }
    inline std::ostream& operator<<(std::ostream& os, const glm::vec4& a) {
        os << "(" << a.x << ", " << a.y << ", " << a.z << ", " << a.w << ")";
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const ImVec2& a) {
        os << "(" << a.x << ", " << a.y << ")";
        return os;
    }
}