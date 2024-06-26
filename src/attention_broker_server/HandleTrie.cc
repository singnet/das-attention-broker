#include "Utils.h"
#include "expression_hasher.h"
#include "HandleTrie.h"
#include <iostream>

using namespace attention_broker_server;

HandleTrie::TrieValue::TrieValue() {
}

HandleTrie::TrieValue::~TrieValue() {
}

HandleTrie::TrieNode::TrieNode() {
    children = new TrieNode*[TRIE_ALPHABET_SIZE];
    for (unsigned int i = 0; i < TRIE_ALPHABET_SIZE; i++) {
        children[i] = NULL;
    }
    this->value = NULL;
    suffix_start = 0;
}

HandleTrie::TrieNode::~TrieNode() {
    for (unsigned int i = 0; i < TRIE_ALPHABET_SIZE; i++) {
        delete children[i];
    }
    delete [] children;
    delete value;
}

bool HandleTrie::TLB_INITIALIZED = false;
unsigned char HandleTrie::TLB[256];

// --------------------------------------------------------------------------------
// Public methods

HandleTrie::HandleTrie(unsigned int key_size) {
    if (key_size == 0 || key_size > 255) {
        Utils::error("Invalid key size: " + to_string(key_size));
    }
    this->key_size = key_size;
    if (! HandleTrie::TLB_INITIALIZED) {
        HandleTrie::TLB_INIT();
    }
    root = new TrieNode();
}

HandleTrie::~HandleTrie() {
    delete root;
}

void HandleTrie::insert(string key, TrieValue *value) {

    if (key.size() != key_size) {
        Utils::error("Invalid key size: " + to_string(key.size()) + " != " + to_string(key_size));
    }

    TrieNode *tree_cursor = root;
    TrieNode *parent = root;
    TrieNode *child;
    TrieNode *split;
    unsigned char key_cursor = 0;
    while (true) {
        unsigned char c = TLB[(unsigned char) key[key_cursor]];
        if (tree_cursor->children[c] == NULL) {
            if (tree_cursor->suffix_start > 0) {
                unsigned char c_key_pred  = TLB[(unsigned char) key[key_cursor -1]];
                if (key[key_cursor] == tree_cursor->suffix[key_cursor]) {
                    child = new TrieNode();
                    child->children[c] = tree_cursor;
                    parent->children[c_key_pred] = child;
                    parent = child;
                    tree_cursor->suffix_start++;
                    key_cursor++;
                } else {
                    child = new TrieNode();
                    child->suffix = key;
                    child->suffix_start = key_cursor + 1;
                    child->value = value;
                    unsigned char c_tree_cursor = TLB[(unsigned char) tree_cursor->suffix[tree_cursor->suffix_start]];
                    tree_cursor->suffix_start++;
                    split = new TrieNode();
                    split->children[c] = child;
                    split->children[c_tree_cursor] = tree_cursor;
                    parent->children[c_key_pred] = split;
                    break;
                }
            } else {
                child = new TrieNode();
                child->suffix = key;
                child->suffix_start = key_cursor + 1;
                child->value = value;
                tree_cursor->children[c] = child;
                break;
            }
        } else {
            parent = tree_cursor;
            tree_cursor = tree_cursor->children[c];
            key_cursor++;
        }
    }
}

HandleTrie::TrieValue *HandleTrie::lookup(string key) {

    if (key.size() != key_size) {
        Utils::error("Invalid key size: " + to_string(key.size()) + " != " + to_string(key_size));
    }

    TrieNode *tree_cursor = root;
    unsigned char key_cursor = 0;
    while (tree_cursor != NULL) {
        if (tree_cursor->suffix_start > 0) {
            bool match = true;
            unsigned int n = key.size();
            for (unsigned int i = key_cursor; i < n; i++) {
                if (key[i] != tree_cursor->suffix[i]) {
                    match = false;
                }
            }
            if (match) {
                return tree_cursor->value;
            } else {
                return NULL;
            }
        } else {
            unsigned char c = TLB[(unsigned char) key[key_cursor]];
            tree_cursor = tree_cursor->children[c];
            key_cursor++;
        }
    }
    return NULL;
}
