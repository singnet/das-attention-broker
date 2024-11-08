#include <iostream>
#include <algorithm>
#include <string>
#include <memory>
#include "AtomDB.h"
#include "Utils.h"

using namespace query_engine;
using namespace commons;

string AtomDB::WILDCARD;
string AtomDB::REDIS_PATTERNS_PREFIX;
string AtomDB::MONGODB_DB_NAME;
string AtomDB::MONGODB_COLLECTION_NAME;
string AtomDB::MONGODB_FIELD_NAME[MONGODB_FIELD::size];

AtomDB::AtomDB() {
    redis_setup();
    mongodb_setup();
}

AtomDB::~AtomDB() {
    if (this->redis_cluster != NULL) {
        redisClusterFree(this->redis_cluster);
    }
    if (this->redis_single != NULL) {
        redisFree(this->redis_single);
    }
    delete this->mongodb_client;
}

void AtomDB::redis_setup() {

    string host = Utils::get_environment("DAS_REDIS_HOSTNAME");
    string port = Utils::get_environment("DAS_REDIS_PORT");
    string address = host + ":" + port;
    string cluster = Utils::get_environment("DAS_USE_REDIS_CLUSTER");
    std::transform(cluster.begin(), cluster.end(), cluster.begin(), ::toupper);
    this->cluster_flag = (cluster == "TRUE");

    if (host == "" || port == "") {
        Utils::error("You need to set Redis access info as environment variables: DAS_REDIS_HOSTNAME, DAS_REDIS_PORT and DAS_USE_REDIS_CLUSTER");
    }
    string cluster_tag = (this->cluster_flag ? "CLUSTER" : "NON-CLUSTER");

    if (this->cluster_flag) {
        this->redis_cluster = redisClusterConnect(address.c_str(), 0);
        this->redis_single = NULL;
    } else {
        this->redis_single = redisConnect(host.c_str(), stoi(port));
        this->redis_cluster = NULL;
    }

    if (this->redis_cluster == NULL && this->redis_single == NULL) {
        Utils::error("Connection error.");
    } else if ((! this->cluster_flag) && this->redis_single->err) {
        Utils::error("Redis error: " + string(this->redis_single->errstr));
    } else if (this->cluster_flag && this->redis_cluster->err) {
        Utils::error("Redis cluster error: " + string(this->redis_cluster->errstr));
    } else {
        cout << "Connected to (" << cluster_tag << ") Redis at " << address << endl;
    }
}

void AtomDB::mongodb_setup() {

    string host = Utils::get_environment("DAS_MONGODB_HOSTNAME");
    string port = Utils::get_environment("DAS_MONGODB_PORT");
    string user = Utils::get_environment("DAS_MONGODB_USERNAME");
    string password = Utils::get_environment("DAS_MONGODB_PASSWORD");
    if (host == "" || port == "" || user == "" || password == "") {
        Utils::error(string("You need to set MongoDB access info as environment variables: ") + \
            "DAS_MONGODB_HOSTNAME, DAS_MONGODB_PORT, DAS_MONGODB_USERNAME and DAS_MONGODB_PASSWORD");
    }
    string address = host + ":" + port;
    string url = "mongodb://" + user + ":" + password + "@" + address;

    try {
        mongocxx::instance instance;
        auto uri = mongocxx::uri{url};
        this->mongodb_client = new mongocxx::client(uri);
        this->mongodb = (*this->mongodb_client)[MONGODB_DB_NAME];
        const auto ping_cmd = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("ping", 1));
        this->mongodb.run_command(ping_cmd.view());
        this->mongodb_collection = this->mongodb[MONGODB_COLLECTION_NAME];
        auto atom_count = this->mongodb_collection.count_documents({});
        std::cout << "Connected to MongoDB at " << address << " Atom count: " << atom_count << endl;
    } catch (const std::exception& e) {
        Utils::error(e.what());
    }
}

shared_ptr<atomdb_api_types::HandleList> AtomDB::query_for_pattern(shared_ptr<char> pattern_handle) {
    redisReply *reply = (redisReply *) redisCommand(this->redis_single, "SMEMBERS %s:%s", REDIS_PATTERNS_PREFIX.c_str(), pattern_handle.get());
    if (reply == NULL) {
        Utils::error("Redis error");
    }
    if (reply->type != REDIS_REPLY_SET && reply->type != REDIS_REPLY_ARRAY) {
        Utils::error("Invalid Redis response");
    }
    // NOTE: Intentionally, we aren't destroying 'reply' objects.'reply' objects are destroyed in ~RedisSet().
    return shared_ptr<atomdb_api_types::RedisSet>(new atomdb_api_types::RedisSet(reply));
}

shared_ptr<atomdb_api_types::AtomDocument> AtomDB::get_atom_document(const char *handle) {
    auto reply = this->mongodb_collection.find_one(
        bsoncxx::v_noabi::builder::basic::make_document(
            bsoncxx::v_noabi::builder::basic::kvp(MONGODB_FIELD_NAME[MONGODB_FIELD::ID], handle)));
    //cout << bsoncxx::to_json(*reply) << endl; // Note to reviewer: please let this dead code here
    return shared_ptr<atomdb_api_types::MongodbDocument>(new atomdb_api_types::MongodbDocument(reply));
}