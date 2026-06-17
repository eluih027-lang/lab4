#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <stdexcept>

#include "ArraySequence.h"
#include "BinarySearchTree.h"
#include "Cardinal.h"
#include "FixedCharWindow.h"
#include "Generators.h"
#include "LazySequence.h"
#include "Optional.h"
#include "Pair.h"
#include "ReadOnlyStream.h"
#include "WriteOnlyStream.h"
#include "FileReadOnlyStream.h"
#include "FileWriteOnlyStream.h"
#include "Stream.h"
#include "SubstringFrequencyCounter.h"

TEST(CardinalTest, FiniteValue) {
    Cardinal c = Cardinal::Finite(42);
    EXPECT_TRUE(c.IsFinite());
    EXPECT_FALSE(c.IsInfinite());
    EXPECT_EQ(c.Value(), 42);
}

TEST(CardinalTest, InfiniteValue) {
    Cardinal c = Cardinal::Infinite();
    EXPECT_FALSE(c.IsFinite());
    EXPECT_TRUE(c.IsInfinite());
    EXPECT_THROW(c.Value(), std::logic_error);
}

TEST(CardinalTest, ZeroIsFinite) {
    Cardinal c = Cardinal::Finite(0);
    EXPECT_TRUE(c.IsFinite());
    EXPECT_EQ(c.Value(), 0);
}

TEST(OptionalTest, EmptyOptional) {
    Optional<int> o;
    EXPECT_FALSE(o.HasValue());
    EXPECT_THROW(o.GetValue(), std::logic_error);
}

TEST(OptionalTest, ValueOptional) {
    Optional<int> o(10);
    EXPECT_TRUE(o.HasValue());
    EXPECT_EQ(o.GetValue(), 10);
    o.Reset();
    EXPECT_FALSE(o.HasValue());
}

TEST(PairTest, DefaultConstructor) {
    Pair<int, std::string> p;
    p.first = 5;
    p.second = "test";
    EXPECT_EQ(p.first, 5);
    EXPECT_EQ(p.second, "test");
}

TEST(PairTest, ParameterizedConstructor) {
    Pair<int, std::string> p(10, "hello");
    EXPECT_EQ(p.first, 10);
    EXPECT_EQ(p.second, "hello");
}

TEST(ArraySequenceTest, CreateEmpty) {
    ArraySequence<int> seq;
    EXPECT_EQ(seq.GetLength(), 0);
    EXPECT_TRUE(seq.IsEmpty());
}

TEST(ArraySequenceTest, CreateWithSize) {
    ArraySequence<int> seq(5);
    EXPECT_EQ(seq.GetLength(), 5);
    EXPECT_FALSE(seq.IsEmpty());
}

TEST(ArraySequenceTest, CreateFromArray) {
    int arr[] = {1, 2, 3};
    ArraySequence<int> seq(arr, 3);
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.Get(0), 1);
    EXPECT_EQ(seq.Get(2), 3);
}

TEST(ArraySequenceTest, CreateFromNullArrayThrows) {
    EXPECT_THROW(ArraySequence<int>(nullptr, 5), std::invalid_argument);
}

TEST(ArraySequenceTest, CreateWithNegativeCountThrows) {
    EXPECT_THROW(ArraySequence<int>((int*)nullptr, -1), std::invalid_argument);
}

TEST(ArraySequenceTest, GetFirstLast) {
    int arr[] = {10, 20, 30};
    ArraySequence<int> seq(arr, 3);
    EXPECT_EQ(seq.GetFirst(), 10);
    EXPECT_EQ(seq.GetLast(), 30);
}

TEST(ArraySequenceTest, GetFirstOnEmptyThrows) {
    ArraySequence<int> seq;
    EXPECT_THROW(seq.GetFirst(), std::out_of_range);
}

TEST(ArraySequenceTest, GetLastOnEmptyThrows) {
    ArraySequence<int> seq;
    EXPECT_THROW(seq.GetLast(), std::out_of_range);
}

TEST(ArraySequenceTest, GetOutOfRangeThrows) {
    ArraySequence<int> seq(3);
    EXPECT_THROW(seq.Get(-1), std::out_of_range);
    EXPECT_THROW(seq.Get(3), std::out_of_range);
}

TEST(ArraySequenceTest, SetAndGet) {
    ArraySequence<int> seq(3);
    seq.Set(0, 100);
    seq.Set(2, 300);
    EXPECT_EQ(seq.Get(0), 100);
    EXPECT_EQ(seq.Get(2), 300);
}

TEST(ArraySequenceTest, GetSubsequence) {
    int arr[] = {0, 1, 2, 3, 4};
    ArraySequence<int> seq(arr, 5);
    Sequence<int>* sub = seq.GetSubsequence(1, 3);
    EXPECT_EQ(sub->GetLength(), 3);
    EXPECT_EQ(sub->Get(0), 1);
    EXPECT_EQ(sub->Get(2), 3);
    delete sub;
}

