#ifndef FILE_WRITE_ONLY_STREAM_H
#define FILE_WRITE_ONLY_STREAM_H

#include "WriteOnlyStream.h"

template <typename T>
class FileWriteOnlyStream : public WriteOnlyStream<T> {
public:
    FileWriteOnlyStream(const std::string& path, Serializer<T>* serializer)
        : WriteOnlyStream<T>(path, serializer) {
    }
};

#endif
