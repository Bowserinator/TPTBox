#include "ElementClasses.h"

// This is literally stolen from TPT
std::array<Element, PT_NUM> const &GetElements() {
	struct DoOnce {
		std::array<Element, PT_NUM> elements;

		DoOnce() {
#define ELEMENT_NUMBERS_CALL
#include "ElementNumbers.h"
#undef ELEMENT_NUMBERS_CALL
		}
	};

	static DoOnce doOnce;
	return doOnce.elements;
}
