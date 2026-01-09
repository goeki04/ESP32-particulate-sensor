#pragma once
#include <ostream>
#include <glm/vec4.hpp> 
#include <imgui.h>  
namespace util {
    inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
        return ImVec2(a.x + b.x, a.y + b.y);
    }

    inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) {
        return ImVec2(a.x - b.x, a.y - b.y);
    }

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
    
    inline void stringToLower(std::string& data) {
        std::transform(data.begin(), data.end(), data.begin(), 
            [](unsigned char c) {return std::tolower(c);
            });
    }
}