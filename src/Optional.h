#ifndef OPTIONAL_H
#define OPTIONAL_H

#include <optional>
#include <stdexcept>

template <typename T>
class Optional {
private:
    std::optional<T> value;

public:
    Optional() : value(std::nullopt) {
    }

    Optional(const T& item) : value(item) {
    }

    bool HasValue() const {
        return value.has_value();
    }

    T& GetValue() {
        if (!value.has_value()) {
            throw std::logic_error("Optional has no value");
        }
        return value.value();
    }

    const T& GetValue() const {
        if (!value.has_value()) {
            throw std::logic_error("Optional has no value");
        }
        return value.value();
    }

    void Reset() {
        value.reset();
    }
};

#endif
