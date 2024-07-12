#ifndef UTIL_TYPES_REVERSIBLE_RANGE_H_
#define UTIL_TYPES_REVERSIBLE_RANGE_H_

/// Usage: for (ReversibleRange r1(start, end, val, reverse); r1.has_next(); val = r1.next())
/// We don't use iterators because we could potentially start with a range that's "beyond"
/// the iterator's end (ie r1(100, 10), we would increment 100 forever never reaching 10)
class ReversibleRange {
private:
    int start_val, end_val, inc;
    int m_pos;

public:
    /** @brief Has a new value? */
    bool has_next() {
        if (inc > 0) return m_pos + 1 <= end_val;
        return m_pos - 1 >= end_val;
    }

    /**
     * @brief Increment pos and return incremented value
     * @return int 
     */
    int next() {
        m_pos += inc;
        return m_pos;
    }

    /**
     * @brief Construct a new Reversible Range object, basically a for [start, end) but reversible
     * @param start Start, < end
     * @param end End, > start
     * @param val Val to initialize to m_pos
     * @param reverse  Reverse iteration order (end -> start instead)
     */
    ReversibleRange(int lo, int hi, auto &val, bool reverse):
            start_val(reverse ? hi - 1 : lo), end_val(reverse ? lo - 1 : hi), inc(reverse ? -1 : 1) {
        val = start_val;
        m_pos = start_val;
    }
};

#endif // UTIL_TYPES_REVERSIBLE_RANGE_H_
