#ifndef ARRAY_SEQUENCE_H
#define ARRAY_SEQUENCE_H

#include "Sequence.h"

#include <algorithm>
#include <stdexcept>

template <typename T>
class ArraySequence : public Sequence<T> {
private:
    T* items_;
    int count_;
    int capacity_;



    void EnsureCapacity(int requiredCapacity) {

        if (requiredCapacity <= capacity_) {
            return;
        }

        int newCapacity = (capacity_ == 0) ? 4 : capacity_;

        while (newCapacity < requiredCapacity) {
            newCapacity *= 2;
        }

        T* newItems = new T[newCapacity];
        for (int i = 0; i < count_; ++i) {
            newItems[i] = items_[i];
        }
        delete[] items_;
        items_ = newItems;
        capacity_ = newCapacity;
    }

    void CheckIndex(int index) const {
        if (index < 0 || index >= count_) {
            throw std::out_of_range("ArraySequence index is out of range");
        }
    }

public:


    ArraySequence() : items_(0), count_(0), capacity_(0) {
    }



    ArraySequence(const T* sourceItems, int sourceCount)
        : items_(0), count_(0), capacity_(0) {
        if (sourceCount < 0) {
            throw std::invalid_argument("ArraySequence count must be non-negative");
        }
        if (sourceCount > 0 && sourceItems == 0) {
            throw std::invalid_argument("ArraySequence source items must not be null");
        }

        EnsureCapacity(sourceCount);
        for (int i = 0; i < sourceCount; ++i) {
            items_[i] = sourceItems[i];
        }
        count_ = sourceCount;
    }


    explicit ArraySequence(int size) : items_(0), count_(0), capacity_(0) {
        if (size < 0) {
            throw std::invalid_argument("ArraySequence size must be non-negative");
        }
        EnsureCapacity(size);
        count_ = size;
    }


    ArraySequence(const ArraySequence<T>& other) : items_(0), count_(0), capacity_(0) {


        EnsureCapacity(other.count_);
        for (int i = 0; i < other.count_; ++i) {
            items_[i] = other.items_[i];
        }
        count_ = other.count_;
    }


    explicit ArraySequence(const Sequence<T>& other) : items_(0), count_(0), capacity_(0) {
        EnsureCapacity(other.GetLength());
        for (int i = 0; i < other.GetLength(); ++i) {
            items_[i] = other.Get(i);
        }
        count_ = other.GetLength();
    }


    ArraySequence<T>& operator=(const ArraySequence<T>& other) {
        if (this != &other) {
            ArraySequence<T> copy(other);
            Swap(copy);
        }
        return *this;
    }

    ~ArraySequence() override {
        delete[] items_;
    }

    void Swap(ArraySequence<T>& other) {
        std::swap(items_, other.items_);
        std::swap(count_, other.count_);
        std::swap(capacity_, other.capacity_);
    }

    int GetLength() const override {
        return count_;
    }

    T Get(int index) const override {
        CheckIndex(index);
        return items_[index];
    }

    T GetFirst() const override {
        if (IsEmpty()) {
            throw std::out_of_range("ArraySequence is empty");
        }
        return items_[0];
    }

    T GetLast() const override {
        if (IsEmpty()) {
            throw std::out_of_range("ArraySequence is empty");
        }
        return items_[count_ - 1];
    }

    void Set(int index, const T& item) {
        CheckIndex(index);
        items_[index] = item;
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || endIndex < 0 || startIndex > endIndex || endIndex >= count_) {
            throw std::out_of_range("ArraySequence subsequence range is out of range");
        }

        ArraySequence<T>* result = new ArraySequence<T>(endIndex - startIndex + 1);
        for (int i = startIndex; i <= endIndex; ++i) {
            result->Set(i - startIndex, items_[i]);
        }
        return result;
    }

    Sequence<T>* Append(const T& item) override {
        EnsureCapacity(count_ + 1);
        items_[count_] = item;
        ++count_;
        return this;
    }

    Sequence<T>* Prepend(const T& item) override {
        EnsureCapacity(count_ + 1);
        for (int i = count_; i > 0; --i) {
            items_[i] = items_[i - 1];
        }
        items_[0] = item;
        ++count_;
        return this;
    }

    Sequence<T>* InsertAt(const T& item, int index) override {
        if (index < 0 || index > count_) {
            throw std::out_of_range("ArraySequence insert index is out of range");
        }

        EnsureCapacity(count_ + 1);
        for (int i = count_; i > index; --i) {
            items_[i] = items_[i - 1];
        }
        items_[index] = item;
        ++count_;
        return this;
    }

    Sequence<T>* Concat(const Sequence<T>& sequence) const override {
        ArraySequence<T>* result = new ArraySequence<T>(count_ + sequence.GetLength());
        for (int i = 0; i < count_; ++i) {
            result->Set(i, items_[i]);
        }
        for (int i = 0; i < sequence.GetLength(); ++i) {
            result->Set(count_ + i, sequence.Get(i));
        }
        return result;
    }

    Sequence<T>* Clone() const override {
        return new ArraySequence<T>(*this);
    }

    void ForEach(const std::function<void(const T&)>& action) const override {
        for (int i = 0; i < count_; ++i) {
            action(items_[i]);
        }
    }

    bool IsEmpty() const {
        return count_ == 0;
    }

protected:
    Sequence<T>* CreateEmptySameKind() const override {
        return new ArraySequence<T>();
    }
};

#endif
