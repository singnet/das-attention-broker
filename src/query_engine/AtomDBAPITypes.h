#ifndef _QUERY_ENGINE_ATOMDBAPITYPES_H
#define _QUERY_ENGINE_ATOMDBAPITYPES_H

#include <hiredis_cluster/hircluster.h>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include "Utils.h"

using namespace std;
using namespace commons;

namespace query_engine {
namespace atomdb_api_types {

/**
 *
 */
class HandleList {

public:

    HandleList() {}
    virtual ~HandleList() {}

    virtual const char *get_handle(unsigned int index) = 0;
    virtual unsigned int size() = 0;
};

class RedisSet : public HandleList {

public:

    RedisSet(redisReply *reply);
    ~RedisSet();

    const char *get_handle(unsigned int index);
    unsigned int size();

private:

    unsigned int handles_size;
    char **handles;
    redisReply *redis_reply;
};

class AtomDocument {

public:

    AtomDocument() {}
    virtual ~AtomDocument() {}

    virtual const char *get(const string &key) = 0;
    virtual const char *get(const string &array_key, unsigned int index) = 0;
    virtual unsigned int get_size(const string &array_key) = 0;
};

class MongodbDocument : public AtomDocument {

public:

    MongodbDocument(core::v1::optional<bsoncxx::v_noabi::document::value>& document);
    ~MongodbDocument();

    const char *get(const string &key);
    virtual const char *get(const string &array_key, unsigned int index);
    virtual unsigned int get_size(const string &array_key);

private:

    core::v1::optional<bsoncxx::v_noabi::document::value> document;
};

} // namespace atomdb_api_types
} // namespace query_engine

#endif // _QUERY_ENGINE_ATOMDBAPITYPES_H
