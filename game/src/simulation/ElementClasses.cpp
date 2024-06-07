#include "ElementClasses.h"

// This is literally stolen from TPT
std::array<Element, PT_NUM> const &GetElements() {
    struct DoOnce {
        std::array<Element, PT_NUM> elements;

        DoOnce() {
#define ELEMENT_NUMBERS_CALL
#include "ElementNumbers.h"
#undef ELEMENT_NUMBERS_CALL

            // Causality check for diffusive elements
            // Temperature transition check (cannot be negative)
            for (auto &el : elements) {
                bool has_diffusion = el.State == ElementState::TYPE_POWDER ||
                    el.State == ElementState::TYPE_LIQUID ||
                    el.State == ElementState::TYPE_GAS;
                if (has_diffusion && el.Causality < el.Diffusion) {
                    el.Causality = std::ceil(el.Diffusion);
                    TraceLog(LOG_WARNING, TextFormat("Element %s has causality (%f) < diffusion (%f), setting causality to diffusion",
                            el.Name.c_str(), el.Causality, el.Diffusion));
                }
                if (el.LowTemperature < 0.0f && el.LowTemperatureTransition != Transition::NONE)
                    TraceLog(LOG_WARNING, TextFormat("Element %s has negative low temperature transition (%f K), did you use _C with a negative number?",
                            el.Name.c_str(), el.LowTemperature));
                if (el.HighTemperature < 0.0f && el.HighTemperatureTransition != Transition::NONE)
                    TraceLog(LOG_WARNING, TextFormat("Element %s has negative high temperature transition (%f K), did you use _C with a negative number?",
                            el.Name.c_str(), el.HighTemperature));
            }
        }
    };

    static DoOnce doOnce;
    return doOnce.elements;
    }