TEST(ArraySequenceTest, GetSubsequenceInvalidRangeThrows) {
    int arr[] = {0, 1, 2};
    ArraySequence<int> seq(arr, 3);
    EXPECT_THROW(seq.GetSubsequence(-1, 1), std::out_of_range);
    EXPECT_THROW(seq.GetSubsequence(1, 0), std::out_of_range);
    EXPECT_THROW(seq.GetSubsequence(0, 3), std::out_of_range);
}

TEST(ArraySequenceTest, Append) {
    ArraySequence<int> seq;
    seq.Append(1);
    seq.Append(2);
    EXPECT_EQ(seq.GetLength(), 2);
    EXPECT_EQ(seq.Get(0), 1);
    EXPECT_EQ(seq.Get(1), 2);
}

TEST(ArraySequenceTest, Prepend) {
    ArraySequence<int> seq;
    seq.Append(2);
    seq.Prepend(1);
    EXPECT_EQ(seq.GetLength(), 2);
    EXPECT_EQ(seq.Get(0), 1);
    EXPECT_EQ(seq.Get(1), 2);
}

TEST(ArraySequenceTest, InsertAt) {
    ArraySequence<int> seq;
    seq.Append(1);
    seq.Append(3);
    seq.InsertAt(2, 1);
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.Get(0), 1);
    EXPECT_EQ(seq.Get(1), 2);
    EXPECT_EQ(seq.Get(2), 3);
}

TEST(ArraySequenceTest, InsertAtInvalidIndexThrows) {
    ArraySequence<int> seq(2);
    EXPECT_THROW(seq.InsertAt(0, -1), std::out_of_range);
    EXPECT_THROW(seq.InsertAt(0, 3), std::out_of_range);
}

TEST(ArraySequenceTest, Concat) {
    int a[] = {1, 2};
    int b[] = {3, 4};
    ArraySequence<int> first(a, 2);
    ArraySequence<int> second(b, 2);
    Sequence<int>* result = first.Concat(second);
    EXPECT_EQ(result->GetLength(), 4);
    EXPECT_EQ(result->Get(0), 1);
    EXPECT_EQ(result->Get(3), 4);
    delete result;
}

TEST(ArraySequenceTest, Clone) {
    int arr[] = {1, 2, 3};
    ArraySequence<int> seq(arr, 3);
    Sequence<int>* clone = seq.Clone();
    EXPECT_EQ(clone->GetLength(), 3);
    EXPECT_EQ(clone->Get(0), 1);
    delete clone;
}

TEST(ArraySequenceTest, ForEach) {
    int arr[] = {1, 2, 3};
    ArraySequence<int> seq(arr, 3);
    int sum = 0;
    seq.ForEach([&sum](const int& v) { sum += v; });
    EXPECT_EQ(sum, 6);
}

TEST(ArraySequenceTest, Map) {
    int arr[] = {1, 2, 3};
    ArraySequence<int> seq(arr, 3);
    Sequence<int>* doubled = seq.Map<int>([](const int& v) { return v * 2; });
    EXPECT_EQ(doubled->Get(0), 2);
    EXPECT_EQ(doubled->Get(1), 4);
    EXPECT_EQ(doubled->Get(2), 6);
    delete doubled;
}

TEST(ArraySequenceTest, Where) {
    int arr[] = {1, 2, 3, 4, 5};
    ArraySequence<int> seq(arr, 5);
    Sequence<int>* even = seq.Where([](const int& v) { return v % 2 == 0; });
    EXPECT_EQ(even->GetLength(), 2);
    EXPECT_EQ(even->Get(0), 2);
    EXPECT_EQ(even->Get(1), 4);
    delete even;
}

TEST(ArraySequenceTest, Reduce) {
    int arr[] = {1, 2, 3};
    ArraySequence<int> seq(arr, 3);
    int sum = seq.Reduce<int>([](int acc, const int& v) { return acc + v; }, 0);
    EXPECT_EQ(sum, 6);
}

TEST(BSTTest, EmptyTree) {
    BinarySearchTree<std::string, int> tree;
    EXPECT_EQ(tree.GetCount(), 0);
    EXPECT_FALSE(tree.Contains("key"));
    EXPECT_EQ(tree.GetOrDefault("key", -1), -1);
}

TEST(BSTTest, AddOrUpdate) {
    BinarySearchTree<std::string, int> tree;
    tree.AddOrUpdate("a", 1);
    EXPECT_EQ(tree.GetCount(), 1);
    EXPECT_TRUE(tree.Contains("a"));
    EXPECT_EQ(tree.GetOrDefault("a", 0), 1);
    tree.AddOrUpdate("a", 5);
    EXPECT_EQ(tree.GetCount(), 1);
    EXPECT_EQ(tree.GetOrDefault("a", 0), 5);
}

