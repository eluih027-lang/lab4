#ifndef READ_ONLY_STREAM_H
#define READ_ONLY_STREAM_H

#include "ArraySequence.h"
#include "LazySequence.h"
#include "Stream.h"

#include <fstream>
#include <string>

enum ReadOnlyStreamSourceKind {
    ReadOnlyStreamEmpty,
    ReadOnlyStreamSequence,
    ReadOnlyStreamLazy,
    ReadOnlyStreamFile,
    ReadOnlyStreamProxy
};

template <typename T>
class ReadOnlyStream {

protected:
    ReadOnlyStreamSourceKind sourceKind;
    ArraySequence<T> sequenceData;
    LazySequence<T> lazyData;
    std::string filePath;
    Deserializer<T>* deserializer;
    ReadOnlyStream<T>* proxyStream;
    mutable std::ifstream input;

    mutable bool opened;
    size_t position;


    void EnsureOpen() const {
        if (sourceKind != ReadOnlyStreamFile || opened) {
            return;
        }
        input.open(filePath.c_str(), std::ios::binary);
        if (!input.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filePath);
        }
        opened = true;
    }

public:
    ReadOnlyStream()
        : sourceKind(ReadOnlyStreamEmpty),
          sequenceData(),
          lazyData(),
          filePath(),
          deserializer(0),
          proxyStream(0),
          input(),
          opened(false),
          position(0) {
    }


    explicit ReadOnlyStream(Sequence<T>* sequence)
        : sourceKind(ReadOnlyStreamSequence),
          sequenceData(),
          lazyData(),
          filePath(),
          deserializer(0),
          proxyStream(0),
          input(),
          opened(false),
          position(0) {
        if (sequence != 0) {
            for (int i = 0; i < sequence->GetLength(); ++i) {
                sequenceData.Append(sequence->Get(i));
            }
        }
    }

    explicit ReadOnlyStream(LazySequence<T>* sequence)
        : sourceKind(ReadOnlyStreamLazy),
          sequenceData(),
          lazyData(sequence == 0 ? LazySequence<T>() : *sequence),
          filePath(),
          deserializer(0),
          proxyStream(0),
          input(),
          opened(false),
          position(0) {
    }

    explicit ReadOnlyStream(const std::string& data)
        : sourceKind(ReadOnlyStreamSequence),
          sequenceData(),
          lazyData(),
          filePath(),
          deserializer(0),
          proxyStream(0),
          input(),
          opened(false),
          position(0) {
        for (size_t i = 0; i < data.size(); ++i) {
            sequenceData.Append(static_cast<T>(data[i]));
        }
    }

    ReadOnlyStream(const std::string& path, Deserializer<T>* itemDeserializer)
        : sourceKind(ReadOnlyStreamFile),
          sequenceData(),
          lazyData(),
          filePath(path),
          deserializer(itemDeserializer),
          proxyStream(0),
          input(),
          opened(false),
          position(0) {
    }

    explicit ReadOnlyStream(ReadOnlyStream<T>* stream)
        : sourceKind(ReadOnlyStreamProxy),
          sequenceData(),
          lazyData(),
          filePath(),
          deserializer(0),
          proxyStream(stream),
          input(),
          opened(false),
          position(0) {
        if (stream == 0) {
            sourceKind = ReadOnlyStreamEmpty;
        }
    }

    virtual ~ReadOnlyStream() {
        Close();
    }

    virtual bool IsEndOfStream() const {
        if (sourceKind == ReadOnlyStreamEmpty) {
            return true;
        }
        if (sourceKind == ReadOnlyStreamSequence) {
            return position >= static_cast<size_t>(sequenceData.GetLength());
        }
        if (sourceKind == ReadOnlyStreamLazy) {
            Cardinal length = lazyData.GetLength();
            if (length.IsFinite()) {
                return position >= length.Value();
            }
            return false;
        }
        if (sourceKind == ReadOnlyStreamFile) {
            EnsureOpen();
            int next = input.peek();
            return next == EOF;
        }
        return proxyStream == 0 || proxyStream->IsEndOfStream();
    }

    virtual T Read() {
        if (sourceKind == ReadOnlyStreamEmpty) {
            throw std::out_of_range("Stream is empty");
        }
        if (sourceKind == ReadOnlyStreamSequence) {
            if (position >= static_cast<size_t>(sequenceData.GetLength())) {
                throw std::out_of_range("Read after end of stream");
            }
            T result = sequenceData.Get(static_cast<int>(position));
            ++position;
            return result;
        }
        if (sourceKind == ReadOnlyStreamLazy) {
            try {
                T result = lazyData.Get(static_cast<int>(position));
                ++position;
                return result;
            } catch (const std::out_of_range&) {
                throw std::out_of_range("Read after end of lazy stream");
            }
        }
        if (sourceKind == ReadOnlyStreamFile) {
            EnsureOpen();
            if (IsEndOfStream()) {
                throw std::out_of_range("Read after end of file stream");
            }
            T result;
            if (deserializer != 0) {
                result = deserializer->Read(input);
            } else {
                input >> result;
                if (!input) {
                    throw std::runtime_error("Cannot read value from file stream");
                }
            }
            ++position;
            return result;
        }
        if (proxyStream == 0) {
            throw std::out_of_range("Proxy stream is null");
        }
        T result = proxyStream->Read();
        ++position;
        return result;
    }

    virtual size_t GetPosition() const {
        if (sourceKind == ReadOnlyStreamProxy && proxyStream != 0) {
            return proxyStream->GetPosition();
        }
        return position;
    }

    virtual bool IsCanSeek() const {
        return sourceKind == ReadOnlyStreamSequence ||
               sourceKind == ReadOnlyStreamLazy ||
               sourceKind == ReadOnlyStreamFile;
    }

    virtual size_t Seek(size_t index) {
        if (!IsCanSeek()) {
            throw std::logic_error("Stream does not support Seek");
        }
        if (sourceKind == ReadOnlyStreamSequence) {
            if (index > static_cast<size_t>(sequenceData.GetLength())) {
                throw std::out_of_range("Seek index is out of range");
            }
            position = index;
            return position;
        }
        if (sourceKind == ReadOnlyStreamLazy) {
            Cardinal length = lazyData.GetLength();
            if (length.IsFinite() && index > length.Value()) {
                throw std::out_of_range("Seek index is out of range");
            }
            position = index;
            return position;
        }

        EnsureOpen();
        input.clear();
        input.seekg(static_cast<std::streamoff>(index), std::ios::beg);
        if (!input) {
            throw std::runtime_error("Cannot seek file stream");
        }
        position = index;
        return position;
    }

    virtual bool IsCanGoBack() const {
        return IsCanSeek();
    }

    virtual void Open() {
        if (sourceKind == ReadOnlyStreamFile) {
            Close();
            input.open(filePath.c_str(), std::ios::binary);
            if (!input.is_open()) {
                throw std::runtime_error("Cannot open file for reading: " + filePath);
            }
            opened = true;
        } else if (sourceKind == ReadOnlyStreamProxy && proxyStream != 0) {
            proxyStream->Open();
        }
        position = 0;
    }

    virtual void Close() {
        if (input.is_open()) {
            input.close();
        }
        opened = false;
        if (sourceKind == ReadOnlyStreamProxy && proxyStream != 0) {
            proxyStream->Close();
        }
    }
};

#endif
