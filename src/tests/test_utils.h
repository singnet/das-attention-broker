#ifndef _ATTENTION_BROKER_SERVER_TESTS_TESTUTILS
#define _ATTENTION_BROKER_SERVER_TESTS_TESTUTILS

#include <string>
using namespace std;

string random_handle();
string prefixed_random_handle(string prefix);
string *build_handle_space(unsigned int size);

#endif