TEST(BSTTest, Increment) {
    BinarySearchTree<std::string, int> tree;
    tree.Increment("x", 1);
    EXPECT_EQ(tree.GetOrDefault("x", 0), 1);
    tree.Increment("x", 2);
    EXPECT_EQ(tree.GetOrDefault("x", 0), 3);
    tree.Increment("y", 10);
    EXPECT_EQ(tree.GetOrDefault("y", 0), 10);
}

TEST(BSTTest, ToArrayInOrder) {
    BinarySearchTree<int, std::string> tree;
    tree.AddOrUpdate(2, "two");
    tree.AddOrUpdate(1, "one");
    tree.AddOrUpdate(3, "three");
    auto arr = tree.ToArrayInOrder();
    EXPECT_EQ(arr.GetLength(), 3);
    EXPECT_EQ(arr.Get(0).first, 1);
    EXPECT_EQ(arr.Get(1).first, 2);
    EXPECT_EQ(arr.Get(2).first, 3);
}

TEST(BSTTest, Clear) {
    BinarySearchTree<int, int> tree;
    tree.AddOrUpdate(1, 1);
    tree.Clear();
    EXPECT_EQ(tree.GetCount(), 0);
    EXPECT_FALSE(tree.Contains(1));
}

TEST(FixedCharWindowTest, InvalidCapacityThrows) {
    EXPECT_THROW(FixedCharWindow(0), std::invalid_argument);
    EXPECT_THROW(FixedCharWindow(-1), std::invalid_argument);
}

TEST(FixedCharWindowTest, NotFullInitially) {
    FixedCharWindow w(3);
    EXPECT_FALSE(w.IsFull());
    EXPECT_EQ(w.ToString(), "");
}

TEST(FixedCharWindowTest, PushAndToString) {
    FixedCharWindow w(3);
    w.Push('a');
    w.Push('b');
    EXPECT_EQ(w.ToString(), "ab");
    w.Push('c');
    EXPECT_TRUE(w.IsFull());
    EXPECT_EQ(w.ToString(), "abc");
}

TEST(FixedCharWindowTest, OverwriteWhenFull) {
    FixedCharWindow w(3);
    w.Push('a');
    w.Push('b');
    w.Push('c');
    w.Push('d');
    EXPECT_EQ(w.ToString(), "bcd");
    w.Push('e');
    EXPECT_EQ(w.ToString(), "cde");
}

TEST(EmptyGeneratorTest, HasNextIsFalse) {
    EmptyGenerator<int> gen;
    EXPECT_FALSE(gen.HasNext());
    EXPECT_THROW(gen.GetNext(), std::out_of_range);
    auto opt = gen.TryGetNext();
    EXPECT_FALSE(opt.HasValue());
}

TEST(EmptyGeneratorTest, LengthIsZero) {
    EmptyGenerator<int> gen;
    Cardinal len = gen.GetLength();
    EXPECT_TRUE(len.IsFinite());
    EXPECT_EQ(len.Value(), 0);
}

TEST(ArrayGeneratorTest, BasicIteration) {
    int data[] = {10, 20, 30};
    ArrayGenerator<int> gen(data, 3);
    EXPECT_TRUE(gen.HasNext());
    EXPECT_EQ(gen.GetNext(), 10);
    EXPECT_EQ(gen.GetNext(), 20);
    EXPECT_EQ(gen.GetNext(), 30);
    EXPECT_FALSE(gen.HasNext());
}

TEST(ArrayGeneratorTest, TryGetNext) {
    int data[] = {5};
    ArrayGenerator<int> gen(data, 1);
    auto opt = gen.TryGetNext();
    EXPECT_TRUE(opt.HasValue());
    EXPECT_EQ(opt.GetValue(), 5);
    auto opt2 = gen.TryGetNext();
    EXPECT_FALSE(opt2.HasValue());
}

TEST(ArrayGeneratorTest, GetLength) {
    int data[] = {1, 2, 3, 4};
    ArrayGenerator<int> gen(data, 4);
    Cardinal len = gen.GetLength();
    EXPECT_TRUE(len.IsFinite());
    EXPECT_EQ(len.Value(), 4);
}

TEST(SequenceGeneratorTest, FromArraySequence) {
    int arr[] = {100, 200};
    ArraySequence<int> seq(arr, 2);
    SequenceGenerator<int> gen(&seq);
    EXPECT_EQ(gen.GetNext(), 100);
    EXPECT_EQ(gen.GetNext(), 200);
    EXPECT_FALSE(gen.HasNext());
}

TEST(SequenceGeneratorTest, NullSequence) {
    SequenceGenerator<int> gen(nullptr);
    EXPECT_FALSE(gen.HasNext());
    EXPECT_EQ(gen.GetLength().Value(), 0);
}

