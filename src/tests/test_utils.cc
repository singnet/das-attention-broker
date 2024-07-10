#include "expression_hasher.h"
#include "Utils.h"
#include "test_utils.h"

static char REVERSE_TLB[16] = {
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

string random_handle() {
    char buffer[HANDLE_HASH_SIZE];
    unsigned int key_size = HANDLE_HASH_SIZE - 1;
    for (unsigned int i = 0; i < key_size; i++) {
        buffer[i] = REVERSE_TLB[(rand() % 16)];
    }
    buffer[key_size] = 0;
    string s = buffer;
    return s;
}

string prefixed_random_handle(string prefix) {
    char buffer[HANDLE_HASH_SIZE];
    unsigned int key_size = HANDLE_HASH_SIZE - 1;
    for (unsigned int i = 0; i < key_size; i++) {
        if (i < prefix.size()) {
            buffer[i] = prefix[i];
        } else {
            buffer[i] = REVERSE_TLB[(rand() % 16)];
        }
    }
    buffer[key_size] = 0;
    string s = buffer;
    return s;
}

string *build_handle_space(unsigned int size) {
    string *answer = new string[size];
    for (unsigned int i = 0; i < size; i++) {
        answer[i] = random_handle();
    }
    return answer;
}
