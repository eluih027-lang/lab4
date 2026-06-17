#ifndef STREAM_H
#define STREAM_H

#include <cstddef>
#include <istream>
#include <ostream>
#include <stdexcept>

template <typename T>
class Deserializer {
public:
    virtual ~Deserializer() {
    }

    virtual T Read(std::istream& input) = 0;
};

template <typename T>
class Serializer {
public:
    virtual ~Serializer() {
    }

    virtual void Write(std::ostream& output, const T& value) = 0;
};

class CharDeserializer : public Deserializer<char> {
public:
    char Read(std::istream& input) override {
        char value = 0;
        input.get(value);
        if (!input) {
            throw std::runtime_error("Cannot deserialize character");
        }
        return value;
    }
};

class CharSerializer : public Serializer<char> {
public:
    void Write(std::ostream& output, const char& value) override {
        output.put(value);
        if (!output) {
            throw std::runtime_error("Cannot serialize character");
        }
    }
};

#endif
