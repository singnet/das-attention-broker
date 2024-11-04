#include "Utils.h"
#include "AtomDBAPITypes.h"

using namespace query_engine;
using namespace atomdb_api_types;
using namespace commons;

RedisSet::RedisSet(redisReply *reply) : HandleList() {
    this->redis_reply = reply;
    this->handles_size = reply->elements;
    this->handles = new char *[this->handles_size];
    for (unsigned int i = 0; i < this->handles_size; i++) {
        handles[i] = reply->element[i]->str;
    }
}

RedisSet::~RedisSet() {
    delete [] this->handles;
    freeReplyObject(this->redis_reply);
}

const char *RedisSet::get_handle(unsigned int index) {
    if (index > this->handles_size) {
        Utils::error("Handle index out of bounds: " + to_string(index) + " Answer array size: " + to_string(this->handles_size));
    }
    return handles[index];
}

unsigned int RedisSet::size() {
    return this->handles_size;
}


MongodbDocument::MongodbDocument(core::v1::optional<bsoncxx::v_noabi::document::value>& document) {
    this->document = document;
}

MongodbDocument::~MongodbDocument() {
}

const char *MongodbDocument::get(const string &key) {
    // Note for reference: .to_string() instead of .data() would return a std::string
    return ((*this->document)[key]).get_string().value.data();
}

const char *MongodbDocument::get(const string &array_key, unsigned int index) {
    // Note for reference: .to_string() instead of .data() would return a std::string
    return ((*this->document)[array_key]).get_array().value[index].get_string().value.data();
}
