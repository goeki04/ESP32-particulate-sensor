#pragma once
namespace Andromeda::Gui{
    inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
        return { a.x + b.x, a.y + b.y };
    }
    inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) {
        return { a.x - b.x, a.y - b.y };
    }
    inline bool operator==(const ImVec2& a, const ImVec2& b) {
        if (a.x == b.x && a.y == b.y) return true;
        return false;
    }
    inline ImVec2 operator*(const ImVec2& a, const float b) {
        return { a.x * b, a.y * b };
    }
}