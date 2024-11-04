#ifndef _QUERY_ENGINE_ATOMDBSINGLETON_H
#define _QUERY_ENGINE_ATOMDBSINGLETON_H

#include <memory>
#include "AtomDB.h"

using namespace std;

namespace query_engine {

/**
 *
 */
class AtomDBSingleton {

public:

    ~AtomDBSingleton() {}
    static void init();
    static shared_ptr<AtomDB> get_instance();

private:

    AtomDBSingleton() {}
    static bool initialized;
    static shared_ptr<AtomDB> atom_db;
};

} // namespace query_engine

#endif // _QUERY_ENGINE_ATOMDBSINGLETON_H
