#include <stdlib.h>
#include "ctest.h"
#include "../graph.h"

CTEST_DATA(node_tests) {
  Graph* graph;
};

CTEST_SETUP(node_tests) {
    data->graph = malloc(sizeof(Graph));
    ragedb_clear(data->graph);
    ragedb_node_add_empty(data->graph, "Node", "empty");
    ragedb_node_add(data->graph, "Node", "existing", "{ \"name\":\"max\", \"email\":\"maxdemarzi@hotmail.com\" }");
}

CTEST_TEARDOWN(node_tests) {
    ragedb_close(data->graph);
}

CTEST2(node_tests, shouldAddNode) {
    long long int created = ragedb_node_add_empty(data->graph, "Node", "key");
    ASSERT_NOT_EQUAL(0, created);
    ASSERT_STR("Node", ragedb_node_property_get_string(data->graph, "Node", "key", "~label"));
    ASSERT_STR("key", ragedb_node_property_get_string(data->graph, "Node", "key", "~key"));
    ASSERT_EQUAL(created, ragedb_node_property_get_integer(data->graph, "Node", "key", "~id"));
}

CTEST2(node_tests, shouldAddNodeWithProperties) {
    long long int created = ragedb_node_add(data->graph, "Node", "key",
            "{ \"name\":\"max\", \"email\":\"maxdemarzi@hotmail.com\" }");
    ASSERT_NOT_EQUAL(0, created);
    ASSERT_STR("Node", ragedb_node_property_get_string(data->graph, "Node", "key", "~label"));
    ASSERT_STR("key", ragedb_node_property_get_string(data->graph, "Node", "key", "~key"));
    ASSERT_EQUAL(created, ragedb_node_property_get_integer(data->graph, "Node", "key", "~id"));
    ASSERT_STR("max", ragedb_node_property_get_string(data->graph, "Node", "key", "name"));
    ASSERT_STR("maxdemarzi@hotmail.com", ragedb_node_property_get_string(data->graph, "Node", "key", "email"));
}

CTEST2(node_tests, shouldRemoveNode) {
    long long int created = ragedb_node_add(data->graph, "Node", "key",
            "{ \"name\":\"max\", \"email\":\"maxdemarzi@hotmail.com\" }");
    ASSERT_NOT_EQUAL(0, created);
    bool removed = ragedb_node_remove(data->graph, "Node", "key");
    ASSERT_TRUE(removed);
    char* label = "Node";
    created = ragedb_node_add(data->graph, label, "key", "{ \"name\":\"xam\", \"email\":\"izramedxam@hotmail.com\" }");
    ASSERT_NOT_EQUAL(0, created);
    ASSERT_STR("xam", ragedb_node_property_get_string(data->graph, "Node", "key", "name"));
}

CTEST2(node_tests, shouldRemoveNodeInMiddle) {
    long long int node1Id = ragedb_node_add(data->graph, "Node", "one","{ \"id\":\"node1\" }");
    long long int node2Id = ragedb_node_add(data->graph, "Node", "two","{ \"id\":\"node2\" }");
    long long int node3Id = ragedb_node_add(data->graph, "Node", "three","{ \"id\":\"node3\" }");
    ASSERT_STR("node1", ragedb_node_property_get_string(data->graph, "Node", "one", "id"));
    ASSERT_STR("node2", ragedb_node_property_get_string(data->graph, "Node", "two", "id"));
    ASSERT_STR("node3", ragedb_node_property_get_string(data->graph, "Node", "three", "id"));
    ragedb_relationship_add_empty(data->graph, "FRIENDS", "Node", "one", "Node", "two");
    ragedb_relationship_add_empty(data->graph, "FRIENDS", "Node", "one", "Node", "three");
    ragedb_relationship_add_empty(data->graph, "FRIENDS", "Node", "two", "Node", "one");
    ASSERT_NOT_EQUAL(0, node2Id);
    bool removed = ragedb_node_remove(data->graph, "Node", "two");
    ASSERT_TRUE(removed);
    long long int node4Id = ragedb_node_add(data->graph, "Node", "four","{ \"id\":\"node4\" }");
    ASSERT_EQUAL(node2Id, node4Id);
    ASSERT_STR("node4", ragedb_node_property_get_string(data->graph, "Node", "four", "id"));
    ASSERT_EQUAL(node2Id, ragedb_node_property_get_integer(data->graph, "Node", "four", "~id"));
}

CTEST2(node_tests, shouldRemoveNodeRelationships) {
    long long int node1Id = ragedb_node_add(data->graph, "Node", "one","{ \"id\":\"node1\" }");
    long long int node2Id = ragedb_node_add(data->graph, "Node", "two","{ \"id\":\"node2\" }");
    long long int node3Id = ragedb_node_add(data->graph, "Node", "three","{ \"id\":\"node3\" }");

    ASSERT_NOT_EQUAL(0, node2Id);
    long long int rel1Id = ragedb_relationship_add_empty(data->graph, "FRIENDS", "Node", "one", "Node", "two");
    long long int rel2Id = ragedb_relationship_add_empty(data->graph, "FRIENDS", "Node", "three", "Node", "one");

    long long int count = ragedb_relationship_type_get_count(data->graph, "FRIENDS");
    ASSERT_EQUAL(2, count);

    bool removed = ragedb_node_remove(data->graph, "Node", "one");
    ASSERT_TRUE(removed);

    count = ragedb_relationship_type_get_count(data->graph, "FRIENDS");
    ASSERT_EQUAL(0, count);
}