TEST(ArithmeticProgressionGeneratorTest, Basic) {
    ArithmeticProgressionGenerator gen(0, 3);
    EXPECT_TRUE(gen.HasNext());
    EXPECT_EQ(gen.GetNext(), 0);
    EXPECT_EQ(gen.GetNext(), 3);
    EXPECT_EQ(gen.GetNext(), 6);
    EXPECT_EQ(gen.GetNext(), 9);
}

TEST(ArithmeticProgressionGeneratorTest, InfiniteLength) {
    ArithmeticProgressionGenerator gen(0, 1);
    EXPECT_TRUE(gen.GetLength().IsInfinite());
}

TEST(StringCharGeneratorTest, Basic) {
    StringCharGenerator gen("ab");
    EXPECT_EQ(gen.GetNext(), 'a');
    EXPECT_EQ(gen.GetNext(), 'b');
    EXPECT_FALSE(gen.HasNext());
}

TEST(StringCharGeneratorTest, Length) {
    StringCharGenerator gen("test");
    EXPECT_EQ(gen.GetLength().Value(), 4);
}

TEST(CyclicPatternCharGeneratorTest, Basic) {
    CyclicPatternCharGenerator gen("ab");
    EXPECT_EQ(gen.GetNext(), 'a');
    EXPECT_EQ(gen.GetNext(), 'b');
    EXPECT_EQ(gen.GetNext(), 'a');
    EXPECT_EQ(gen.GetNext(), 'b');
}

TEST(CyclicPatternCharGeneratorTest, EmptyPatternThrows) {
    EXPECT_THROW(CyclicPatternCharGenerator(""), std::invalid_argument);
}

TEST(CyclicPatternCharGeneratorTest, InfiniteLength) {
    CyclicPatternCharGenerator gen("x");
    EXPECT_TRUE(gen.GetLength().IsInfinite());
}

TEST(LazySequenceTest, DefaultConstructor) {
    LazySequence<int> seq;
    EXPECT_TRUE(seq.GetLength().IsFinite());
    EXPECT_EQ(seq.GetLength().Value(), 0);
    EXPECT_EQ(seq.GetMaterializedCount(), 0);
}

TEST(LazySequenceTest, FromArray) {
    int arr[] = {1, 2, 3};
    LazySequence<int> seq(arr, 3);
    EXPECT_EQ(seq.GetLength().Value(), 3);
    EXPECT_EQ(seq.Get(0), 1);
    EXPECT_EQ(seq.Get(1), 2);
    EXPECT_EQ(seq.Get(2), 3);
}

TEST(LazySequenceTest, FromSequence) {
    int arr[] = {5, 6};
    ArraySequence<int> arraySeq(arr, 2);
    LazySequence<int> seq(&arraySeq);
    EXPECT_EQ(seq.Get(0), 5);
    EXPECT_EQ(seq.Get(1), 6);
}

TEST(LazySequenceTest, FromGenerator) {
    LazySequence<int> seq(new ArithmeticProgressionGenerator(10, 1));
    EXPECT_EQ(seq.Get(0), 10);
    EXPECT_EQ(seq.Get(1), 11);
    EXPECT_TRUE(seq.GetLength().IsInfinite());
}

TEST(LazySequenceTest, CopyConstructorSharesState) {
    int arr[] = {1, 2};
    LazySequence<int> seq1(arr, 2);
    LazySequence<int> seq2(seq1);
    EXPECT_EQ(seq1.Get(0), 1);
    EXPECT_EQ(seq2.GetMaterializedCount(), 1);
}

TEST(LazySequenceTest, AssignmentOperator) {
    int arr[] = {7, 8};
    LazySequence<int> seq1(arr, 2);
    LazySequence<int> seq2;
    seq2 = seq1;
    EXPECT_EQ(seq2.Get(0), 7);
}

TEST(LazySequenceTest, GetFirst) {
    int arr[] = {10, 20};
    LazySequence<int> seq(arr, 2);
    EXPECT_EQ(seq.GetFirst(), 10);
}

TEST(LazySequenceTest, GetFirstOnEmptyThrows) {
    LazySequence<int> seq;
    EXPECT_THROW(seq.GetFirst(), std::out_of_range);
}

TEST(LazySequenceTest, GetLast) {
    int arr[] = {10, 20, 30};
    LazySequence<int> seq(arr, 3);
    EXPECT_EQ(seq.GetLast(), 30);
}

TEST(LazySequenceTest, GetLastOnEmptyThrows) {
    LazySequence<int> seq;
    EXPECT_THROW(seq.GetLast(), std::out_of_range);
}

TEST(LazySequenceTest, GetLastOnInfiniteThrows) {
    LazySequence<int> seq(new ArithmeticProgressionGenerator(0, 1));
    EXPECT_THROW(seq.GetLast(), std::logic_error);
}

TEST(LazySequenceTest, GetNegativeIndexThrows) {
    int arr[] = {1};
    LazySequence<int> seq(arr, 1);
    EXPECT_THROW(seq.Get(-1), std::out_of_range);
}

