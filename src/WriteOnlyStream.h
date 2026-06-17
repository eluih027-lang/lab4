#ifndef WRITE_ONLY_STREAM_H
#define WRITE_ONLY_STREAM_H

#include "ArraySequence.h"
#include "Stream.h"

#include <fstream>
#include <string>

enum WriteOnlyStreamTargetKind {
    WriteOnlyStreamNone,
    WriteOnlyStreamSequence,
    WriteOnlyStreamFile,
    WriteOnlyStreamProxy
};

template <typename T>
class WriteOnlyStream {
private:
    WriteOnlyStreamTargetKind targetKind;
    Sequence<T>* targetSequence;
    std::string filePath;
    Serializer<T>* serializer;
    WriteOnlyStream<T>* proxyStream;
    std::ofstream output;
    bool opened;
    size_t position;

public:
    WriteOnlyStream()
        : targetKind(WriteOnlyStreamNone),
          targetSequence(0),
          filePath(),
          serializer(0),
          proxyStream(0),
          output(),
          opened(false),
          position(0) {
    }

    explicit WriteOnlyStream(Sequence<T>* sequence)
        : targetKind(WriteOnlyStreamSequence),
          targetSequence(sequence),
          filePath(),
          serializer(0),
          proxyStream(0),
          output(),
          opened(false),
          position(0) {
        if (targetSequence == 0) {
            throw std::invalid_argument("WriteOnlyStream needs Sequence target");
        }
    }

    WriteOnlyStream(const std::string& path, Serializer<T>* itemSerializer)
        : targetKind(WriteOnlyStreamFile),
          targetSequence(0),
          filePath(path),
          serializer(itemSerializer),
          proxyStream(0),
          output(),
          opened(false),
          position(0) {
    }

    explicit WriteOnlyStream(WriteOnlyStream<T>* stream)
        : targetKind(WriteOnlyStreamProxy),
          targetSequence(0),
          filePath(),
          serializer(0),
          proxyStream(stream),
          output(),
          opened(false),
          position(0) {
        if (stream == 0) {
            targetKind = WriteOnlyStreamNone;
        }
    }

    virtual ~WriteOnlyStream() {
        Close();
    }

    virtual size_t GetPosition() const {
        if (targetKind == WriteOnlyStreamProxy && proxyStream != 0) {
            return proxyStream->GetPosition();
        }
        return position;
    }

    virtual size_t Write(T value) {
        if (targetKind == WriteOnlyStreamSequence) {
            targetSequence->Append(value);
            ++position;
            return position;
        }
        if (targetKind == WriteOnlyStreamFile) {
            if (!opened) {
                Open();
            }
            if (serializer != 0) {
                serializer->Write(output, value);
            } else {
                output << value;
                if (!output) {
                    throw std::runtime_error("Cannot write value to file stream");
                }
            }
            ++position;
            return position;
        }
        if (targetKind == WriteOnlyStreamProxy && proxyStream != 0) {
            return proxyStream->Write(value);
        }
        throw std::logic_error("WriteOnlyStream has no target");
    }

    virtual void Open() {
        if (targetKind == WriteOnlyStreamFile) {
            Close();
            output.open(filePath.c_str(), std::ios::binary | std::ios::trunc);
            if (!output.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + filePath);
            }
            opened = true;
            position = 0;
        } else if (targetKind == WriteOnlyStreamProxy && proxyStream != 0) {
            proxyStream->Open();
        }
    }

    virtual void Close() {
        if (output.is_open()) {
            output.close();
        }
        opened = false;
        if (targetKind == WriteOnlyStreamProxy && proxyStream != 0) {
            proxyStream->Close();
        }
    }
};

#endif
