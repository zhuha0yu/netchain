
#include"chain.h"





chain_node::chain_node()
{   
    
   
    timeout.tv_sec=0;
    timeout.tv_usec=100000;//set timeout
    
    lenh=num_f+1;
    lenn=htons(num_f+1);
    
   
}

int chain_node::add_node(std::string name)
{
    node node_new;
    uint32_t ip;
    std::string strip="20.0."+name.substr(name.find('s')+1)+".1";
    ip=inet_addr(strip.c_str());
    node_new.node_ip=ip;
    node_new.node_socket=socket(AF_INET,SOCK_DGRAM,0);
    if(node_new.node_socket<0) return -1;
    setsockopt(node_new.node_socket,SOL_SOCKET,SO_RCVTIMEO,(const char*) &timeout,sizeof(timeval));
    memset(&node_new.node_addr,0,queue_addr_len);
    node_new.node_addr.sin_family=AF_INET;
    node_new.node_addr.sin_port=htons(NETCHAIN_PORT);
    node_new.node_addr.sin_addr.s_addr=ip;
    queue_addr_len=sizeof(node_new.node_addr);
    nodes.insert(std::pair<std::string,node>(name,node_new));
    return 0;

}

int chain_node::delete_node(std::string name)
{
    nodes.erase(name);
}

void chain_node::load_node_assignment(const char* file_name)
{
    std::ifstream chain;
    chain.open(file_name,std::ios::in);
    std::string line;
    int i=0;
    while(chain>>line)
    {
        node_sw_assign[i++]=line;
        if(nodes.count(line)==0)
        
            add_node(line);
        
    }

    chain.close();   
}
void chain_node::save_key(const char* file_name)
{
    std::ofstream keys;
    keys.open(file_name,std::ios::out);
    for(std::map<uint16_t,chain>::iterator it=key_chain.begin(); it!=key_chain.end();it++)
    {
        keys<<it->first;
        for(int i=0;i<3;i++)
        {
            keys<<" "<<it->second.nodes[i];
        }
        keys<<std::endl;

    }

    keys.close();   
}
void chain_node::load_key(const char* file_name)
{
    std::ifstream keys;
    keys.open(file_name,std::ios::in);
    chain chain1;
    uint16_t key;
    while(keys>>key)
    {
        keys>>chain1.nodes[0]>>chain1.nodes[1]>>chain1.nodes[2];
        key_chain.insert(std::pair<uint16_t,chain>(key,chain1));
    }

    keys.close(); 
}

std::string get_str_ip(uint32_t ip)
{
    struct in_addr address_struct;
    address_struct.s_addr = ip;

    
    return inet_ntoa(address_struct);
    
}

void chain_node::get_nodes(uint16_t key,int index)
{
    chain new_key_chain;
    new_key_chain.nodes[0]=node_sw_assign[index];
    int i=1;
    while(i<3)
    {
        index++;
        index%=num_virtual_node;
        if(i==1&&node_sw_assign[index]!=new_key_chain.nodes[0])
        {
            new_key_chain.nodes[1]=node_sw_assign[index];
            i++;
        }
        else if(i==2&&node_sw_assign[index]!=new_key_chain.nodes[0]&&node_sw_assign[index]!=new_key_chain.nodes[1])
        {
            new_key_chain.nodes[2]=node_sw_assign[index];
            i++;
        }
        
    }
    key_chain.insert(std::pair<uint16_t,struct chain>(key,new_key_chain));

}