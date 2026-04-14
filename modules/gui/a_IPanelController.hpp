#pragma once
#include <string_view>

namespace Andromeda::Gui{
    class IPanelController{
    public:
        virtual ~IPanelController() = default;
        virtual bool isPanelOpen(std::string_view name) const = 0;
        virtual void setPanelOpen(std::string_view name, bool open) = 0;
    };
}