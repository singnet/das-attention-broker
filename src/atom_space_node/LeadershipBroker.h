#ifndef _ATOM_SPACE_NODE_LEADERSHIPBROKER_H
#define _ATOM_SPACE_NODE_LEADERSHIPBROKER_H

#include "MessageBroker.h"

using namespace std;

namespace atom_space_node {

enum class LeadershipBrokerType {
    SINGLE_MASTER_SERVER
};


// -------------------------------------------------------------------------------------------------
// Abstract superclass

/**
 * Implements the algorithm for leader election.
 *
 * This is the abstract class defining the API used by AtomSpaceNodes to deal with leader election.
 * Users of the AtomSpaceNode module aren't supposed to interact with LeadershipBroker directly.
 */
class LeadershipBroker {

public:

    /**
     * Factory method for concrete subclasses.
     *
     * @param instance_type Defines which subclass should be used to instantiate the
     * LeadershipBroker.
     * @return An instance of the selected LeadershipBroker subclass.
     */
    static LeadershipBroker *factory(LeadershipBrokerType instance_type);

    /**
     * Destructor.
     */
    virtual ~LeadershipBroker();

    /**
     * Sets MessageBroker to be used to communicate with peers.
     *
     * @param message_broker The MessageBroker to be used to ciommunicate with peers.
     */
    void set_message_broker(MessageBroker *message_broker);

    /**
     * Returns the leader node ID.
     *
     * @return The leader node ID.
     */
    string leader_id();

    /**
     * Sets the leader node ID.
     *
     * @param leader_id The leader node ID.
     */
    void set_leader_id(const string &leader_id);

    /**
     * Return true iff a leader has been defined.
     */
    bool has_leader();

    // ----------------------------------------------------------------
    // Public abstract API

    /**
     * Starts an election to define the leader of the network.
     *
     * @param my_vote The vote casted by the hosting node to tghe leadership election.
     */
    virtual void start_leader_election(const string &my_vote) = 0;

protected:

    LeadershipBroker();

private:

    MessageBroker *message_broker;
    string network_leader_id;

};

// -------------------------------------------------------------------------------------------------
// Concrete subclasses

/**
 * Concrete implementation of a leadership selection algorithm based in a topology of one server
 * connected to N clients, where clients only know about the server which is always the leader of
 * the network.
 */
class SingleMasterServer : public LeadershipBroker {

public:

    /**
     * Basic constructor
     */
    SingleMasterServer();

    /**
     * Destructor
     */
    ~SingleMasterServer();


    // ----------------------------------------------------------------
    // Public LeadershipBroker abstract API

    void start_leader_election(const string &my_vote);
};


} // namespace atom_space_node

#endif // _ATOM_SPACE_NODE_LEADERSHIPBROKER_H

