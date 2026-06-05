#pragma once
#include <string_view>
#include <vector>
namespace Andromeda::Gui{
    class IPanelController{
    public:
        virtual ~IPanelController() = default;
        virtual bool isPanelOpen(std::string_view name) const = 0;
        virtual void setPanelOpen(std::string_view name, bool open) = 0;
        virtual std::vector<std::string_view> getPanelNames() const = 0;
    };
}   