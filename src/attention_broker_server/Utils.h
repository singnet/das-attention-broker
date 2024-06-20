#ifndef _ATTENTION_BROKER_SERVER_UTILS_H
#define _ATTENTION_BROKER_SERVER_UTILS_H

#include <string>

using namespace std;

namespace attention_broker_server {

/**
 *
 */
class Utils {

public:

    Utils();
    ~Utils();

    static void error(string msg);
    static bool flip_coin(double true_probability = 0.5);
};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_UTILS_H
