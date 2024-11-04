#ifndef _QUERY_ELEMENT_LINKTEMPLATE_H
#define _QUERY_ELEMENT_LINKTEMPLATE_H

#include "QueryNode.h"
#include "Source.h"
#include "Terminal.h"
#include "AtomDBSingleton.h"
#include "AtomDBAPITypes.h"
#include "DASQueryAnswer.h"
#include "expression_hasher.h"
#include "RequestQueue.h"

//include "common.pb.h"
#include "AttentionBrokerServer.h"
#include "attention_broker.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include "attention_broker.pb.h"

using namespace std;
using namespace query_engine;
using namespace attention_broker_server;

namespace query_element {

/**
 *
 */
template <unsigned int ARITY>
class LinkTemplate : public Source {

public:

    LinkTemplate(const string &type, const array<QueryElement *, ARITY> &targets) {
        this->arity = ARITY;
        this->type = type;
        this->target_template = targets;
        this->is_leaf = true;
        this->fetch_finished = false;
        char *handle_keys[ARITY + 1];
        bool wildcard_flag = (type == AtomDB::WILDCARD);
        handle_keys[0] = (wildcard_flag ?
            (char *) AtomDB::WILDCARD.c_str() :
            named_type_hash((char *) type.c_str()));
        for (unsigned int i = 1; i <= ARITY; i++) {
            // Note:
            // It's safe to get stored shared_ptr's raw pointer here because handle_keys[]
            // is used solely in this scope so it's guaranteed that handle will not be freed.
            if (targets[i - 1]->is_terminal()) {
                handle_keys[i] = ((Terminal *) targets[i - 1])->handle.get();
            } else {
                this->is_leaf = false;
                handle_keys[i] = (char *) AtomDB::WILDCARD.c_str();
            }
        }
        this->handle = shared_ptr<char>(composite_hash(handle_keys, ARITY + 1));
        if (! wildcard_flag) {
            free(handle_keys[0]);
        }
        // This is correct. id is not necessarily a handle but an identificator. It just happens that
        // we want the string for this identificator to be the same as the string representing the handle.
        this->id = this->handle.get();
    }

    virtual ~LinkTemplate() {
    }

    void fetch_links() {
        cout << "XXXXXXXXX fetch_links() BEGIN" << endl;
        shared_ptr<AtomDB> db = AtomDBSingleton::get_instance();
        this->fetch_result = db->query_for_pattern(this->handle);
        unsigned int answer_count = this->fetch_result->size();
        cout << "XXXXXXXXX fetch_links() 1" << endl;
        cout << "XXXXXXXXX fetch_links() answer_count: " << answer_count << endl;
        if (answer_count > 0) {
            cout << "XXXXXXXXX fetch_links() 2" << endl;
            dasproto::HandleList handle_list;
            for (unsigned int i = 0; i < answer_count; i++) {
                handle_list.add_list(this->fetch_result->get_handle(i));
            }
            dasproto::ImportanceList importance_list;
            grpc::ClientContext context;
            auto stub = dasproto::AttentionBroker::NewStub(grpc::CreateChannel(
                this->attention_broker_address, 
                grpc::InsecureChannelCredentials()));
            stub->get_importance(&context, handle_list, &importance_list);
            // TODO Remove [] of atom_document
            shared_ptr<atomdb_api_types::AtomDocument> atom_document[answer_count];
            cout << "XXXXXXXXX fetch_links() 3" << endl;
            for (unsigned int i = 0; i < answer_count; i++) {
                atom_document[i] = db->get_atom_document(this->fetch_result->get_handle(i));
                DASQueryAnswer *query_answer = new DASQueryAnswer(
                    this->fetch_result->get_handle(i),
                    importance_list.list(i));
                const char *s = atom_document[i]->get("targets", 0);
                for (unsigned int j = 0; j < this->arity; j++) {
                    if (this->target_template[j]->is_terminal()) {
                        Terminal *terminal = (Terminal *) this->target_template[j];
                        if (terminal->is_variable) {
                            if (! query_answer->assignment.assign(
                                    terminal->name.c_str(),
                                    atom_document[i]->get("targets", j))) {
                                Utils::error("Error assigning variable: " + terminal->name + " a value: " + string(atom_document[i]->get("targets", j)));
                            }
                        }
                    }
                }
                cout << "XXXXXXXXX fetch_links() 4" << endl;
                //this->output_buffer->add_query_answer(query_answer);
                this->local_buffer.enqueue((void *) query_answer);
                cout << "XXXXXXXXX fetch_links() 5" << endl;
            }
            cout << "XXXXXXXXX fetch_links() finishing query answers" << endl;
            //query_answers_finished();
            set_fetch_finished();
        }
        cout << "XXXXXXXXX fetch_links() END" << endl;
    }

    void local_buffer_processor_method() {
        while (! (this->is_fetch_finished() && this->local_buffer.empty())) {
            DASQueryAnswer *query_answer;
            while ((query_answer = (DASQueryAnswer *) this->local_buffer.dequeue()) != NULL) {
                cout << "XXXXXXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
                this->output_buffer->add_query_answer(query_answer);
            }
            Utils::sleep();
        }
        this->output_buffer->query_answers_finished();
    }

    virtual void setup_buffers() {
        Source::setup_buffers();
        if (! this->is_leaf) {
            for (unsigned int i = 0; i < ARITY; i++) {
                string id = this->id + "_target_" + to_string(i);
                string server_id = id + "_server";
                string client_id = id + "_client";
                target_buffer[i] = shared_ptr<QueryNodeServer>(new QueryNodeServer(server_id));
                if (! this->target_template[i]->is_terminal()) {
                    cout << "XXXXXXXXXXXXX setup_buffers() Setting inner template buffers" << endl;
                    this->target_template[i]->id = client_id;
                    this->target_template[i]->subsequent_id = server_id;
                    this->target_template[i]->setup_buffers();
                }
            }
        }
        this->local_buffer_processor = new thread(&LinkTemplate::local_buffer_processor_method, this);
        fetch_links();
    }

private:

    string type;
    array<QueryElement *, ARITY> target_template;
    unsigned int arity;
    shared_ptr<char> handle;
    shared_ptr<atomdb_api_types::HandleList> fetch_result;
    vector<shared_ptr<atomdb_api_types::AtomDocument>> atom_documents;
    bool is_leaf;
    RequestQueue local_buffer;
    thread *local_buffer_processor;
    bool fetch_finished;
    mutex fetch_finished_mutex;
    shared_ptr<QueryNodeServer> target_buffer[ARITY];

    void set_fetch_finished() {
        fetch_finished_mutex.lock();
        this->fetch_finished = true;
        fetch_finished_mutex.unlock();
    }

    bool is_fetch_finished() {
        bool answer;
        fetch_finished_mutex.lock();
        answer = this->fetch_finished;
        fetch_finished_mutex.unlock();
        return answer;
    }
};

} // namespace query_element

#endif // _QUERY_ELEMENT_LINKTEMPLATE_H
