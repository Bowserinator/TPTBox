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
			for (auto &el: elements) {
				bool has_diffusion = el.State == ElementState::TYPE_POWDER || el.State == ElementState::TYPE_LIQUID || el.State == ElementState::TYPE_GAS;
        		if (has_diffusion && el.Causality < el.Diffusion) {
					el.Causality = el.Diffusion;
					TraceLog(LOG_WARNING, TextFormat("Element %s has causality (%f) < diffusion (%f), setting causality to diffusion",
							el.Name.c_str(), el.Causality, el.Diffusion));
				}
			}
		}
	};

	static DoOnce doOnce;
	return doOnce.elements;
}
