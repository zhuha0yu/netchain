#include<sys/socket.h>
#include<iostream>
#include<fstream>
#include<string>
#include<arpa/inet.h>
#include<cstring>
#include<map>
#include<signal.h>
#include<regex>
#define NETCHAIN_PORT 35678
#define num_virtual_node 1024
#define num_f 2
#define num_switch 4
#ifndef _HASH_H
#define _HASH_H

struct node
{
    uint32_t node_ip;
    sockaddr_in node_addr;
    int node_socket;
};
struct chain
{
    std::string nodes[num_f+1];
};
class chain_node
{
public:
    int lenh;
    int lenn;
    timeval timeout;
    std::map<std::string,node> nodes;
    socklen_t queue_addr_len;
    std::map<uint16_t,chain> key_chain;
    std::string node_sw_assign[num_virtual_node];
    chain_node();
    int add_node(std::string name);
    int delete_node(std::string name);
    void load_node_assignment(const char* file_name);
    void get_nodes(uint16_t key,int index);
    void save_key(const char* file_name);
    void load_key(const char* file_name);

};



#endif


std::string get_str_ip(uint32_t ip);





