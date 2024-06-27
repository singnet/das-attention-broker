#ifndef _ATTENTION_BROKER_SERVER_HANDLETRIE_H
#define _ATTENTION_BROKER_SERVER_HANDLETRIE_H

#include <mutex>

#define TRIE_ALPHABET_SIZE ((unsigned int) 16)

using namespace std;

namespace attention_broker_server {

/**
 *
 */
class HandleTrie {

public:

    class TrieValue {
        protected:
            TrieValue();
        public:
            virtual ~TrieValue();
            virtual void merge(TrieValue *other) = 0;
    };

    class TrieNode {
        public:
            TrieNode();
            ~TrieNode();
            void insert(
                unsigned char *key, 
                unsigned int last_char, 
                unsigned int cursor, 
                TrieValue *value);
            TrieValue *lookup(
                unsigned char *key,
                unsigned int last_char,
                unsigned int cursor);
            TrieNode **children;
            TrieValue *value;
            string suffix;
            unsigned char suffix_start;
            mutex trie_node_mutex;
    };

    TrieNode *root;
    unsigned int key_size;

    HandleTrie(unsigned int key_size);
    ~HandleTrie();
    void insert(string key, TrieValue *value);
    TrieValue *lookup(string key);

private:

    static unsigned char TLB[256];
    static bool TLB_INITIALIZED;
    static void TLB_INIT() {
        TLB[(unsigned char) '0'] = 0;
        TLB[(unsigned char) '1'] = 1;
        TLB[(unsigned char) '2'] = 2;
        TLB[(unsigned char) '3'] = 3;
        TLB[(unsigned char) '4'] = 4;
        TLB[(unsigned char) '5'] = 5;
        TLB[(unsigned char) '6'] = 6;
        TLB[(unsigned char) '7'] = 7;
        TLB[(unsigned char) '8'] = 8;
        TLB[(unsigned char) '9'] = 9;
        TLB[(unsigned char) 'a'] = TLB[(unsigned char) 'A'] = 10;
        TLB[(unsigned char) 'b'] = TLB[(unsigned char) 'B'] = 11;
        TLB[(unsigned char) 'c'] = TLB[(unsigned char) 'C'] = 12;
        TLB[(unsigned char) 'd'] = TLB[(unsigned char) 'D'] = 13;
        TLB[(unsigned char) 'e'] = TLB[(unsigned char) 'E'] = 14;
        TLB[(unsigned char) 'f'] = TLB[(unsigned char) 'F'] = 15;
        TLB_INITIALIZED = true;
    }
};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_HANDLETRIE_H
