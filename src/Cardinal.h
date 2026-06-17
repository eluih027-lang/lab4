#ifndef CARDINAL_H
#define CARDINAL_H

#include <cstddef>
#include <limits>
#include <stdexcept>

class Cardinal {
private:
    bool infinite;
    size_t finiteValue;

public:
    Cardinal() : infinite(false), finiteValue(0) {
    }

    explicit Cardinal(size_t value) : infinite(false), finiteValue(value) {
    }

    static Cardinal Finite(size_t value) {
        return Cardinal(value);
    }

    static Cardinal Infinite() {
        Cardinal result;
        result.infinite = true;
        result.finiteValue = std::numeric_limits<size_t>::max();
        return result;
    }

    bool IsInfinite() const {
        return infinite;
    }

    bool IsFinite() const {
        return !infinite;
    }

    size_t Value() const {
        if (infinite) {
            throw std::logic_error("Infinite cardinal has no finite value");
        }
        return finiteValue;
    }
};

#endif