TEST(LazySequenceTest, GetOutOfRangeThrows) {
    int arr[] = {1, 2};
    LazySequence<int> seq(arr, 2);
    EXPECT_THROW(seq.Get(2), std::out_of_range);
}

TEST(LazySequenceTest, GetMaterializedCount) {
    int arr[] = {1, 2, 3, 4};
    LazySequence<int> seq(arr, 4);
    EXPECT_EQ(seq.GetMaterializedCount(), 0);
    seq.Get(0);
    EXPECT_EQ(seq.GetMaterializedCount(), 1);
    seq.Get(2);
    EXPECT_EQ(seq.GetMaterializedCount(), 3);
}

TEST(LazySequenceTest, Append) {
    int arr[] = {1, 2};
    LazySequence<int> seq(arr, 2);
    LazySequence<int>* result = seq.Append(3);
    EXPECT_EQ(result->Get(0), 1);
    EXPECT_EQ(result->Get(1), 2);
    EXPECT_EQ(result->Get(2), 3);
    EXPECT_EQ(result->GetLength().Value(), 3);
    delete result;
}

TEST(LazySequenceTest, Prepend) {
    int arr[] = {2, 3};
    LazySequence<int> seq(arr, 2);
    LazySequence<int>* result = seq.Prepend(1);
    EXPECT_EQ(result->Get(0), 1);
    EXPECT_EQ(result->Get(1), 2);
    EXPECT_EQ(result->Get(2), 3);
    delete result;
}

TEST(LazySequenceTest, InsertAt) {
    int arr[] = {1, 3};
    LazySequence<int> seq(arr, 2);
    LazySequence<int>* result = seq.InsertAt(2, 1);
    EXPECT_EQ(result->Get(0), 1);
    EXPECT_EQ(result->Get(1), 2);
    EXPECT_EQ(result->Get(2), 3);
    delete result;
}

TEST(LazySequenceTest, InsertAtNegativeIndexThrows) {
    int arr[] = {1};
    LazySequence<int> seq(arr, 1);
    EXPECT_THROW(seq.InsertAt(0, -1), std::out_of_range);
}

TEST(LazySequenceTest, InsertAtOutOfRangeThrows) {
    int arr[] = {1};
    LazySequence<int> seq(arr, 1);
    EXPECT_THROW(seq.InsertAt(0, 1), std::out_of_range);
}

TEST(LazySequenceTest, ConcatFinite) {
    int a[] = {1, 2};
    int b[] = {3, 4};
    LazySequence<int> first(a, 2);
    LazySequence<int> second(b, 2);
    LazySequence<int>* result = first.Concat(&second);
    EXPECT_EQ(result->GetLength().Value(), 4);
    EXPECT_EQ(result->Get(0), 1);
    EXPECT_EQ(result->Get(3), 4);
    delete result;
}

TEST(LazySequenceTest, ConcatNullThrows) {
    int a[] = {1};
    LazySequence<int> first(a, 1);
    EXPECT_THROW(first.Concat(nullptr), std::invalid_argument);
}

TEST(LazySequenceTest, ConcatWithInfiniteFirst) {
    LazySequence<int> infinite(new ArithmeticProgressionGenerator(0, 1));
    int b[] = {100, 200};
    LazySequence<int> finite(b, 2);
    LazySequence<int>* result = infinite.Concat(&finite);
    EXPECT_TRUE(result->GetLength().IsInfinite());
    EXPECT_EQ(result->Get(0), 0);
    EXPECT_EQ(result->Get(1), 1);
    EXPECT_EQ(result->Get(2), 2);
    delete result;
}

TEST(LazySequenceTest, GetSubsequence) {
    int arr[] = {0, 1, 2, 3, 4};
    LazySequence<int> seq(arr, 5);
    LazySequence<int>* sub = seq.GetSubsequence(1, 3);
    EXPECT_EQ(sub->GetLength().Value(), 3);
    EXPECT_EQ(sub->Get(0), 1);
    EXPECT_EQ(sub->Get(2), 3);
    delete sub;
}

TEST(LazySequenceTest, GetSubsequenceInvalidThrows) {
    int arr[] = {1, 2, 3};
    LazySequence<int> seq(arr, 3);
    EXPECT_THROW(seq.GetSubsequence(-1, 0), std::out_of_range);
    EXPECT_THROW(seq.GetSubsequence(0, -1), std::out_of_range);
    EXPECT_THROW(seq.GetSubsequence(1, 0), std::out_of_range);
    EXPECT_THROW(seq.GetSubsequence(0, 3), std::out_of_range);
}

TEST(LazySequenceTest, Map) {
    int arr[] = {1, 2, 3};
    LazySequence<int> seq(arr, 3);
    LazySequence<int>* doubled = seq.Map<int>([](const int& x) { return x * 2; });
    EXPECT_EQ(doubled->Get(0), 2);
    EXPECT_EQ(doubled->Get(1), 4);
    EXPECT_EQ(doubled->Get(2), 6);
    delete doubled;
}

