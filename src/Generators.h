#ifndef GENERATORS_H
#define GENERATORS_H

#include "ArraySequence.h"
#include "Generator.h"
#include "Sequence.h"

#include <stdexcept>
#include <string>

template <typename T>
class EmptyGenerator : public Generator<T> {
public:
    T GetNext() override {
        throw std::out_of_range("EmptyGenerator has no elements");
    }

    bool HasNext() const override {
        return false;
    }

    Cardinal GetLength() const override {
        return Cardinal::Finite(0);
    }
};

template <typename T>
class ArrayGenerator : public Generator<T> {
private:
    ArraySequence<T> items;
    int position;

public:
    ArrayGenerator(const T* sourceItems, int count) : items(sourceItems, count), position(0) {
    }

    T GetNext() override {
        if (!HasNext()) {
            throw std::out_of_range("ArrayGenerator has no next element");
        }
        T result = items.Get(position);
        ++position;
        return result;
    }

    bool HasNext() const override {
        return position < items.GetLength();
    }

    Cardinal GetLength() const override {
        return Cardinal::Finite(static_cast<size_t>(items.GetLength()));
    }
};

template <typename T>
class SequenceGenerator : public Generator<T> {
private:
    ArraySequence<T> items;
    int position;

public:
    explicit SequenceGenerator(Sequence<T>* sequence) : items(), position(0) {
        if (sequence == 0) {
            return;
        }
        for (int i = 0; i < sequence->GetLength(); ++i) {
            items.Append(sequence->Get(i));
        }
    }

    T GetNext() override {
        if (!HasNext()) {
            throw std::out_of_range("SequenceGenerator has no next element");
        }
        T result = items.Get(position);
        ++position;
        return result;
    }

    bool HasNext() const override {
        return position < items.GetLength();
    }

    Cardinal GetLength() const override {
        return Cardinal::Finite(static_cast<size_t>(items.GetLength()));
    }
};

class ArithmeticProgressionGenerator : public Generator<int> {
private:
    int current;
    int step;

public:
    ArithmeticProgressionGenerator(int first, int difference)
        : current(first), step(difference) {
    }

    int GetNext() override {
        int result = current;
        current += step;
        return result;
    }

    bool HasNext() const override {
        return true;
    }

    Cardinal GetLength() const override {
        return Cardinal::Infinite();
    }
};

class StringCharGenerator : public Generator<char> {
private:
    std::string data;
    size_t position;

public:
    explicit StringCharGenerator(const std::string& source)
        : data(source), position(0) {
    }

    char GetNext() override {
        if (!HasNext()) {
            throw std::out_of_range("StringCharGenerator has no next character");
        }
        char result = data[position];
        ++position;
        return result;
    }

    bool HasNext() const override {
        return position < data.size();
    }

    Cardinal GetLength() const override {
        return Cardinal::Finite(data.size());
    }
};

class CyclicPatternCharGenerator : public Generator<char> {
private:
    std::string pattern;
    size_t position;

public:
    explicit CyclicPatternCharGenerator(const std::string& sourcePattern)
        : pattern(sourcePattern), position(0) {
        if (pattern.empty()) {
            throw std::invalid_argument("Cyclic pattern must not be empty");
        }
    }

    char GetNext() override {
        char result = pattern[position % pattern.size()];
        ++position;
        return result;
    }

    bool HasNext() const override {
        return true;
    }

    Cardinal GetLength() const override {
        return Cardinal::Infinite();
    }
};

#endif
