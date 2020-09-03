#include "client.h"
#include "proto.h"
#include <assert.h>

#define VERIFY(cond)                                                           \
    if (!(cond)) {                                                             \
        printf("\e[31mUnit test failed [ %s ] at %s - line: %d\e[0m\n\n",      \
               __func__, __FILE__, __LINE__);                                  \
        return;                                                                \
    } else {                                                                   \
        printf("\e[32mUnit test passed [ %s ] at %s - line: %d\e[0m\n\n",      \
               __func__, __FILE__, __LINE__);                                  \
    }

#define PASSED                                                                 \
    { printf("\e[32mUnit test [ %s ] passed..\n", __func__); }

void test_hex_string_to_address() {
    const char *hexstring = "0xabcdef0";
    long int number = (int)strtol(hexstring, NULL, 16);
    const char *hexstring2 = "abcdef0";
    log_info("hex representation: hex(%ld):%lx\n", number, number);
    long int number2 = (int)strtol(hexstring2, NULL, 16);
    log_info("hex representation: hex(%ld):%lx\n", number2, number2);
}

void test_connection() {
    connection *conn1 = new_conn("a", "afu1", 10);
    add_conn(conn1);
    VERIFY(conn1->size == 10 && strcmp(conn1->circuit_id, "a") == 0 &&
           strcmp(conn1->afu_name, "afu1") == 0 && conn1->next == NULL);

    connection *conn2 = new_conn("b", "afu2", 10);
    add_conn(conn2);
    VERIFY(clist == conn2 && clist->next == conn1);

    connection *conn3 = new_conn("c", "afu3", 10);
    add_conn(conn3);
    VERIFY(clist == conn3 && clist->next == conn2);

    list_connections();

    connection *conn_ret = get_conn("b");
    log_info("returned conn: %s\n", conn_ret->circuit_id);
    VERIFY(strcmp(conn_ret->circuit_id, "b") == 0);

    del_conn("b");
    VERIFY(clist == conn3 && clist->next == conn1);

    del_conn("a");
    VERIFY(clist == conn3 && clist->next == NULL);

    del_conn("c");
    VERIFY(clist == NULL);
}
void test_iport_marshalling() {

    iport_list *pl = NULL;
    pl = add_port(pl, 1);
    u_int8_t rep = marshal_iports(pl);
    VERIFY(rep == 1);

    pl = add_port(pl, 5);
    rep = marshal_iports(pl);
    VERIFY(rep == 17); // hex(11) == 17 == 2^4 + 2^0
}

void test_iport() {
    u_int8_t port = 1; // expect port 1
    u_int8_t ptest = 0;
    log_info("test - %d ...\n", port);
    iport_list *pl = unmarshal_iports(port);
    cycle_ports(pl);
    free_iport_list(pl);

    port = 3; // expect port 1 and 2
    log_info("test - %d ...\n", port);
    pl = unmarshal_iports(port);
    cycle_ports(pl);
    ptest = marshal_iports(pl);
    log_info("test - serialization %d == %d ...\n", ptest, port);
    VERIFY(ptest == port);
    free_iport_list(pl);

    port = 10; // expect port 2 and 4
    log_info("test - %d ...\n", port);
    pl = unmarshal_iports(port);
    cycle_ports(pl);

    ptest = marshal_iports(pl);
    log_info("test - serialization %d == %d ...\n", ptest, port);
    VERIFY(ptest == port);
    free_iport_list(pl);

    port = 0; // expect port 1
    log_info("test - %d ...\n", port);
    pl = unmarshal_iports(port);
    cycle_ports(pl);

    port = 16; // expect port 5
    log_info("test - %d ...\n", port);
    pl = unmarshal_iports(port);
    cycle_ports(pl);
}

void test_integration() {

#ifdef MOCK
    char *id = "cc592edc-eca6-4e0f-9541-5a3684c37757";
    char *afu = "/dev/ibm-n0";
    uint64_t size = 68719476736;

    iport_list *plm = NULL;
    plm = add_port(plm, 1);
    // plm = add_port(plm, 3);
    cycle_ports(plm);

    pmessage mresp = send_attach_memory_msg(id, afu, plm, size, SOCK_PATH);
    VERIFY(mresp.status == ATTACH_OK);

    pmessage mdresp = send_detach_memory_msg(id, SOCK_PATH);
    VERIFY(mdresp.status == DETACH_OK);

    pmessage cresp =
        send_attach_compute_msg(id, afu, plm, size, mresp.ea, SOCK_PATH);
    VERIFY(cresp.status == ATTACH_OK);

    pmessage cdresp = send_detach_compute_msg(id, SOCK_PATH);
    VERIFY(cdresp.status == DETACH_OK);
#else
    log_info("this is an API test, build with MOCK=1\n");
#endif
}

int main(int argc, char const *argv[]) {
    test_hex_string_to_address();
    test_iport_marshalling();
    test_connection();
    test_integration();
}
