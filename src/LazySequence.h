#ifndef LAZY_SEQUENCE_H
#define LAZY_SEQUENCE_H

#include "ArraySequence.h"
#include "Generators.h"
#include "Pair.h"

#include <memory>
#include <stdexcept>

template <typename T>
class LazySequence;

template <typename T>
class SubsequenceGenerator;

template <typename T>
class ConcatGenerator;

template <typename T>
class InsertAtGenerator;

template <typename T, typename U, typename Mapper>
class MapGenerator;

template <typename T, typename Predicate>
class WhereGenerator;

template <typename T, typename U>
class ZipGenerator;

template <typename T>
class Interleave3Generator;

template <typename T>
class LazySequenceState {
public:
    std::shared_ptr<Generator<T>> generator;
    ArraySequence<T> materializedItems;
    Cardinal length;
    bool exhausted;

    LazySequenceState()
        : generator(std::shared_ptr<Generator<T>> (new EmptyGenerator<T>())),

          materializedItems(),
          length(Cardinal::Finite(0)),
          exhausted(false) {
    }

    explicit LazySequenceState(Generator<T>* sourceGenerator)
        : generator(std::shared_ptr<Generator<T>> (sourceGenerator)),
          materializedItems(),
          length(sourceGenerator == 0 ? Cardinal::Finite(0) : sourceGenerator->GetLength()),
          exhausted(false) {
        if (sourceGenerator == 0) {
            generator = std::shared_ptr<Generator<T> >(new EmptyGenerator<T>());
        }
    }
};

template <typename T>
class LazySequence {
private:
    std::shared_ptr<LazySequenceState<T> > state;


    void MaterializeTo(size_t index) const {
        while (state->materializedItems.GetLength() <= index) {
            if (state->exhausted || !state->generator->HasNext()) {
                state->exhausted = true;
                if (state->length.IsInfinite()) {
                    state->length = Cardinal::Finite(
                        static_cast<size_t>(state->materializedItems.GetLength()));
                }
                throw std::out_of_range("LazySequence index is out of range");
            }
            state->materializedItems.Append(state->generator->GetNext());
        }
    }

public:
    LazySequence()
        : state(std::shared_ptr<LazySequenceState<T> >(new LazySequenceState<T>())) {
    }

    LazySequence(T* items, int count)
        : state(std::shared_ptr<LazySequenceState<T> >(
              new LazySequenceState<T>(new ArrayGenerator<T>(items, count)))) {
    }

    explicit LazySequence(Sequence<T>* sequence)
        : state(std::shared_ptr<LazySequenceState<T> >(
              new LazySequenceState<T>(new SequenceGenerator<T>(sequence)))) {
    }

    explicit LazySequence(Generator<T>* generator)
        : state(std::shared_ptr<LazySequenceState<T> >(new LazySequenceState<T>(generator))) {
    }

    LazySequence(const LazySequence<T>& other) : state(other.state) {
    }

    LazySequence<T>& operator=(const LazySequence<T>& other) {
        if (this != &other) {
            state = other.state;
        }
        return *this;
    }

    T GetFirst() const {
        return Get(0);
    }

    T GetLast() const {
        Cardinal currentLength = GetLength();
        if (currentLength.IsInfinite()) {
            throw std::logic_error("Infinite LazySequence has no last element");
        }
        if (currentLength.Value() == 0) {
            throw std::out_of_range("LazySequence is empty");
        }
        return Get(static_cast<int>(currentLength.Value() - 1));
    }

    T Get(int index) const {
    if (index < 0) throw std::out_of_range("Index out of range: negative index");
    MaterializeTo(index);
    if (index >= state->materializedItems.GetLength()) {
        throw std::out_of_range("Index out of range");
    }
    return state->materializedItems.Get(index);
    }

    LazySequence<T>* GetSubsequence(int startIndex, int endIndex) const {
        if (startIndex < 0 || endIndex < startIndex) {
            throw std::out_of_range("Invalid subsequence indexes");
        }
        Cardinal currentLength = GetLength();
        if (currentLength.IsFinite() && static_cast<size_t>(endIndex) >= currentLength.Value()) {
            throw std::out_of_range("Subsequence end is out of range");
        }
        return new LazySequence<T>(
            new SubsequenceGenerator<T>(*this, static_cast<size_t>(startIndex), static_cast<size_t>(endIndex)));
    }

