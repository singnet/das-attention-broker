#ifndef _QUERY_ELEMENT_LINKTEMPLATE_H
#define _QUERY_ELEMENT_LINKTEMPLATE_H

#include <cstring>

#include "QueryNode.h"
#include "Source.h"
#include "Iterator.h"
#include "And.h"
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
        cout << "XXXXX CONSTRUCTOR LinkTemplate::LinkTemplate() BEGIN: " << (unsigned long) this << endl;
        this->arity = ARITY;
        this->type = type;
        this->target_template = targets;
        this->fetch_finished = false;
        this->atom_document = NULL;
        this->local_answers = NULL;
        this->local_answers_size = 0;
        this->local_buffer_processor = NULL;
        bool wildcard_flag = (type == AtomDB::WILDCARD);
        this->handle_keys[0] = (wildcard_flag ?
            (char *) AtomDB::WILDCARD.c_str() :
            named_type_hash((char *) type.c_str()));
        for (unsigned int i = 1; i <= ARITY; i++) {
            // Note:
            // It's safe to get stored shared_ptr's raw pointer here because handle_keys[]
            // is used solely in this scope so it's guaranteed that handle will not be freed.
            if (targets[i - 1]->is_terminal()) {
                this->handle_keys[i] = ((Terminal *) targets[i - 1])->handle.get();
            } else {
                this->handle_keys[i] = (char *) AtomDB::WILDCARD.c_str();
                this->inner_template.push_back(targets[i - 1]);
            }
        }
        this->handle = shared_ptr<char>(composite_hash(this->handle_keys, ARITY + 1));
        if (! wildcard_flag) {
            free(this->handle_keys[0]);
        }
        // This is correct. id is not necessarily a handle but an identificator. It just happens that
        // we want the string for this identificator to be the same as the string representing the handle.
        this->id = this->handle.get();
        cout << "XXXXX CONSTRUCTOR LinkTemplate::LinkTemplate() END: " << (unsigned long) this << endl;
    }

    virtual ~LinkTemplate() {
        cout << "XXXXX DESTRUCTOR LinkTemplate::LinkTemplate() BEGIN: " << (unsigned long) this << endl;
        graceful_shutdown();
        local_answers_mutex.lock();
        if (local_answers_size > 0) {
            delete [] this->atom_document;
            delete [] this->local_answers;
            delete [] this->next_inner_answer;
        }
        local_answers_mutex.unlock();
        cout << "XXXXX DESTRUCTOR LinkTemplate::LinkTemplate() END: " << (unsigned long) this << endl;
    }

    virtual void graceful_shutdown() {
        cout << "XXXXX LinkTemplate::graceful_shutdown(): " << (unsigned long) this << endl;
        set_fetch_finished();
        if (this->local_buffer_processor != NULL) {
            this->local_buffer_processor->join();
            this->local_buffer_processor = NULL;
        }
        Source::graceful_shutdown();
    }

    void increment_local_answers_size() {
        local_answers_mutex.lock();
        this->local_answers_size++;
        local_answers_mutex.unlock();
    }

    unsigned int get_local_answers_size() {
        unsigned int answer;
        local_answers_mutex.lock();
        answer = this->local_answers_size;
        local_answers_mutex.unlock();
        return answer;
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
            // shared_ptr<atomdb_api_types::AtomDocument> atom_document[answer_count];
            this->atom_document = new shared_ptr<atomdb_api_types::AtomDocument>[answer_count];
            this->local_answers = new DASQueryAnswer *[answer_count];
            this->next_inner_answer = new unsigned int[answer_count];
            cout << "XXXXXXXXX fetch_links() 3" << endl;
            for (unsigned int i = 0; i < answer_count; i++) {
                this->atom_document[i] = db->get_atom_document(this->fetch_result->get_handle(i));
                DASQueryAnswer *query_answer = new DASQueryAnswer(
                    this->fetch_result->get_handle(i),
                    importance_list.list(i));
                const char *s = this->atom_document[i]->get("targets", 0);
                for (unsigned int j = 0; j < this->arity; j++) {
                    if (this->target_template[j]->is_terminal()) {
                        Terminal *terminal = (Terminal *) this->target_template[j];
                        if (terminal->is_variable) {
                            if (! query_answer->assignment.assign(
                                    terminal->name.c_str(),
                                    this->atom_document[i]->get("targets", j))) {
                                Utils::error("Error assigning variable: " + terminal->name + " a value: " + string(this->atom_document[i]->get("targets", j)));
                            }
                        }
                    }
                }
                cout << "XXXXXXXXX fetch_links() 4" << endl;
                if (this->inner_template.size() == 0) {
                    this->local_buffer.enqueue((void *) query_answer);
                } else {
                    this->local_answers[i] = query_answer;
                    this->next_inner_answer[i] = 0;
                    this->increment_local_answers_size();
                }
                cout << "XXXXXXXXX fetch_links() 5" << endl;
                if (is_fetch_finished()) {
                    break;
                }
            }
            cout << "XXXXXXXXX fetch_links() finishing query answers" << endl;
            if (this->inner_template.size() == 0) {
                set_fetch_finished();
            }
        }
        cout << "XXXXXXXXX fetch_links() END" << endl;
    }

    bool is_feasible(unsigned int index) {
        unsigned int inner_answers_size = inner_answers.size();
        unsigned int cursor = this->next_inner_answer[index];
        while (inner_answers_size > cursor) {
            if (this->inner_answers[cursor] != NULL) {
                bool passed_first_check = true;
                unsigned int arity = this->atom_document[index]->get_size("targets");
                unsigned int target_cursor = 0;
                for (unsigned int i = 0; i < arity; i++) {
                    // Note to revisor: pointer comparation is correct here
                    if (handle_keys[i + 1] == (char *) AtomDB::WILDCARD.c_str()) {
                        if (target_cursor > this->inner_answers[cursor]->handles_size) {
                            Utils::error("Invalid query answer in inner link template match");
                        }
                        if (strncmp(
                            this->atom_document[index]->get("targets", i), 
                            this->inner_answers[cursor]->handles[target_cursor++],
                            HANDLE_HASH_SIZE)) {

                            passed_first_check = false;
                            break;
                        }
                    }
                }
                if (passed_first_check && this->local_answers[index]->merge(this->inner_answers[cursor])) {
                    this->inner_answers[cursor] == NULL;
                    return true;
                }
            }
            this->next_inner_answer[index]++;
            cursor++;
        }
    }

    bool ingest_newly_arrived_answers() {
        bool flag = false;
        DASQueryAnswer *answer;
        cout << "XXXXXXXXXXXXXXXXXXXX ingest_newly_arrived_answers() " << endl;
        while ((answer = this->inner_template_iterator->pop()) != NULL) {
            this->inner_answers.push_back(answer);
            cout << "XXXXXXXXXXXXXXXXXXXX ingest_newly_arrived_answers() " << answer->handles_size << " " << answer->handles[0] << " " << answer->assignment.to_string() << endl;
            flag = true;
        }
        return flag;
    }

    void local_buffer_processor_method() {
        if (this->inner_template.size() == 0) {
            while (! (this->is_fetch_finished() && this->local_buffer.empty())) {
                DASQueryAnswer *query_answer;
                while ((query_answer = (DASQueryAnswer *) this->local_buffer.dequeue()) != NULL) {
                    cout << "XXXXXXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
                    this->output_buffer->add_query_answer(query_answer);
                }
                Utils::sleep();
            }
        } else {
            while (! this->is_fetch_finished()) {
                if (ingest_newly_arrived_answers()) {
                    unsigned int size = get_local_answers_size();
                    for (unsigned int i = 0; i < size; i++) {
                        if (this->local_answers[i] != NULL) {
                            if (is_feasible(i)) {
                                this->output_buffer->add_query_answer(this->local_answers[i]);
                                this->local_answers[i] = NULL;
                            }
                        }
                    }
                } else {
                    Utils::sleep();
                }
                bool finished_flag = true;
                for (unsigned int i = 0; i < size; i++) {
                    if (this->local_answers[i] != NULL) {
                        finished_flag = false;
                        break;
                    }
                }
                if (finished_flag) {
                    set_fetch_finished();
                }
            }
        }
        this->output_buffer->query_answers_finished();
    }

    virtual void setup_buffers() {
        cout << "XXXXX LinkTemplate::setup_buffers() BEGIN: " << (unsigned long) this << endl;
        cout << "XXXXX id: " << this->id << endl;
        cout << "XXXXX subsequent id: " << this->subsequent_id << endl;

        Source::setup_buffers();
        if (this->inner_template.size() > 0) {
            switch(this->inner_template.size()) {
                case 1: {
                    cout << "XXXXX case 1: " << endl;
                    this->inner_template_iterator = shared_ptr<Iterator>(new Iterator(
                        inner_template[0]
                    ));
                    break;
                }
                case 2: {
                    this->inner_template_iterator = shared_ptr<Iterator>(new Iterator(
                        new And<2>({
                            inner_template[0],
                            inner_template[1]
                        }),
                        true
                    ));
                    break;
                }
                case 3: {
                    this->inner_template_iterator = shared_ptr<Iterator>(new Iterator(
                        new And<3>({
                            inner_template[0],
                            inner_template[1],
                            inner_template[2]
                        }),
                        true
                    ));
                    break;
                }
                case 4: {
                    this->inner_template_iterator = shared_ptr<Iterator>(new Iterator(
                        new And<4>({
                            inner_template[0],
                            inner_template[1],
                            inner_template[2],
                            inner_template[3]
                        }),
                        true
                    ));
                    break;
                }
                default: {
                    Utils::error("Invalid number of inner templates (> 4) in link template.");
                }
            }
        }
        this->local_buffer_processor = new thread(&LinkTemplate::local_buffer_processor_method, this);
        fetch_links();
        cout << "XXXXX LinkTemplate::setup_buffers() END: " << (unsigned long) this << endl;
    }

private:

    string type;
    array<QueryElement *, ARITY> target_template;
    unsigned int arity;
    shared_ptr<char> handle;
    char *handle_keys[ARITY + 1];
    shared_ptr<atomdb_api_types::HandleList> fetch_result;
    vector<shared_ptr<atomdb_api_types::AtomDocument>> atom_documents;
    vector<QueryElement *> inner_template;
    RequestQueue local_buffer;
    thread *local_buffer_processor;
    bool fetch_finished;
    mutex fetch_finished_mutex;
    shared_ptr<QueryNodeServer> target_buffer[ARITY];
    shared_ptr<Iterator> inner_template_iterator;
    shared_ptr<atomdb_api_types::AtomDocument> *atom_document;
    DASQueryAnswer **local_answers;
    unsigned int *next_inner_answer;
    vector<DASQueryAnswer *> inner_answers;
    unsigned int local_answers_size;
    mutex local_answers_mutex;

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