TEST(LazySequenceTest, MapOnInfinite) {
    LazySequence<int> seq(new ArithmeticProgressionGenerator(0, 1));
    LazySequence<int>* doubled = seq.Map<int>([](const int& x) { return x * 10; });
    EXPECT_EQ(doubled->Get(0), 0);
    EXPECT_EQ(doubled->Get(1), 10);
    EXPECT_TRUE(doubled->GetLength().IsInfinite());
    delete doubled;
}

TEST(LazySequenceTest, Where) {
    int arr[] = {1, 2, 3, 4, 5};
    LazySequence<int> seq(arr, 5);
    LazySequence<int>* even = seq.Where([](const int& x) { return x % 2 == 0; });
    EXPECT_EQ(even->Get(0), 2);
    EXPECT_EQ(even->Get(1), 4);
    EXPECT_EQ(even->GetLength().Value(), 2);
    delete even;
}

TEST(LazySequenceTest, WhereEmptyResult) {
    int arr[] = {1, 3, 5};
    LazySequence<int> seq(arr, 3);
    LazySequence<int>* even = seq.Where([](const int& x) { return x % 2 == 0; });
    EXPECT_EQ(even->GetLength().Value(), 0);
    delete even;
}

TEST(LazySequenceTest, WhereOnInfinite) {
    LazySequence<int> seq(new ArithmeticProgressionGenerator(0, 1));
    LazySequence<int>* filtered = seq.Where([](const int& x) { return x % 2 == 0; });
    EXPECT_EQ(filtered->Get(0), 0);
    EXPECT_EQ(filtered->Get(1), 2);
    EXPECT_TRUE(filtered->GetLength().IsInfinite());
    delete filtered;
}

TEST(LazySequenceTest, Reduce) {
    int arr[] = {1, 2, 3, 4};
    LazySequence<int> seq(arr, 4);
    int sum = seq.Reduce<int>(0, [](int acc, const int& x) { return acc + x; });
    EXPECT_EQ(sum, 10);
}

TEST(LazySequenceTest, ReduceOnInfiniteThrows) {
    LazySequence<int> seq(new ArithmeticProgressionGenerator(0, 1));
    EXPECT_THROW(seq.Reduce<int>(0, [](int a, const int&) { return a; }), std::logic_error);
}

TEST(LazySequenceTest, Zip) {
    int a[] = {1, 2, 3};
    int b[] = {10, 20, 30};
    LazySequence<int> first(a, 3);
    LazySequence<int> second(b, 3);
    LazySequence<Pair<int, int>>* zipped = first.Zip(&second);
    EXPECT_EQ(zipped->Get(0).first, 1);
    EXPECT_EQ(zipped->Get(0).second, 10);
    EXPECT_EQ(zipped->Get(2).first, 3);
    EXPECT_EQ(zipped->Get(2).second, 30);
    EXPECT_EQ(zipped->GetLength().Value(), 3);
    delete zipped;
}

TEST(LazySequenceTest, ZipDifferentLengths) {
    int a[] = {1, 2};
    int b[] = {10, 20, 30};
    LazySequence<int> first(a, 2);
    LazySequence<int> second(b, 3);
    LazySequence<Pair<int, int>>* zipped = first.Zip(&second);
    EXPECT_EQ(zipped->GetLength().Value(), 2);
    delete zipped;
}

TEST(LazySequenceTest, ZipNullThrows) {
    int a[] = {1};
    LazySequence<int> first(a, 1);
    EXPECT_THROW(first.Zip<int>(nullptr), std::invalid_argument);
}

TEST(LazySequenceTest, Interleave3) {
    LazySequence<int> first(new ArithmeticProgressionGenerator(0, 1));
    LazySequence<int> second(new ArithmeticProgressionGenerator(100, 1));
    LazySequence<int> third(new ArithmeticProgressionGenerator(200, 1));
    LazySequence<int>* result = LazySequence<int>::Interleave3(&first, &second, &third);
    EXPECT_EQ(result->Get(0), 0);
    EXPECT_EQ(result->Get(1), 100);
    EXPECT_EQ(result->Get(2), 200);
    EXPECT_EQ(result->Get(3), 1);
    EXPECT_EQ(result->Get(4), 101);
    EXPECT_EQ(result->Get(5), 201);
    delete result;
}

TEST(LazySequenceTest, Interleave3NullThrows) {
    LazySequence<int> first(new ArithmeticProgressionGenerator(0, 1));
    EXPECT_THROW(LazySequence<int>::Interleave3(&first, nullptr, &first), std::invalid_argument);
}

TEST(ReadOnlyStreamTest, EmptyStream) {
    ReadOnlyStream<int> stream;
    EXPECT_TRUE(stream.IsEndOfStream());
    EXPECT_THROW(stream.Read(), std::out_of_range);
}