CTEST2(node_tests, shouldAddNodeWithObjectProperties) {
    long long int created = ragedb_node_add(data->graph, "Node", "key",
            "{ \"name\":\"max\", \"email\":\"maxdemarzi@hotmail.com\", \"stats\":{ \"age\":40,\"weight\":230 } }");
    ASSERT_NOT_EQUAL(0, created);
    ASSERT_STR("Node", ragedb_node_property_get_string(data->graph, "Node", "key", "~label"));
    ASSERT_STR("key", ragedb_node_property_get_string(data->graph, "Node", "key", "~key"));
    ASSERT_EQUAL(created, ragedb_node_property_get_integer(data->graph, "Node", "key", "~id"));
    ASSERT_STR("max", ragedb_node_property_get_string(data->graph, "Node", "key", "name"));
    ASSERT_STR("maxdemarzi@hotmail.com", ragedb_node_property_get_string(data->graph, "Node", "key", "email"));

    cJSON stats = ragedb_node_property_get_object(data->graph, "Node", "key", "stats");
    ASSERT_EQUAL(40, ragedb_property_get_integer(&stats, "age"));
    ASSERT_EQUAL(230, ragedb_property_get_integer(&stats, "weight"));
}

// todo: Deal with Array Properties. For reference see:
// https://spacesciencesoftware.wordpress.com/2013/09/10/a-good-way-to-read-json-with-c/
// https://stackoverflow.com/questions/16900874/using-cjson-to-read-in-a-json-array

CTEST2(node_tests, shouldGetEmptyNode) {
    cJSON empty = ragedb_node_get(data->graph, "Node", "empty");
    int nodeId = ragedb_property_get_integer(&empty, "~id");
    ASSERT_STR("Node", ragedb_node_property_get_string(data->graph, "Node", "empty", "~label"));
    ASSERT_STR("empty", ragedb_node_property_get_string(data->graph, "Node", "empty", "~key"));
    ASSERT_EQUAL(nodeId, ragedb_node_property_get_integer(data->graph, "Node", "empty", "~id"));
}

CTEST2(node_tests, shouldGetNodeWithProperties) {
    cJSON existing = ragedb_node_get(data->graph, "Node", "existing");
    int nodeId = ragedb_property_get_integer(&existing, "~id");
    ASSERT_STR("Node", ragedb_property_get_string(&existing, "~label"));
    ASSERT_STR("existing", ragedb_property_get_string(&existing, "~key"));
    ASSERT_EQUAL(nodeId, ragedb_property_get_integer(&existing, "~id"));
}

CTEST2(node_tests, shouldGetNodeId) {
    long long int existing = ragedb_node_get_id(data->graph, "Node", "existing");
    ASSERT_EQUAL(1, existing);
    long long int empty = ragedb_node_get_id(data->graph, "Node", "empty");
    ASSERT_EQUAL(0, empty);
}
CTEST2(node_tests, shouldNotAddNodeAlreadyThere) {
    long long int created = ragedb_node_add(data->graph, "Node", "key",
            "{ \"name\":\"max\", \"email\":\"maxdemarzi@hotmail.com\", \"stats\":{ \"age\":40,\"weight\":230 } }");
    ASSERT_NOT_EQUAL(0, created);
    created = ragedb_node_add(data->graph, "Node", "key",
            "{ \"name\":\"max\", \"email\":\"maxdemarzi@hotmail.com\", \"stats\":{ \"age\":40,\"weight\":230 } }");
    ASSERT_EQUAL(-1, created);
}

CTEST2(node_tests, shouldNotGetNodeNotThere) {
    cJSON not_existing = ragedb_node_get(data->graph, "Node", "not_existing");
    ASSERT_NULL(not_existing.string);
}

CTEST2(node_tests, shouldNotGetNodeLabelNotThere) {
    cJSON not_existing = ragedb_node_get(data->graph, "UnknownLabel", "not_existing");
    ASSERT_NULL(not_existing.string);
}


CTEST2(node_tests, shouldNotRemoveNodeNotThere) {
    bool not_existing = ragedb_node_remove(data->graph, "Node", "not_existing");
    ASSERT_FALSE(not_existing);
}

CTEST2(node_tests, shouldNotGetNodeIdOfNodeNotThere) {
    long long int not_existing = ragedb_node_get_id(data->graph, "Node", "not_existing");
    ASSERT_EQUAL(-1, not_existing);
}

CTEST2(node_tests, shouldGetAllNodes) {
    int count = 0;
    roaring_uint32_iterator_t * all = ragedb_all_get_node_ids(data->graph);
    while(all->has_value) {
        count++;
        roaring_advance_uint32_iterator(all);
    }
    ASSERT_EQUAL(2, count);
}

CTEST2(node_tests, shouldGetAllNodesExceptDeleted) {
    int count = 0;
    ragedb_node_remove_by_id(data->graph, 0);
    roaring_uint32_iterator_t * all = ragedb_all_get_node_ids(data->graph);
    while(all->has_value) {
        count++;
        roaring_advance_uint32_iterator(all);
    }
    ASSERT_EQUAL(1, count);
}

CTEST2(node_tests, shouldGetAllNodesWithLabel) {
    int count = 0;
    roaring_uint32_iterator_t * all = ragedb_all_get_node_ids_with_label(data->graph, "Node");
    while(all->has_value) {
        count++;
        roaring_advance_uint32_iterator(all);
    }
    ASSERT_EQUAL(2, count);
}
