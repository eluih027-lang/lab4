#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include "ArraySequence.h"
#include "Pair.h"

#include <cstddef>

template <typename TKey, typename TValue>
class BinarySearchTree {
private:
    struct Node {
        TKey key;
        TValue value;
        Node* left;
        Node* right;

        Node(const TKey& nodeKey, const TValue& nodeValue)
            : key(nodeKey), value(nodeValue), left(0), right(0) {
        }
    };

    Node* root;
    size_t count;

    static Node* CopyNode(Node* node) {
        if (node == 0) {
            return 0;
        }
        Node* result = new Node(node->key, node->value);
        result->left = CopyNode(node->left);
        result->right = CopyNode(node->right);
        return result;
    }

    static void DeleteNode(Node* node) {
        if (node == 0) {
            return;
        }
        DeleteNode(node->left);
        DeleteNode(node->right);
        delete node;
    }

    static void FillInOrder(Node* node, ArraySequence<Pair<TKey, TValue> >& result) {
        if (node == 0) {
            return;
        }
        FillInOrder(node->left, result);
        result.Append(Pair<TKey, TValue>(node->key, node->value));
        FillInOrder(node->right, result);
    }

    Node* FindNode(const TKey& key) const {
        Node* current = root;
        while (current != 0) {
            if (key == current->key) {
                return current;
            }
            if (key < current->key) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return 0;
    }

public:
    BinarySearchTree() : root(0), count(0) {
    }

    BinarySearchTree(const BinarySearchTree<TKey, TValue>& other)
        : root(CopyNode(other.root)), count(other.count) {
    }

    BinarySearchTree<TKey, TValue>& operator=(const BinarySearchTree<TKey, TValue>& other) {
        if (this != &other) {
            BinarySearchTree<TKey, TValue> copy(other);
            Swap(copy);
        }
        return *this;
    }

    ~BinarySearchTree() {
        Clear();
    }

    void Swap(BinarySearchTree<TKey, TValue>& other) {
        Node* tempRoot = root;
        root = other.root;
        other.root = tempRoot;

        size_t tempCount = count;
        count = other.count;
        other.count = tempCount;
    }

    void Clear() {
        DeleteNode(root);
        root = 0;
        count = 0;
    }

    size_t GetCount() const {
        return count;
    }

    bool Contains(const TKey& key) const {
        return FindNode(key) != 0;
    }

    TValue GetOrDefault(const TKey& key, const TValue& defaultValue) const {
        Node* node = FindNode(key);
        if (node == 0) {
            return defaultValue;
        }
        return node->value;
    }

    void AddOrUpdate(const TKey& key, const TValue& value) {
        if (root == 0) {
            root = new Node(key, value);
            ++count;
            return;
        }

        Node* current = root;
        while (true) {
            if (key == current->key) {
                current->value = value;
                return;
            }
            if (key < current->key) {
                if (current->left == 0) {
                    current->left = new Node(key, value);
                    ++count;
                    return;
                }
                current = current->left;
            } else {
                if (current->right == 0) {
                    current->right = new Node(key, value);
                    ++count;
                    return;
                }
                current = current->right;
            }
        }
    }

    void Increment(const TKey& key, const TValue& step) {
        if (root == 0) {
            root = new Node(key, step);
            ++count;
            return;
        }

        Node* current = root;
        while (true) {
            if (key == current->key) {
                current->value = current->value + step;
                return;
            }
            if (key < current->key) {
                if (current->left == 0) {
                    current->left = new Node(key, step);
                    ++count;
                    return;
                }
                current = current->left;
            } else {
                if (current->right == 0) {
                    current->right = new Node(key, step);
                    ++count;
                    return;
                }
                current = current->right;
            }
        }
    }

    ArraySequence<Pair<TKey, TValue> > ToArrayInOrder() const {
        ArraySequence<Pair<TKey, TValue> > result;
        FillInOrder(root, result);
        return result;
    }
};

#endif
