#ifndef PAIR_H
#define PAIR_H

template <typename T, typename U>
class Pair {
public:
    T first;
    U second;

    Pair() : first(), second() {
    }

    Pair(const T& firstValue, const U& secondValue)
        : first(firstValue), second(secondValue) {
    }
};

#endif
