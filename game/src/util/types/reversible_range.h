#ifndef UTIL_TYPES_REVERSIBLE_RANGE_H_
#define UTIL_TYPES_REVERSIBLE_RANGE_H_

#include "../iterator_tpl.h"

class ReversibleRange {
private:
    int start_val, end_val, inc;

public:
    struct it_state {
        int pos;
        inline void next(const ReversibleRange* ref) { pos += ref->inc; }
        inline void begin(const ReversibleRange* ref) { pos = ref->start_val; }
        inline void end(const ReversibleRange* ref) { pos = ref->end_val; }
        inline int& get(ReversibleRange* ref) { return pos; }
        inline const int& get(const ReversibleRange* ref) { return pos; }
        inline bool equals(const it_state& s) const { return pos == s.pos; }
    };
    SETUP_ITERATORS(ReversibleRange, int&, it_state);

    /**
     * @brief Construct a new Reversible Range object, basically a for [start, end) but reversible
     * @param start Start, < end
     * @param end End, > start
     * @param reverse  Reverse iteration order (end -> start instead)
     */
    ReversibleRange(int lo, int hi, bool reverse):
        start_val(reverse ? hi - 1 : lo), end_val(reverse ? lo - 1 : hi), inc(reverse ? -1 : 1) {}
};

#endif // UTIL_TYPES_REVERSIBLE_RANGE_H_