    Cardinal GetLength() const {
        return state->length;
    }

    size_t GetMaterializedCount() const {
        return static_cast<size_t>(state->materializedItems.GetLength());
    }

    LazySequence<T>* Append(const T& item) const {


        LazySequence<T> single(const_cast<T*>(&item), 1);
        return Concat(&single);
    }

    LazySequence<T>* Prepend(const T& item) const {
        LazySequence<T> single(const_cast<T*>(&item), 1);
        return single.Concat(const_cast<LazySequence<T>*>(this));
    }

    LazySequence<T>* InsertAt(const T& item, int index) const {
        if (index < 0) {
            throw std::out_of_range("Insert index must be non-negative");
        }
        Cardinal currentLength = GetLength();
        if (currentLength.IsFinite() && static_cast<size_t>(index) >= currentLength.Value()) {
            throw std::out_of_range("Insert index is out of range");
        }
        return new LazySequence<T>(
            new InsertAtGenerator<T>(*this, item, static_cast<size_t>(index)));
    }

    LazySequence<T>* Concat(LazySequence<T>* other) const {
        if (other == 0) {
            throw std::invalid_argument("Other LazySequence must not be null");
        }

        return new LazySequence<T>(new ConcatGenerator<T>(*this, *other));
    }

    static LazySequence<T>* Interleave3(LazySequence<T>* first,
                                        LazySequence<T>* second,
                                        LazySequence<T>* third) {
        if (first == 0 || second == 0 || third == 0) {
            throw std::invalid_argument("Interleave3 arguments must not be null");
        }

        return new LazySequence<T>(new Interleave3Generator<T>(*first, *second, *third));
    }

    template <typename U, typename Mapper>
    LazySequence<U>* Map(Mapper mapper) const {
        return new LazySequence<U>(new MapGenerator<T, U, Mapper>(*this, mapper));
    }

    template <typename Predicate>
    LazySequence<T>* Where(Predicate predicate) const {
        return new LazySequence<T>(new WhereGenerator<T, Predicate>(*this, predicate));
    }

    template <typename U, typename Reducer>
    U Reduce(U initialValue, Reducer reducer) const {
        Cardinal currentLength = GetLength();
        if (currentLength.IsInfinite()) {
            throw std::logic_error("Reduce without a limit is not allowed for infinite LazySequence");
        }

        U accumulator = initialValue;
        for (size_t i = 0; i < currentLength.Value(); ++i) {
            accumulator = reducer(accumulator, Get(static_cast<int>(i)));
        }
        return accumulator;
    }

    template <typename U>
    LazySequence<Pair<T, U> >* Zip(LazySequence<U>* other) const {
        if (other == 0) {
            throw std::invalid_argument("Other LazySequence must not be null");
        }
        return new LazySequence<Pair<T, U> >(new ZipGenerator<T, U>(*this, *other));
    }
};

template <typename T>
class SubsequenceGenerator : public Generator<T> {
private:
    LazySequence<T> source;
    size_t currentIndex;
    size_t endIndex;
    Cardinal length;

public:
    SubsequenceGenerator(const LazySequence<T>& sourceSequence, size_t start, size_t end)
        : source(sourceSequence),
          currentIndex(start),
          endIndex(end),
          length(Cardinal::Finite(end - start + 1)) {
    }

    T GetNext() override {
        if (!HasNext()) {
            throw std::out_of_range("SubsequenceGenerator has no next element");
        }
        T result = source.Get(static_cast<int>(currentIndex));
        ++currentIndex;
        return result;
    }

    bool HasNext() const override {
        return currentIndex <= endIndex;
    }

    Cardinal GetLength() const override {
        return length;
    }
};

template <typename T>
class ConcatGenerator : public Generator<T> {
private:
    LazySequence<T> first;
    LazySequence<T> second;
    size_t firstIndex;
    size_t secondIndex;
    mutable bool firstExhausted;
    Cardinal length;

public:
    ConcatGenerator(const LazySequence<T>& firstSequence, const LazySequence<T>& secondSequence)
        : first(firstSequence),
          second(secondSequence),
          firstIndex(0),
          secondIndex(0),
          firstExhausted(false),
          length(Cardinal::Infinite()) {
        Cardinal firstLength = first.GetLength();
        Cardinal secondLength = second.GetLength();
        if (firstLength.IsFinite() && secondLength.IsFinite()) {
            length = Cardinal::Finite(firstLength.Value() + secondLength.Value());
        }
    }

