#include <cstdlib>
#include <cstring>
#include "gtest/gtest.h"

#include "DASQueryAnswer.h"
#include "test_utils.h"

using namespace query_engine;

TEST(DASQueryAnswer, assignments_basics) {

    Assignment mapping0;

    // Tests assign()
    Assignment mapping1;
    EXPECT_TRUE(mapping1.assign("v1", "1"));
    EXPECT_TRUE(mapping1.assign("v2", "2"));
    EXPECT_TRUE(mapping1.assign("v2", "2"));
    EXPECT_FALSE(mapping1.assign("v2", "3"));
    Assignment mapping2;
    EXPECT_TRUE(mapping1.assign("v1", "1"));
    EXPECT_TRUE(mapping1.assign("v3", "3"));
    Assignment mapping3;
    EXPECT_TRUE(mapping3.assign("v1", "1"));
    EXPECT_TRUE(mapping3.assign("v2", "3"));

    // Tests get()
    EXPECT_TRUE(strcmp(mapping1.get("v1"), "1") == 0);
    EXPECT_TRUE(strcmp(mapping1.get("v2"), "2") == 0);
    EXPECT_TRUE(mapping1.get("blah") == NULL);
    EXPECT_TRUE(mapping1.get("v11") == NULL);
    EXPECT_TRUE(mapping1.get("v") == NULL);
    EXPECT_TRUE(mapping1.get("") == NULL);

    // Tests is_compatible()
    EXPECT_TRUE(mapping1.is_compatible(mapping0));
    EXPECT_TRUE(mapping2.is_compatible(mapping0));
    EXPECT_TRUE(mapping3.is_compatible(mapping0));
    EXPECT_TRUE(mapping0.is_compatible(mapping1));
    EXPECT_TRUE(mapping0.is_compatible(mapping2));
    EXPECT_TRUE(mapping0.is_compatible(mapping3));
    EXPECT_TRUE(mapping1.is_compatible(mapping2));
    EXPECT_TRUE(mapping2.is_compatible(mapping1));
    EXPECT_TRUE(mapping2.is_compatible(mapping3));
    EXPECT_TRUE(mapping3.is_compatible(mapping2));
    EXPECT_FALSE(mapping1.is_compatible(mapping3));
    EXPECT_FALSE(mapping3.is_compatible(mapping1));

    // Tests copy_from()
    Assignment mapping4;
    mapping4.copy_from(mapping1);
    EXPECT_TRUE(strcmp(mapping4.get("v1"), "1") == 0);
    EXPECT_TRUE(strcmp(mapping4.get("v2"), "2") == 0);
    EXPECT_TRUE(mapping4.is_compatible(mapping2));
    EXPECT_TRUE(mapping2.is_compatible(mapping4));
    EXPECT_FALSE(mapping4.is_compatible(mapping3));
    EXPECT_FALSE(mapping3.is_compatible(mapping4));

    // Tests add_assignments()
    mapping4.add_assignments(mapping1);
    mapping4.add_assignments(mapping2);
    EXPECT_TRUE(strcmp(mapping4.get("v1"), "1") == 0);
    EXPECT_TRUE(strcmp(mapping4.get("v2"), "2") == 0);
    EXPECT_TRUE(strcmp(mapping4.get("v3"), "3") == 0);
    EXPECT_TRUE(mapping1.is_compatible(mapping4));
    EXPECT_TRUE(mapping2.is_compatible(mapping4));
    EXPECT_FALSE(mapping3.is_compatible(mapping4));
    EXPECT_TRUE(mapping4.is_compatible(mapping1));
    EXPECT_TRUE(mapping4.is_compatible(mapping2));
    EXPECT_FALSE(mapping4.is_compatible(mapping3));

    // Tests to_string():
    EXPECT_TRUE(mapping0.to_string() != "");
    EXPECT_TRUE(mapping1.to_string() != "");
    EXPECT_TRUE(mapping4.to_string() != "");
}

TEST(DASQueryAnswer, query_answer_basics) {

    // Tests add_handle()
    DASQueryAnswer query_answer1("h1", 0);
    query_answer1.assignment.assign("v1", "1");
    EXPECT_EQ(query_answer1.handles_size, 1);
    EXPECT_TRUE(strcmp(query_answer1.handles[0], "h1") == 0);
    query_answer1.add_handle("hx");
    EXPECT_EQ(query_answer1.handles_size, 2);
    EXPECT_TRUE(strcmp(query_answer1.handles[0], "h1") == 0);
    EXPECT_TRUE(strcmp(query_answer1.handles[1], "hx") == 0);

    // Tests merge()
    DASQueryAnswer query_answer2("h2", 0);
    query_answer2.assignment.assign("v2", "2");
    query_answer2.add_handle("hx");
    query_answer2.merge(&query_answer1);
    EXPECT_EQ(query_answer2.handles_size, 3);
    EXPECT_TRUE(strcmp(query_answer2.handles[0], "h2") == 0);
    EXPECT_TRUE(strcmp(query_answer2.handles[1], "hx") == 0);
    EXPECT_TRUE(strcmp(query_answer2.handles[2], "h1") == 0);
    EXPECT_FALSE(query_answer2.assignment.assign("v1", "x"));
    EXPECT_FALSE(query_answer2.assignment.assign("v2", "x"));
    EXPECT_TRUE(query_answer2.assignment.assign("v3", "x"));

    // Tests copy()
    DASQueryAnswer *query_answer3 = DASQueryAnswer::copy(&query_answer2);
    EXPECT_EQ(query_answer3->handles_size, 3);
    EXPECT_TRUE(strcmp(query_answer3->handles[0], "h2") == 0);
    EXPECT_TRUE(strcmp(query_answer3->handles[1], "hx") == 0);
    EXPECT_TRUE(strcmp(query_answer3->handles[2], "h1") == 0);
    EXPECT_FALSE(query_answer3->assignment.assign("v1", "x"));
    EXPECT_FALSE(query_answer3->assignment.assign("v2", "x"));
    EXPECT_FALSE(query_answer3->assignment.assign("v3", "y"));
    EXPECT_TRUE(query_answer3->assignment.assign("v4", "x"));
}
