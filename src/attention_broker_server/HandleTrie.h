#ifndef _ATTENTION_BROKER_SERVER_HANDLETRIE_H
#define _ATTENTION_BROKER_SERVER_HANDLETRIE_H

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
    };

    class TrieNode {
        public:
            TrieNode();
            virtual ~TrieNode();
            virtual void insert(
                unsigned char *key, 
                unsigned int last_char, 
                unsigned int cursor, 
                TrieValue *value) = 0;
            virtual TrieValue *lookup(
                unsigned char *key,
                unsigned int last_char,
                unsigned int cursor) = 0;
    };

    class InternalTrieNode : public TrieNode {
        public:
            InternalTrieNode();
            ~InternalTrieNode();
            virtual void insert(
                unsigned char *key, 
                unsigned int last_char, 
                unsigned int cursor, 
                TrieValue *value);
            virtual TrieValue *lookup(
                unsigned char *key,
                unsigned int last_char,
                unsigned int cursor);
            TrieNode **children;
    };

    class TrieLeaf : public TrieNode {
        public:
            TrieLeaf(TrieValue *value);
            ~TrieLeaf();
            virtual void insert(
                unsigned char *key, 
                unsigned int last_char, 
                unsigned int cursor, 
                TrieValue *value);
            virtual TrieValue *lookup(
                unsigned char *key,
                unsigned int last_char,
                unsigned int cursor);
            TrieValue *value;
            //string suffix;
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
        TLB['0'] = 0;
        TLB['1'] = 1;
        TLB['2'] = 2;
        TLB['3'] = 3;
        TLB['4'] = 4;
        TLB['5'] = 5;
        TLB['6'] = 6;
        TLB['7'] = 7;
        TLB['8'] = 8;
        TLB['9'] = 9;
        TLB['a'] = TLB['A'] = 10;
        TLB['b'] = TLB['B'] = 11;
        TLB['c'] = TLB['C'] = 12;
        TLB['d'] = TLB['D'] = 13;
        TLB['e'] = TLB['E'] = 14;
        TLB['f'] = TLB['F'] = 15;
        TLB_INITIALIZED = true;
    }
};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_HANDLETRIE_H
