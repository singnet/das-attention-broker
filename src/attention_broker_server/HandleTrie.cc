#include "Utils.h"
#include "expression_hasher.h"
#include "HandleTrie.h"

using namespace attention_broker_server;

HandleTrie::TrieValue::TrieValue() {
}

HandleTrie::TrieValue::~TrieValue() {
}

HandleTrie::TrieNode::TrieNode() {
}

HandleTrie::TrieNode::~TrieNode() {
}

HandleTrie::InternalTrieNode::InternalTrieNode() {
    children = new TrieNode*[TRIE_ALPHABET_SIZE];
    for (unsigned int i = 0; i < TRIE_ALPHABET_SIZE; i++) {
        children[i] = NULL;
    }
}

HandleTrie::InternalTrieNode::~InternalTrieNode() {
    for (unsigned int i = 0; i < TRIE_ALPHABET_SIZE; i++) {
        delete children[i];
    }
    delete [] children;
}

HandleTrie::TrieLeaf::TrieLeaf(TrieValue *value) {
    this->value = value;
}

HandleTrie::TrieLeaf::~TrieLeaf() {
    delete value;
}

void HandleTrie::InternalTrieNode::insert(
    unsigned char *key, 
    unsigned int last_char, 
    unsigned int cursor, 
    TrieValue *value) {

    unsigned char c = TLB[key[cursor]];
    if (cursor == last_char) {
        children[c] = new TrieLeaf(value);
    } else {
        TrieNode *child = children[c];
        if (child == NULL) {
            child = new InternalTrieNode();
            children[c] = child;
        }
        child->insert(key, last_char, cursor + 1, value);
    }
}

HandleTrie::TrieValue *HandleTrie::InternalTrieNode::lookup(
    unsigned char *key, 
    unsigned int last_char, 
    unsigned int cursor) {

    TrieNode *child = children[TLB[key[cursor]]];
    if (child == NULL) {
        return NULL;
    }
    if (cursor == last_char) {
        return ((TrieLeaf *) child)->value;
    } else {
        return child->lookup(key, last_char, cursor + 1);
    }
}

HandleTrie::TrieValue *HandleTrie::TrieLeaf::lookup(
    unsigned char *key, 
    unsigned int last_char, 
    unsigned int cursor) {

    Utils::error("Invalid method call.");
    return NULL; // Not reached
}

void HandleTrie::TrieLeaf::insert(
    unsigned char *key, 
    unsigned int last_char, 
    unsigned int cursor, 
    TrieValue *value) {

    Utils::error("Invalid method call.");
}

bool HandleTrie::TLB_INITIALIZED = false;
unsigned char HandleTrie::TLB[256];

// --------------------------------------------------------------------------------
// Public methods

HandleTrie::HandleTrie(unsigned int key_size) {
    if (key_size == 0) {
        Utils::error("Invalid key size: " + to_string(key_size));
    }
    this->key_size = key_size;
    if (! HandleTrie::TLB_INITIALIZED) {
        HandleTrie::TLB_INIT();
    }
    root = new InternalTrieNode();
}

HandleTrie::~HandleTrie() {
    delete root;
}

void HandleTrie::insert(string key, TrieValue *value) {
    if (key.size() != key_size) {
        Utils::error("Invalid key size: " + to_string(key.size()) + " != " + to_string(key_size));
    }
    root->insert((unsigned char *) key.c_str(), key_size - 1, (unsigned int) 0, value);

    /*
    tree_cursor = root;
    key_cursor = 0;
    while (true) {
        unsigned char c = TLB[key_cursor];
        if (tree_cursor->children[c] == NULL) {
            child = new TrieLeaf(value);
            child->suffix.assign(key, key_cursor + 1, key.size() - key_cursor - 1);
            tree_cursor->children[c] = child;
            break;
        }
        tree_cursor = tree_cursor->children[c];
        key_cursor++;
    }
    */
}

HandleTrie::TrieValue *HandleTrie::lookup(string key) {
    if (key.size() != key_size) {
        Utils::error("Invalid key size: " + to_string(key.size()) + " != " + to_string(key_size));
    }
    return root->lookup((unsigned char *) key.c_str(), key_size - 1, (unsigned int) 0);
}
