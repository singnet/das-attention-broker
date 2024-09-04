#include <cstdlib>
#include <cmath>

#include "gtest/gtest.h"
#include "test_utils.h"
#include "LeadershipBroker.h"

using namespace atom_space_node;

TEST(LeadershipBroker, basics) {

    try {
        LeadershipBroker::factory((LeadershipBrokerType) -1);
        FAIL() << "Expected exception";
    } catch(std::runtime_error const &error) {
    } catch(...) {
        FAIL() << "Expected std::runtime_error";
    }

    SingleMasterServer *leadership_broker = (SingleMasterServer *)
        LeadershipBroker::factory(LeadershipBrokerType::SINGLE_MASTER_SERVER);

    EXPECT_EQ(leadership_broker->leader_id(), "");
    EXPECT_FALSE(leadership_broker->has_leader());
    leadership_broker->start_leader_election("blah");
    EXPECT_EQ(leadership_broker->leader_id(), "blah");
    EXPECT_TRUE(leadership_broker->has_leader());
}