    T GetNext() override {
        if (!firstExhausted) {
            try {
                T result = first.Get(static_cast<int>(firstIndex));
                ++firstIndex;
                return result;
            } catch (const std::out_of_range&) {
                firstExhausted = true;
            }
        }

        try {
            T result = second.Get(static_cast<int>(secondIndex));
            ++secondIndex;
            return result;
        } catch (const std::out_of_range&) {
            throw std::out_of_range("ConcatGenerator has no next element");
        }
    }

    bool HasNext() const override {
        if (!firstExhausted) {
            Cardinal firstLength = first.GetLength();
            if (firstLength.IsInfinite()) {
                return true;
            }
            if (firstIndex < firstLength.Value()) {
                return true;
            }
            firstExhausted = true;
        }

        Cardinal secondLength = second.GetLength();
        if (secondLength.IsInfinite()) {
            return true;
        }
        return secondIndex < secondLength.Value();
    }

    Cardinal GetLength() const override {
        return length;
    }
};

template <typename T>
class InsertAtGenerator : public Generator<T> {
private:
    LazySequence<T> source;
    T insertedItem;
    size_t insertIndex;
    size_t position;
    Cardinal length;

public:
    InsertAtGenerator(const LazySequence<T>& sourceSequence, const T& item, size_t index)
        : source(sourceSequence),
          insertedItem(item),
          insertIndex(index),
          position(0),
          length(Cardinal::Infinite()) {
        Cardinal sourceLength = source.GetLength();
        if (sourceLength.IsFinite()) {
            length = Cardinal::Finite(sourceLength.Value() + 1);
        }
    }

    T GetNext() override {
        if (!HasNext()) {
            throw std::out_of_range("InsertAtGenerator has no next element");
        }

        if (position == insertIndex) {
            ++position;
            return insertedItem;
        }

        size_t sourceIndex = position;
        if (position > insertIndex) {
            sourceIndex = position - 1;
        }
        ++position;
        return source.Get(static_cast<int>(sourceIndex));
    }

    bool HasNext() const override {
        if (length.IsInfinite()) {
            return true;
        }
        return position < length.Value();
    }

    Cardinal GetLength() const override {
        return length;
    }
};

template <typename T, typename U, typename Mapper>
class MapGenerator : public Generator<U> {
private:
    LazySequence<T> source;
    Mapper mapper;
    size_t position;
    Cardinal length;

public:
    MapGenerator(const LazySequence<T>& sourceSequence, Mapper sourceMapper)
        : source(sourceSequence), mapper(sourceMapper), position(0), length(sourceSequence.GetLength()) {
    }

    U GetNext() override {
        if (!HasNext()) {
            throw std::out_of_range("MapGenerator has no next element");
        }
        T value = source.Get(static_cast<int>(position));
        ++position;
        return mapper(value);
    }

    bool HasNext() const override {
        if (length.IsInfinite()) {
            return true;
        }
        return position < length.Value();
    }

    Cardinal GetLength() const override {
        return length;
    }
};

template <typename T, typename Predicate>
class WhereGenerator : public Generator<T> {
private:
    LazySequence<T> source;
    Predicate predicate;
    mutable size_t sourceIndex;
    mutable Optional<T> bufferedItem;

    bool PrepareNext() const {
        if (bufferedItem.HasValue()) {
            return true;
        }

        while (true) {
            Cardinal sourceLength = source.GetLength();
            if (sourceLength.IsFinite() && sourceIndex >= sourceLength.Value()) {
                return false;
            }

            try {
                T value = source.Get(static_cast<int>(sourceIndex));
                ++sourceIndex;

                if (predicate(value)) {
                    bufferedItem = Optional<T>(value);
                    return true;
                }
            } catch (const std::out_of_range&) {
                return false;
            }
        }
    }

public:
    WhereGenerator(const LazySequence<T>& sourceSequence, Predicate sourcePredicate)
        : source(sourceSequence), predicate(sourcePredicate), sourceIndex(0), bufferedItem() {
    }

