#ifndef FILE_READ_ONLY_STREAM_H
#define FILE_READ_ONLY_STREAM_H

#include "ReadOnlyStream.h"

template <typename T>
class FileReadOnlyStream : public ReadOnlyStream<T> {
public:
    FileReadOnlyStream(const std::string& path, Deserializer<T>* deserializer)
        : ReadOnlyStream<T>(path, deserializer) {
    }
};

#endif
