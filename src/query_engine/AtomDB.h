#ifndef _QUERY_ENGINE_ATOMDB_H
#define _QUERY_ENGINE_ATOMDB_H

#include <vector>
#include <memory>
#include <hiredis_cluster/hircluster.h>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include "AtomDBAPITypes.h"

using namespace std;

namespace query_engine {


enum MONGODB_FIELD {
    ID = 0,
    size
};

/**
 *
 */
class AtomDB {

public:

    AtomDB();
    ~AtomDB();

    static string WILDCARD;
    static string REDIS_PATTERNS_PREFIX;
    static string MONGODB_DB_NAME;
    static string MONGODB_COLLECTION_NAME;
    static string MONGODB_FIELD_NAME[MONGODB_FIELD::size];

    static void initialize_statics() {
        WILDCARD = "*";
        REDIS_PATTERNS_PREFIX = "patterns";
        MONGODB_DB_NAME = "das";
        MONGODB_COLLECTION_NAME = "atoms";
        MONGODB_FIELD_NAME[MONGODB_FIELD::ID] = "_id";
    }

    shared_ptr<atomdb_api_types::HandleList> query_for_pattern(shared_ptr<char> pattern_handle);
    shared_ptr<atomdb_api_types::AtomDocument> get_atom_document(const char *handle);

private:

    bool cluster_flag;
    redisClusterContext *redis_cluster;
    redisContext *redis_single;
    mongocxx::client *mongodb_client;
    mongocxx::database mongodb;
    mongocxx::v_noabi::collection mongodb_collection;

    void redis_setup();
    void mongodb_setup();
};

} // namespace query_engine

#endif // _QUERY_ENGINE_ATOMDB_H
