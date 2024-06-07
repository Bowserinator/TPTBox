#ifndef INTERFACE_HUD_ABSTRACT_IMIDDLETOOLTIP_H_
#define INTERFACE_HUD_ABSTRACT_IMIDDLETOOLTIP_H_

#include <string>

// Inherit this for classes that display a tooltip in the middle
class IMiddleTooltip {
public:
    bool hasTooltip() const { return tooltip_to_display.length() > 0; }
    std::string getTooltip() const { return tooltip_to_display; }
    void clearTooltip() { tooltip_to_display = ""; }

protected:
    void setCurrentTooltip(const std::string &tooltip) { tooltip_to_display = tooltip; };

private:
    std::string tooltip_to_display = "";
};

#endif // INTERFACE_HUD_ABSTRACT_IMIDDLETOOLTIP_H_
