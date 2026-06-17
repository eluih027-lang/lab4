#ifndef SUBSTRING_FREQUENCY_COUNTER_H
#define SUBSTRING_FREQUENCY_COUNTER_H

#include "ArraySequence.h"
#include "BinarySearchTree.h"
#include "FixedCharWindow.h"
#include "LazySequence.h"
#include "Pair.h"
#include "ReadOnlyStream.h"

#include <string>

class SubstringFrequencyCounter {
private:
    BinarySearchTree<std::string, int> frequencies;
    size_t totalProcessedCharacters;
    size_t totalSubstrings;

    void ValidateK(int k) const;
    void ProcessCharacter(char value, FixedCharWindow& window);
    void IncrementFrequency(const std::string& substring);

public:
    SubstringFrequencyCounter();

    void ProcessStream(ReadOnlyStream<char>* stream, int k);
    void ProcessLazySequence(LazySequence<char>* sequence, int k, size_t limit);

    size_t GetTotalProcessedCharacters() const;
    size_t GetTotalSubstrings() const;
    int GetFrequency(std::string substring) const;
    ArraySequence<Pair<std::string, int> > GetAllFrequencies() const;
    size_t GetUniqueSubstringCount() const;
    void Clear();
};

#endif