TEST(ReadOnlyStreamTest, FromSequence) {
    int arr[] = {1, 2, 3};
    ArraySequence<int> seq(arr, 3);
    ReadOnlyStream<int> stream(&seq);
    EXPECT_FALSE(stream.IsEndOfStream());
    EXPECT_EQ(stream.Read(), 1);
    EXPECT_EQ(stream.Read(), 2);
    EXPECT_EQ(stream.Read(), 3);
    EXPECT_TRUE(stream.IsEndOfStream());
}

TEST(ReadOnlyStreamTest, FromLazySequence) {
    int arr[] = {10, 20};
    LazySequence<int> lazy(arr, 2);
    ReadOnlyStream<int> stream(&lazy);
    EXPECT_EQ(stream.Read(), 10);
    EXPECT_EQ(stream.Read(), 20);
    EXPECT_TRUE(stream.IsEndOfStream());
}

TEST(ReadOnlyStreamTest, FromString) {
    ReadOnlyStream<char> stream("hi");
    EXPECT_EQ(stream.Read(), 'h');
    EXPECT_EQ(stream.Read(), 'i');
    EXPECT_TRUE(stream.IsEndOfStream());
}

TEST(ReadOnlyStreamTest, GetPosition) {
    int arr[] = {1, 2, 3};
    ArraySequence<int> seq(arr, 3);
    ReadOnlyStream<int> stream(&seq);
    EXPECT_EQ(stream.GetPosition(), 0);
    stream.Read();
    EXPECT_EQ(stream.GetPosition(), 1);
}

TEST(ReadOnlyStreamTest, SeekSequence) {
    int arr[] = {0, 1, 2, 3, 4};
    ArraySequence<int> seq(arr, 5);
    ReadOnlyStream<int> stream(&seq);
    EXPECT_TRUE(stream.IsCanSeek());
    stream.Seek(3);
    EXPECT_EQ(stream.GetPosition(), 3);
    EXPECT_EQ(stream.Read(), 3);
}

TEST(ReadOnlyStreamTest, SeekOutOfRangeThrows) {
    int arr[] = {1, 2};
    ArraySequence<int> seq(arr, 2);
    ReadOnlyStream<int> stream(&seq);
    EXPECT_THROW(stream.Seek(3), std::out_of_range);
}

TEST(ReadOnlyStreamTest, IsCanGoBack) {
    int arr[] = {1, 2};
    ArraySequence<int> seq(arr, 2);
    ReadOnlyStream<int> stream(&seq);
    EXPECT_TRUE(stream.IsCanGoBack());
}

TEST(ReadOnlyStreamTest, ReadAfterEndThrows) {
    int arr[] = {1};
    ArraySequence<int> seq(arr, 1);
    ReadOnlyStream<int> stream(&seq);
    stream.Read();
    EXPECT_THROW(stream.Read(), std::out_of_range);
}

TEST(ReadOnlyStreamTest, ProxyStream) {
    int arr[] = {5, 6};
    ArraySequence<int> seq(arr, 2);
    ReadOnlyStream<int> inner(&seq);
    ReadOnlyStream<int> stream(&inner);
    EXPECT_EQ(stream.Read(), 5);
    EXPECT_EQ(stream.Read(), 6);
}

TEST(WriteOnlyStreamTest, WriteToSequence) {
    ArraySequence<int> seq;
    WriteOnlyStream<int> stream(&seq);
    stream.Write(1);
    stream.Write(2);
    EXPECT_EQ(stream.GetPosition(), 2);
    EXPECT_EQ(seq.GetLength(), 2);
    EXPECT_EQ(seq.Get(0), 1);
    EXPECT_EQ(seq.Get(1), 2);
}

TEST(WriteOnlyStreamTest, NullSequenceThrows) {
    EXPECT_THROW(WriteOnlyStream<int>((Sequence<int>*)nullptr), std::invalid_argument);
}

TEST(WriteOnlyStreamTest, ProxyStream) {
    ArraySequence<int> seq;
    WriteOnlyStream<int> inner(&seq);
    WriteOnlyStream<int> stream(&inner);
    stream.Write(100);
    EXPECT_EQ(seq.Get(0), 100);
}

TEST(SubstringFrequencyCounterTest, InvalidKThrows) {
    SubstringFrequencyCounter counter;
    LazySequence<char> seq;
    EXPECT_THROW(counter.ProcessLazySequence(&seq, 0, 0), std::invalid_argument);
    EXPECT_THROW(counter.ProcessLazySequence(&seq, -1, 0), std::invalid_argument);
}

TEST(SubstringFrequencyCounterTest, EmptySequence) {
    SubstringFrequencyCounter counter;
    LazySequence<char> seq;
    counter.ProcessLazySequence(&seq, 2, 0);
    EXPECT_EQ(counter.GetTotalProcessedCharacters(), 0);
    EXPECT_EQ(counter.GetTotalSubstrings(), 0);
}

