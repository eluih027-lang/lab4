#ifndef GENERATOR_H
#define GENERATOR_H

#include "Cardinal.h"
#include "Optional.h"

#include <stdexcept>

template <typename T>
class Generator {
public:
    virtual ~Generator() {
    }

    virtual T GetNext() = 0;
    virtual bool HasNext() const = 0;

    virtual Optional<T> TryGetNext() {
        if (!HasNext()) {
            return Optional<T>();
        }
        return Optional<T>(GetNext());
    }

    virtual Cardinal GetLength() const {
        return Cardinal::Infinite();
    }
};

#endif
