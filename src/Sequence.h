#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <functional>
#include <stdexcept>

template <typename T>
class ArraySequence;

template <typename T>
class Sequence {
public:
    Sequence() = default;
    Sequence(const Sequence<T>& other) = default;

    virtual ~Sequence() = default;


    virtual int GetLength() const = 0;
    virtual T Get(int index) const = 0;
    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const = 0;

    virtual T GetFirst() const {
        if (GetLength() == 0) {
            throw std::out_of_range("Sequence is empty");
        }
        return Get(0);
    }

    virtual T GetLast() const {
        if (GetLength() == 0) {
            throw std::out_of_range("Sequence is empty");
        }
        return Get(GetLength() - 1);
    }

    virtual Sequence<T>* Append(const T& item) = 0;




    virtual Sequence<T>* Prepend(const T& item) = 0;
    virtual Sequence<T>* InsertAt(const T& item, int index) = 0;
    virtual Sequence<T>* Concat(const Sequence<T>& sequence) const = 0;
    virtual Sequence<T>* Clone() const = 0;

    virtual void ForEach(const std::function<void(const T&)>& action) const {
        for (int i = 0; i < GetLength(); ++i) {
            action(Get(i));
        }
    }

    template <typename TResult>


    Sequence<TResult>* Map(const std::function<TResult(const T&)>& mapper) const {

        ArraySequence<TResult>* result = new ArraySequence<TResult>();

        for (int i = 0; i < GetLength(); ++i) {

            result->Append(mapper(Get(i)));
        }
        return result;
    }

    Sequence<T>* Where(const std::function<bool(const T&)>& predicate) const {



        Sequence<T>* result = CreateEmptySameKind();
        for (int i = 0; i < GetLength(); ++i) {
            T value = Get(i);
            if (predicate(value)) {
                result->Append(value);
            }
        }
        return result;
    }

    template <typename TAcc>


    TAcc Reduce(const std::function<TAcc(TAcc, const T&)>& reducer, TAcc initial) const {
        TAcc accumulator = initial;


        for (int i = 0; i < GetLength(); ++i) {
            accumulator = reducer(accumulator, Get(i));
        }
        return accumulator;
    }

protected:
    virtual Sequence<T>* CreateEmptySameKind() const = 0;
};

#endif