TEST(SubstringFrequencyCounterTest, SingleCharK1) {
    SubstringFrequencyCounter counter;
    char data[] = {'a'};
    LazySequence<char> seq(data, 1);
    counter.ProcessLazySequence(&seq, 1, 1);
    EXPECT_EQ(counter.GetTotalSubstrings(), 1);
    EXPECT_EQ(counter.GetFrequency("a"), 1);
}

TEST(SubstringFrequencyCounterTest, BasicCounting) {
    SubstringFrequencyCounter counter;
    char data[] = {'a', 'b', 'a', 'b', 'a'};
    LazySequence<char> seq(data, 5);
    counter.ProcessLazySequence(&seq, 2, 5);
    EXPECT_EQ(counter.GetTotalProcessedCharacters(), 5);
    EXPECT_EQ(counter.GetTotalSubstrings(), 4);
    EXPECT_EQ(counter.GetFrequency("ab"), 2);
    EXPECT_EQ(counter.GetFrequency("ba"), 2);
}

TEST(SubstringFrequencyCounterTest, UniqueSubstrings) {
    SubstringFrequencyCounter counter;
    char data[] = {'a', 'a', 'a'};
    LazySequence<char> seq(data, 3);
    counter.ProcessLazySequence(&seq, 2, 3);
    EXPECT_EQ(counter.GetUniqueSubstringCount(), 1);
    EXPECT_EQ(counter.GetFrequency("aa"), 2);
}

TEST(SubstringFrequencyCounterTest, GetAllFrequenciesSorted) {
    SubstringFrequencyCounter counter;
    char data[] = {'c', 'a', 'b', 'a'};
    LazySequence<char> seq(data, 4);
    counter.ProcessLazySequence(&seq, 2, 4);
    auto all = counter.GetAllFrequencies();
    EXPECT_EQ(all.Get(0).first, "ab");
    EXPECT_EQ(all.Get(1).first, "ba");
    EXPECT_EQ(all.Get(2).first, "ca");
}

TEST(SubstringFrequencyCounterTest, Clear) {
    SubstringFrequencyCounter counter;
    char data[] = {'a', 'a'};
    LazySequence<char> seq(data, 2);
    counter.ProcessLazySequence(&seq, 1, 2);
    EXPECT_GT(counter.GetTotalSubstrings(), 0);
    counter.Clear();
    EXPECT_EQ(counter.GetTotalProcessedCharacters(), 0);
    EXPECT_EQ(counter.GetTotalSubstrings(), 0);
    EXPECT_EQ(counter.GetUniqueSubstringCount(), 0);
}

TEST(SubstringFrequencyCounterTest, NullLazySequenceThrows) {
    SubstringFrequencyCounter counter;
    EXPECT_THROW(counter.ProcessLazySequence(nullptr, 2, 10), std::invalid_argument);
}

TEST(SubstringFrequencyCounterTest, InfiniteSequenceWithLimit) {
    SubstringFrequencyCounter counter;
    LazySequence<char> seq(new CyclicPatternCharGenerator("ab"));
    counter.ProcessLazySequence(&seq, 2, 5);
    EXPECT_EQ(counter.GetTotalSubstrings(), 4);
    EXPECT_EQ(counter.GetFrequency("ab"), 2);
    EXPECT_EQ(counter.GetFrequency("ba"), 2);
}

TEST(IntegrationTest, LazySequenceWithStream) {
    int arr[] = {1, 2, 3, 4, 5};
    LazySequence<int> lazy(arr, 5);
    ReadOnlyStream<int> stream(&lazy);
    int sum = 0;
    while (!stream.IsEndOfStream()) {
        sum += stream.Read();
    }
    EXPECT_EQ(sum, 15);
}

TEST(IntegrationTest, MapReducePipeline) {
    int arr[] = {1, 2, 3, 4, 5};
    LazySequence<int> seq(arr, 5);
    LazySequence<int>* filtered = seq.Where([](const int& x) { return x % 2 != 0; });
    LazySequence<int>* doubled = filtered->Map<int>([](const int& x) { return x * 2; });
    int result = doubled->Reduce<int>(0, [](int acc, const int& x) { return acc + x; });
    EXPECT_EQ(result, 18);
    delete filtered;
    delete doubled;
}

TEST(IntegrationTest, ZipAndMap) {
    int a[] = {1, 2, 3};
    int b[] = {10, 20, 30};
    LazySequence<int> first(a, 3);
    LazySequence<int> second(b, 3);
    LazySequence<Pair<int, int>>* zipped = first.Zip(&second);
    LazySequence<int>* sums = zipped->Map<int>([](const Pair<int, int>& p) {
        return p.first + p.second;
    });
    EXPECT_EQ(sums->Get(0), 11);
    EXPECT_EQ(sums->Get(1), 22);
    EXPECT_EQ(sums->Get(2), 33);
    delete zipped;
    delete sums;
}