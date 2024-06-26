#include <cstdlib>
#include "gtest/gtest.h"

#include "Utils.h"
#include "expression_hasher.h"
#include "HandleTrie.h"

using namespace attention_broker_server;
using namespace std;

class TestValue: public HandleTrie::TrieValue {
    public:
        unsigned int count;
        TestValue() {
            count = 1;
        }
};

unsigned char TLB[16] = {
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'a',
    'b',
    'c',
    'd',
    'e',
    'f',
};

/*
TEST(HandleTrieTest, Basics) {
    
    char buffer[1000];
    map<string, int> baseline;
    TestValue *value;

    for (unsigned int key_size: {2, 5, 10, 100}) {
        baseline.clear();
        HandleTrie *trie = new HandleTrie(key_size);
        for (unsigned int i = 0; i < 100000; i++) {
            for (unsigned int j = 0; j < key_size; j++) {
                buffer[j] = TLB[(rand() % 16)];
            }
            buffer[key_size] = 0;
            string s = buffer;
            if (baseline.find(s) == baseline.end()) {
                baseline[s] = 0;
            }
            baseline[s] = baseline[s] + 1;
            value = (TestValue *) trie->lookup(s);
            if (value == NULL) {
                value = new TestValue();
                trie->insert(s, value);
            } else {
                value->count += 1;
            }
        }
        for (auto const& pair : baseline) {
            value = (TestValue *) trie->lookup(pair.first);
            EXPECT_EQ(pair.second, value->count);
        }
        delete trie;
    }
}

TEST(HandleTrieTest, hasher) {
    char buffer[1000];
    map<string, int> baseline;
    TestValue *value;

    for (unsigned int key_count: {1, 2, 5}) {
        baseline.clear();
        unsigned int key_size = (HANDLE_HASH_SIZE - 1) * key_count;
        HandleTrie *trie = new HandleTrie(key_size);
        for (unsigned int i = 0; i < 100000; i++) {
            for (unsigned int j = 0; j < key_size; j++) {
                buffer[j] = TLB[(rand() % 16)];
            }
            buffer[key_size] = 0;
            string s = buffer;
            if (baseline.find(s) == baseline.end()) {
                baseline[s] = 0;
            }
            baseline[s] = baseline[s] + 1;
            value = (TestValue *) trie->lookup(s);
            if (value == NULL) {
                value = new TestValue();
                trie->insert(s, value);
            } else {
                value->count += 1;
            }
        }
        for (auto const& pair : baseline) {
            value = (TestValue *) trie->lookup(pair.first);
            //cout << pair.second << " " << value->count << endl;
            EXPECT_EQ(pair.second, value->count);
        }
        delete trie;
    }
}
*/

TEST(HandleTrieTest, benchmark) {
    char buffer[1000];
    map<string, int> baseline;
    TestValue *value;
    StopWatch timer_std;
    StopWatch timer_trie;
    unsigned int n_insertions = 200000;

    timer_std.start();
    for (unsigned int key_count: {1, 2}) {
        unsigned int key_size = (HANDLE_HASH_SIZE - 1) * key_count;
        for (unsigned int i = 0; i < n_insertions; i++) {
            for (unsigned int j = 0; j < key_size; j++) {
                buffer[j] = TLB[(rand() % 16)];
            }
            buffer[key_size] = 0;
            string s = buffer;
            if (baseline.find(s) == baseline.end()) {
                baseline[s] = 0;
            }
            baseline[s] = baseline[s] + 1;
        }
    }
    timer_std.stop();


    timer_trie.start();
    for (unsigned int key_count: {1, 2}) {
        unsigned int key_size = (HANDLE_HASH_SIZE - 1) * key_count;
        HandleTrie *trie = new HandleTrie(key_size);
        for (unsigned int i = 0; i < n_insertions; i++) {
            for (unsigned int j = 0; j < key_size; j++) {
                buffer[j] = TLB[(rand() % 16)];
            }
            buffer[key_size] = 0;
            string s = buffer;
            value = (TestValue *) trie->lookup(s);
            if (value == NULL) {
                value = new TestValue();
                trie->insert(s, value);
            } else {
                value->count += 1;
            }
        }
    }
    timer_trie.stop();
    cout << "stdlib: " + timer_std.str_time() << endl;
    cout << "trie: " + timer_trie.str_time() << endl;
    EXPECT_EQ(true, false);
}
