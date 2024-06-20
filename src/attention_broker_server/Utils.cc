#include <cstdlib>
#include <cmath>
#include <stdexcept>

#include "Utils.h"

using namespace attention_broker_server;

// --------------------------------------------------------------------------------
// Public methods

Utils::Utils() {
}

Utils::~Utils() {
}

void Utils::error(string msg) {
    throw runtime_error(msg);
}

bool Utils::flip_coin(double true_probability) {
    long f = 1000;
    return (rand() % f) < lround(true_probability * f);
}
