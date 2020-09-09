/*
 * Copyright 2019 International Business Machines
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "client.h"
#include "proto.h"
#include <assert.h>
#include <signal.h>
#include "agent.h"

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

    char *sock_path = "/tmp/thymesisflow-test.sock";

    pid_t agent_pid;
    switch (agent_pid = fork()){
      case -1:
        log_error("Error starting agent...\n");
      case 0:
        //child
        log_info("Starting agent...\n");
        run_agent(sock_path);
        exit(0);
    }

    // wait for the agent to start
    sleep(1);

    log_info("Sending mock requests...\n");

    iport_list *plm = NULL;
    plm = add_port(plm, 1);
    // plm = add_port(plm, 3);
    cycle_ports(plm);

    pmessage mresp = send_attach_memory_msg(id, afu, plm, size, sock_path);
    VERIFY(mresp.status == ATTACH_OK);

    pmessage mdresp = send_detach_memory_msg(id, sock_path);
    VERIFY(mdresp.status == DETACH_OK);

    pmessage cresp =
        send_attach_compute_msg(id, afu, plm, size, mresp.ea, sock_path);
    VERIFY(cresp.status == ATTACH_OK);

    pmessage cdresp = send_detach_compute_msg(id, sock_path);
    VERIFY(cdresp.status == DETACH_OK);

    //terminate the agent
    kill(agent_pid,SIGTERM);
#else
    log_info("this is an API test, build with MOCK=1\n");
#endif
}

int main(int argc, char const *argv[]) {
#ifdef MOCK
    test_hex_string_to_address();
    test_iport_marshalling();
    test_connection();
    test_integration();
#else
    log_warn("Unit test are available only with MOCK=1\n");
#endif
}
