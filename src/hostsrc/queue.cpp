#include"queue.h"



queue_return send_queue(chain_node* chain_nodes,uint16_t operationh,uint16_t keyh,uint64_t valueh=0)
{
    uint16_t count_n=htons(num_f+1);
    sockaddr_in src_addr;
    socklen_t addr_len=sizeof(src_addr);
    static uint32_t sequence=0;
    uint16_t key=htons(keyh);
    
    nc_payload_recv nc_recv;
    memset(&nc_recv,0,18);
    char nc_send[150];
    memset(nc_send,0,150);
    int totallen=18+4*(num_f+1);
    
    
    memcpy(nc_send,&count_n,2);
    //not read, correct order
    if(operationh!=2)
        for(int i=0;i<=num_f;i++)
            memcpy(nc_send+2+4*i,&(chain_nodes->nodes.at(chain_nodes->key_chain.at(keyh).nodes[i]).node_ip),4);
    else
        for(int i=num_f;i>=0;i--)
            memcpy(nc_send+2+4*i,&(chain_nodes->nodes.at(chain_nodes->key_chain.at(keyh).nodes[i]).node_ip),4);
    uint16_t operation=htons(operationh);
    memcpy(nc_send+2+4*(num_f+1),&operation,2);
    
    memcpy(nc_send+8+4*(num_f+1),&key,2);
    uint64_t value=htonl64(valueh);
    memcpy(nc_send+10+4*(num_f+1),&value,8);
    queue_return result;
    for(int i=0;i<MAX_RETRY;i++)
    {   
        
    
        
        uint32_t seqn=htonl(0);
        memcpy(nc_send+4+4*(num_f+1),&seqn,4);

        _nc_send args;
        if(operationh!=2)
            args.socket=chain_nodes->nodes.at(chain_nodes->key_chain.at(keyh).nodes[0]).node_socket;
        else
            args.socket=chain_nodes->nodes.at(chain_nodes->key_chain.at(keyh).nodes[num_f]).node_socket;
        args.nc_send=nc_send;
        args.totallen=totallen;
        args.flag=0;
        if(operationh!=2)
            args.dst_addr=(sockaddr*)&(chain_nodes->nodes.at(chain_nodes->key_chain.at(keyh).nodes[0]).node_addr);
        else
            args.dst_addr=(sockaddr*)&(chain_nodes->nodes.at(chain_nodes->key_chain.at(keyh).nodes[num_f]).node_addr);
        args.addr_len=addr_len;
        
        sendto(args.socket,&nc_send,totallen,0,args.dst_addr,addr_len);
        
        int ret=recvfrom(args.socket,&nc_recv,18,0,(sockaddr *)&src_addr,&addr_len);
        
        if(ret<0) continue;
        ntoh(&nc_recv);
        
        key=ntohs(key);
        if(nc_recv.key==key&&nc_recv.operation==5)
        {
            result.is_success=true;
            result.value=nc_recv.value;
            return result;
        }
    }
    result.is_success=false;
    result.value=0;
    return result;
}


uint64_t ntohl64(uint64_t netvalue)
{
    uint64_t res = 0; 
    uint32_t high,low;
    low=netvalue & 0xFFFFFFFF;
    high=(netvalue>>32)&0xFFFFFFFF;
    low = ntohl(low);
    high = ntohl(high);
    res=low;
    res<<=32;
    res|=high;
    return res;
}

uint64_t htonl64(uint64_t hostvalue)
{
    uint64_t res = 0; 
    uint32_t high,low;
    low=hostvalue & 0xFFFFFFFF;
    high=(hostvalue>>32)&0xFFFFFFFF;
    low = htonl(low);
    high = htonl(high);
    res=low;
    res<<=32;
    res|=high;
    return res;
}
void ntoh(nc_payload_recv* recv)
{
    recv->count=ntohs(recv->count);
    recv->operation=ntohs(recv->operation);
    recv->sequence=ntohl(recv->sequence);
    recv->key=ntohs(recv->key);
    recv->value=ntohl64(recv->value);
}
uint16_t getopfromstr(std::string str)
{
    if(str=="write") return 1;
    else if(str=="read") return 2;
    else if(str=="insert") return 3;
    else if(str=="delete") return 4;
    return 0;
}