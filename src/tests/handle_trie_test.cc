#include <cstdlib>
#include <thread>
#include <vector>

#include "gtest/gtest.h"

#include "Utils.h"
#include "expression_hasher.h"
#include "HandleTrie.h"
#include "RequestSelector.h"
#include "test_utils.h"

#define HANDLE_SPACE_SIZE ((unsigned int) 100)

using namespace attention_broker_server;
using namespace std;

class TestValue: public HandleTrie::TrieValue {
    public:
        unsigned int count;
        TestValue(int count = 1) {
            this->count = count;
        }
        void merge(TrieValue *other) {

        }
};

class AccumulatorValue: public HandleTrie::TrieValue {
    public:
        unsigned int count;
        AccumulatorValue() {
            this->count = 1;
        }
        void merge(TrieValue *other) {
            count += ((AccumulatorValue *) other)->count;
        }
};

char R_TLB[16] = {
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

TEST(HandleTrieTest, basics) {
    
    HandleTrie trie(4);
    TestValue *value;

    trie.insert("ABCD", new TestValue(3));
    value = (TestValue *) trie.lookup("ABCD");
    EXPECT_TRUE(value != NULL);
    EXPECT_TRUE(value->count == 3);
    value = (TestValue *) trie.lookup("ABCX");
    EXPECT_TRUE(value == NULL);

    trie.insert("ABCX", new TestValue(4));
    value = (TestValue *) trie.lookup("ABCX");
    EXPECT_TRUE(value != NULL);
    EXPECT_TRUE(value->count == 4);
    value = (TestValue *) trie.lookup("ABCD");
    EXPECT_TRUE(value != NULL);
    EXPECT_TRUE(value->count == 3);

    trie.insert("ABXD", new TestValue(5));
    value = (TestValue *) trie.lookup("ABXD");
    EXPECT_TRUE(value != NULL);
    EXPECT_TRUE(value->count == 5);

    trie.insert("XBCD", new TestValue(6));
    value = (TestValue *) trie.lookup("XBCD");
    EXPECT_TRUE(value != NULL);
    EXPECT_TRUE(value->count == 6);

    trie.insert("AXCD", new TestValue(7));
    value = (TestValue *) trie.lookup("AXCD");
    EXPECT_TRUE(value != NULL);
    EXPECT_TRUE(value->count == 7);

    value = (TestValue *) trie.lookup("ABCD");
    EXPECT_TRUE(value != NULL);
    EXPECT_TRUE(value->count == 3);
    value = (TestValue *) trie.lookup("ABCX");
    EXPECT_TRUE(value != NULL);
    EXPECT_TRUE(value->count == 4);
    value = (TestValue *) trie.lookup("ABXD");
    EXPECT_TRUE(value != NULL);
    EXPECT_TRUE(value->count == 5);
    value = (TestValue *) trie.lookup("XBCD");
    EXPECT_TRUE(value != NULL);
    EXPECT_TRUE(value->count == 6);
    value = (TestValue *) trie.lookup("AXCD");
    EXPECT_TRUE(value != NULL);
    EXPECT_TRUE(value->count == 7);

    value = (TestValue *) trie.lookup("ABXX");
    EXPECT_TRUE(value == NULL);
    value = (TestValue *) trie.lookup("AXCX");
    EXPECT_TRUE(value == NULL);
    value = (TestValue *) trie.lookup("AXXD");
    EXPECT_TRUE(value == NULL);
    value = (TestValue *) trie.lookup("XBCX");
    EXPECT_TRUE(value == NULL);
    value = (TestValue *) trie.lookup("XBXD");
    EXPECT_TRUE(value == NULL);
    value = (TestValue *) trie.lookup("XXCD");
    EXPECT_TRUE(value == NULL);
}

TEST(HandleTrieTest, merge) {
    
    HandleTrie trie(4);

    trie.insert("ABCD", new AccumulatorValue());
    trie.insert("ABCX", new AccumulatorValue());
    trie.insert("ABXD", new AccumulatorValue());
    trie.insert("ABXX", new AccumulatorValue());
    trie.insert("AXCD", new AccumulatorValue());
    trie.insert("AXCX", new AccumulatorValue());
    trie.insert("AXXD", new AccumulatorValue());
    trie.insert("AXXX", new AccumulatorValue());
    trie.insert("XBCD", new AccumulatorValue());
    trie.insert("XBCX", new AccumulatorValue());
    trie.insert("XBXD", new AccumulatorValue());
    trie.insert("XBXX", new AccumulatorValue());
    trie.insert("XXCD", new AccumulatorValue());
    trie.insert("XXCX", new AccumulatorValue());
    trie.insert("XXXD", new AccumulatorValue());
    trie.insert("XXXX", new AccumulatorValue());

    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("ABCD"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("ABCX"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("ABXD"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("ABXX"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("AXCD"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("AXCX"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("AXXD"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("AXXX"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XBCD"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XBCX"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XBXD"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XBXX"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XXCD"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XXCX"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XXXD"))->count == 1);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XXXX"))->count == 1);

    trie.insert("ABXX", new AccumulatorValue());
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("ABXX"))->count == 2);
    trie.insert("XXCD", new AccumulatorValue());
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XXCD"))->count == 2);

    trie.insert("ABCD", new AccumulatorValue());
    trie.insert("ABCX", new AccumulatorValue());
    trie.insert("ABXD", new AccumulatorValue());
    trie.insert("ABXX", new AccumulatorValue());
    trie.insert("AXCD", new AccumulatorValue());
    trie.insert("AXCX", new AccumulatorValue());
    trie.insert("AXXD", new AccumulatorValue());
    trie.insert("AXXX", new AccumulatorValue());
    trie.insert("XBCD", new AccumulatorValue());
    trie.insert("XBCX", new AccumulatorValue());
    trie.insert("XBXD", new AccumulatorValue());
    trie.insert("XBXX", new AccumulatorValue());
    trie.insert("XXCD", new AccumulatorValue());
    trie.insert("XXCX", new AccumulatorValue());
    trie.insert("XXXD", new AccumulatorValue());
    trie.insert("XXXX", new AccumulatorValue());

    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("ABCD"))->count == 2);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("ABCX"))->count == 2);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("ABXD"))->count == 2);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("ABXX"))->count == 3);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("AXCD"))->count == 2);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("AXCX"))->count == 2);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("AXXD"))->count == 2);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("AXXX"))->count == 2);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XBCD"))->count == 2);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XBCX"))->count == 2);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XBXD"))->count == 2);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XBXX"))->count == 2);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XXCD"))->count == 3);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XXCX"))->count == 2);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XXXD"))->count == 2);
    EXPECT_TRUE(((AccumulatorValue *) trie.lookup("XXXX"))->count == 2);
}

