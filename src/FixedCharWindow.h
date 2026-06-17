#ifndef FIXED_CHAR_WINDOW_H
#define FIXED_CHAR_WINDOW_H

#include <stdexcept>
#include <string>

class FixedCharWindow {
private:
    char* data;
    int capacity;
    int size;
    int start;

public:
    explicit FixedCharWindow(int windowCapacity)
        : data(0), capacity(windowCapacity), size(0), start(0) {
        if (windowCapacity <= 0) {
            throw std::invalid_argument("Window capacity must be positive");
        }
        data = new char[capacity];
    }

    FixedCharWindow(const FixedCharWindow&) = delete;
    FixedCharWindow& operator=(const FixedCharWindow&) = delete;

    ~FixedCharWindow() {
        delete[] data;
    }

    void Push(char value) {
        if (size < capacity) {
            int insertIndex = (start + size) % capacity;
            data[insertIndex] = value;
            ++size;
            return;
        }

        data[start] = value;
        start = (start + 1) % capacity;
    }

    bool IsFull() const {
        return size == capacity;
    }

    std::string ToString() const {
        std::string result;
        for (int i = 0; i < size; ++i) {
            int index = (start + i) % capacity;
            result.push_back(data[index]);
        }
        return result;
    }
};

#endif
