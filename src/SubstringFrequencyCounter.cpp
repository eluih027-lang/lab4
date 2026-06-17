#include "SubstringFrequencyCounter.h"

#include <stdexcept>

SubstringFrequencyCounter::SubstringFrequencyCounter()
    : frequencies(), totalProcessedCharacters(0), totalSubstrings(0) {
}

void SubstringFrequencyCounter::ValidateK(int k) const {
    if (k <= 0) {
        throw std::invalid_argument("Substring length k must be positive");
    }
}

void SubstringFrequencyCounter::IncrementFrequency(const std::string& substring) {
    frequencies.Increment(substring, 1);
}

void SubstringFrequencyCounter::ProcessCharacter(char value, FixedCharWindow& window) {
    ++totalProcessedCharacters;
    window.Push(value);



    if (window.IsFull()) {
        IncrementFrequency(window.ToString());
        ++totalSubstrings;
    }
}

void SubstringFrequencyCounter::ProcessStream(ReadOnlyStream<char>* stream, int k) {
    if (stream == 0) {
        throw std::invalid_argument("Stream must not be null");
    }
    ValidateK(k);
    Clear();

    FixedCharWindow window(k);
    while (!stream->IsEndOfStream()) {
        char value = stream->Read();
        ProcessCharacter(value, window);
    }
}

void SubstringFrequencyCounter::ProcessLazySequence(LazySequence<char>* sequence, int k, size_t limit) {
    if (sequence == 0) {
        throw std::invalid_argument("LazySequence must not be null");
    }
    ValidateK(k);
    Clear();

    ReadOnlyStream<char> stream(sequence);
    FixedCharWindow window(k);
    size_t readCount = 0;
    while (readCount < limit && !stream.IsEndOfStream()) {
        try {
            char value = stream.Read();
            ProcessCharacter(value, window);
            ++readCount;
        } catch (const std::out_of_range&) {
            break;
        }
    }
}

size_t SubstringFrequencyCounter::GetTotalProcessedCharacters() const {
    return totalProcessedCharacters;
}

size_t SubstringFrequencyCounter::GetTotalSubstrings() const {
    return totalSubstrings;
}

int SubstringFrequencyCounter::GetFrequency(std::string substring) const {
    return frequencies.GetOrDefault(substring, 0);
}

ArraySequence<Pair<std::string, int> > SubstringFrequencyCounter::GetAllFrequencies() const {
    return frequencies.ToArrayInOrder();
}

size_t SubstringFrequencyCounter::GetUniqueSubstringCount() const {
    return frequencies.GetCount();
}

void SubstringFrequencyCounter::Clear() {
    frequencies.Clear();
    totalProcessedCharacters = 0;
    totalSubstrings = 0;
}