TEST(HandleTrieTest, random_stress) {
    
    char buffer[1000];
    map<string, unsigned int> baseline;
    TestValue *value;

    for (unsigned int key_size: {2, 5, 10, 100}) {
        baseline.clear();
        HandleTrie *trie = new HandleTrie(key_size);
        for (unsigned int i = 0; i < 100000; i++) {
            for (unsigned int j = 0; j < key_size; j++) {
                buffer[j] = R_TLB[(rand() % 16)];
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
            EXPECT_TRUE(value != NULL);
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
                buffer[j] = R_TLB[(rand() % 16)];
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

TEST(HandleTrieTest, benchmark) {
    char buffer[1000];
    map<string, int> baseline;
    TestValue *value;
    StopWatch timer_std;
    StopWatch timer_trie;
    unsigned int n_insertions = 1000000;

    timer_std.start();
    for (unsigned int key_count: {1, 2, 5}) {
        unsigned int key_size = (HANDLE_HASH_SIZE - 1) * key_count;
        for (unsigned int i = 0; i < n_insertions; i++) {
            for (unsigned int j = 0; j < key_size; j++) {
                buffer[j] = R_TLB[(rand() % 16)];
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
    for (unsigned int key_count: {1, 2, 5}) {
        unsigned int key_size = (HANDLE_HASH_SIZE - 1) * key_count;
        HandleTrie *trie = new HandleTrie(key_size);
        for (unsigned int i = 0; i < n_insertions; i++) {
            for (unsigned int j = 0; j < key_size; j++) {
                buffer[j] = R_TLB[(rand() % 16)];
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
    cout << "=======================================================" << endl;
    cout << "stdlib: " + timer_std.str_time() << endl;
    cout << "trie: " + timer_trie.str_time() << endl;
    cout << "=======================================================" << endl;
    //EXPECT_EQ(true, false);
}

void producer(HandleTrie *trie, unsigned int n_insertions) {
    char buffer[1000];
    AccumulatorValue *value;
    unsigned int key_size = HANDLE_HASH_SIZE - 1;
    for (unsigned int i = 0; i < n_insertions; i++) {
        for (unsigned int j = 0; j < key_size; j++) {
            buffer[j] = R_TLB[(rand() % 16)];
        }
        buffer[key_size] = 0;
        string s = buffer;
        value = new AccumulatorValue();
        trie->insert(s, value);
    }
}

void visitor(HandleTrie *trie, unsigned int n_visits) {
    char buffer[1000];
    unsigned int key_size = HANDLE_HASH_SIZE - 1;
    for (unsigned int i = 0; i < n_visits; i++) {
        for (unsigned int j = 0; j < key_size; j++) {
            buffer[j] = R_TLB[(rand() % 16)];
        }
        buffer[key_size] = 0;
        string s = buffer;
        trie->lookup(s);
    }
}

void producer2(HandleTrie *trie, unsigned int n_insertions, string *handles) {
    AccumulatorValue *value;
    for (unsigned int i = 0; i < n_insertions; i++) {
        string s = handles[rand() % HANDLE_SPACE_SIZE];
        value = new AccumulatorValue();
        trie->insert(s, value);
    }
}

void visitor2(HandleTrie *trie, unsigned int n_visits, string *handles) {
    for (unsigned int i = 0; i < n_visits; i++) {
        string s = handles[rand() % HANDLE_SPACE_SIZE];
        trie->lookup(s);
    }
}

TEST(HandleTrieTest, multithread) {
    vector<thread *> producers;
    vector<thread *> visitors;
    unsigned int n_insertions = 10000;
    unsigned int n_visits = 10000;
    StopWatch timer;
    timer.start();
    for (int n_producers: {2, 10, 100}) {
        for (int n_visitors: {2, 10, 100}) {
            unsigned int key_size = HANDLE_HASH_SIZE - 1;
            HandleTrie *trie = new HandleTrie(key_size);
            producers.clear();
            visitors.clear();
            for (int i = 0; i < n_producers; i++) {
                producers.push_back(new thread(&producer, trie, n_insertions));
            }
            for (int i = 0; i < n_visitors; i++) {
                visitors.push_back(new thread(&visitor, trie, n_visits));
            }
            for (thread *t: producers) {
                t->join();
            }
            for (thread *t: visitors) {
                t->join();
            }
            delete trie;
        }
    }
    timer.stop();
}

TEST(HandleTrieTest, multithread_limited_handle_set) {
    string handles[HANDLE_SPACE_SIZE];
    for (unsigned int i = 0; i < HANDLE_SPACE_SIZE; i++) {
        handles[i] = random_handle();
    }
    vector<thread *> producers;
    vector<thread *> visitors;
    unsigned int n_insertions = 100000;
    unsigned int n_visits = 100000;
    for (int n_producers: {2, 10, 100}) {
        for (int n_visitors: {2, 10, 100}) {
            unsigned int key_size = HANDLE_HASH_SIZE - 1;
            HandleTrie *trie = new HandleTrie(key_size);
            for (int i = 0; i < n_producers; i++) {
                producers.push_back(new thread(&producer2, trie, n_insertions, handles));
            }
            for (int i = 0; i < n_visitors; i++) {
                visitors.push_back(new thread(&visitor2, trie, n_visits, handles));
            }
            for (thread *t: producers) {
                t->join();
            }
            for (thread *t: visitors) {
                t->join();
            }
            delete trie;
            producers.clear();
            visitors.clear();
        }
    }
}