    T GetNext() override {
        if (!PrepareNext()) {
            throw std::out_of_range("WhereGenerator has no next element");
        }
        T result = bufferedItem.GetValue();
        bufferedItem.Reset();
        return result;
    }

    bool HasNext() const override {
        return PrepareNext();
    }

    Cardinal GetLength() const override {
        Cardinal sourceLength = source.GetLength();
        if (sourceLength.IsInfinite()) {
            return Cardinal::Infinite();
        }

        size_t result = 0;
        for (size_t i = 0; i < sourceLength.Value(); ++i) {
            if (predicate(source.Get(static_cast<int>(i)))) {
                ++result;
            }
        }
        return Cardinal(result);
    }
};

template <typename T, typename U>
class ZipGenerator : public Generator<Pair<T, U> > {
private:
    LazySequence<T> first;
    LazySequence<U> second;
    size_t position;
    Cardinal length;

public:
    ZipGenerator(const LazySequence<T>& firstSequence, const LazySequence<U>& secondSequence)
        : first(firstSequence), second(secondSequence), position(0), length(Cardinal::Infinite()) {
        Cardinal firstLength = first.GetLength();
        Cardinal secondLength = second.GetLength();
        if (firstLength.IsFinite() && secondLength.IsFinite()) {
            size_t minLength = firstLength.Value() < secondLength.Value()
                                   ? firstLength.Value()
                                   : secondLength.Value();
            length = Cardinal::Finite(minLength);
        } else if (firstLength.IsFinite()) {
            length = Cardinal::Finite(firstLength.Value());
        } else if (secondLength.IsFinite()) {
            length = Cardinal::Finite(secondLength.Value());
        }
    }

    Pair<T, U> GetNext() override {
        if (!HasNext()) {
            throw std::out_of_range("ZipGenerator has no next element");
        }
        Pair<T, U> result(first.Get(static_cast<int>(position)),
                          second.Get(static_cast<int>(position)));
        ++position;
        return result;
    }

    bool HasNext() const override {
        if (length.IsInfinite()) {
            return true;
        }
        return position < length.Value();
    }

    Cardinal GetLength() const override {
        return length;
    }
};

template <typename T>
class Interleave3Generator : public Generator<T> {
private:
    LazySequence<T> first;
    LazySequence<T> second;
    LazySequence<T> third;
    size_t row;
    int phase;
    size_t produced;
    Cardinal length;

    bool HasElementAt(const LazySequence<T>& sequence, size_t index) const {
        Cardinal sequenceLength = sequence.GetLength();
        if (sequenceLength.IsInfinite()) {
            return true;
        }
        return index < sequenceLength.Value();
    }

    void MoveToNextSource() {
        ++phase;
        if (phase == 3) {
            phase = 0;
            ++row;
        }
    }

public:
    Interleave3Generator(const LazySequence<T>& firstSequence,
                         const LazySequence<T>& secondSequence,
                         const LazySequence<T>& thirdSequence)
        : first(firstSequence),
          second(secondSequence),
          third(thirdSequence),
          row(0),
          phase(0),
          produced(0),
          length(Cardinal::Infinite()) {
        Cardinal firstLength = first.GetLength();
        Cardinal secondLength = second.GetLength();
        Cardinal thirdLength = third.GetLength();
        if (firstLength.IsFinite() && secondLength.IsFinite() && thirdLength.IsFinite()) {
            length = Cardinal::Finite(firstLength.Value() + secondLength.Value() + thirdLength.Value());
        }
    }

    T GetNext() override {
        while (HasNext()) {
            LazySequence<T>* current = &first;
            if (phase == 1) {
                current = &second;
            } else if (phase == 2) {
                current = &third;
            }

            size_t currentRow = row;
            MoveToNextSource();

            if (!HasElementAt(*current, currentRow)) {
                continue;
            }

            try {
                T result = current->Get(static_cast<int>(currentRow));
                ++produced;
                return result;
            } catch (const std::out_of_range&) {
                continue;
            }
        }
        throw std::out_of_range("Interleave3Generator has no next element");
    }

    bool HasNext() const override {
        if (length.IsInfinite()) {
            return true;
        }
        return produced < length.Value();
    }

    Cardinal GetLength() const override {
        return length;
    }
};

#endif
