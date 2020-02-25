#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<cstring>
#include<string>
#include<iostream>
#include<errno.h>

#include<unistd.h>
#include"chain.h"


#define MAX_RETRY 5
#ifndef _QUEUE_H
#define _QUEUE_H
struct nc_payload_recv
{
    uint16_t count __attribute__((packed));
    uint16_t operation __attribute__((packed));
    uint32_t sequence __attribute__((packed));
    uint16_t key __attribute__((packed));
    uint64_t value __attribute__((packed));
};
struct _nc_send
{
    int socket;
    const char* nc_send;
    int totallen;
    int flag;
    const sockaddr* dst_addr;
    socklen_t addr_len;
};

struct queue_return
{
    bool is_success;
    uint64_t value;
};
queue_return send_queue(chain_node* chain_nodes,uint16_t operation,uint16_t key,uint64_t value=0);
#endif

uint64_t ntohl64(uint64_t);
uint64_t htonl64(uint64_t);
void ntoh(nc_payload_recv*);
uint16_t getopfromstr(std::string str);