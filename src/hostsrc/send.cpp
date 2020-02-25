#include<iostream>
#include<string>
#include<stdlib.h>
#include<unistd.h>
#include <algorithm>
#include"queue.h"
#include"chain.h"
#include"evaluate.h"
#include"hashing.h"
#include <sys/time.h> 



int main(int argc,char *args[])
{


    chain_node chain_nodes;
    
    std::cout<<"Welcome to NetChain!"<<std::endl; 
    chain_nodes.load_node_assignment("nodes.csv");
    //load existed keys.

    while (true)
    {
        uint16_t key;
        uint32_t value=0;
        std::string operation;
        queue_return result;
        
        std::cout<<"Please enter command:<operation> <key>( <value>). Enter help for help:"<<std::endl;
        std::cin>>operation;
        transform(operation.begin(),operation.end(),operation.begin(),::tolower);
        //to lower case
        if(operation=="update")
        {
            //load and update
            chain_nodes.load_node_assignment("nodes.csv");
            chain_nodes.load_key("keys.csv");
            std::cout<<"Update successfully!"<<std::endl;
            continue;
        }
        else if(operation=="nodes"||operation=="node")
        {
            //show keys
            for(std::map<std::string,node>::const_iterator it = chain_nodes.nodes.begin(); it != chain_nodes.nodes.end(); it++)
            std::cout<<"Node : "<<it->first<<", IP: "<<get_str_ip(it->second.node_ip)<<std::endl;
            continue;
        }
        else if(operation=="evaluate")
        {
            start_evaluate(&chain_nodes);
            continue;
        }
        else if(operation=="key"||operation=="keys")
        {
            for(std::map<uint16_t,chain>::iterator it=chain_nodes.key_chain.begin(); it!=chain_nodes.key_chain.end();it++)
            {
                std::cout<<"key:\t\t"<<it->first<<" , chain:\t"<<it->second.nodes[0]<<"\t"<<it->second.nodes[1]<<"\t"<<it->second.nodes[2]<<std::endl;

            }
            continue;
        }
        else if(operation=="help"||operation=="?")
        {
            std::cout<<"Availble operations:"<<std::endl;
            std::cout<<"insert <key> <value> : Insert one key to NetChain Key-Value storage"<<std::endl;
            std::cout<<"delete <key> : Delete one key from NetChain Key-Value storage"<<std::endl;
            std::cout<<"write <key> <value> : Write value of the key in NetChain Key-Value storage"<<std::endl;
            std::cout<<"read <key> : Read value of the key from NetChain Key-Value storage"<<std::endl;
            std::cout<<"update : update Virtual nodes from files"<<std::endl;
            std::cout<<"key : show available keys and chain of them"<<std::endl;
            std::cout<<"node : print chain ips"<<std::endl;
            std::cout<<"evaluate : start evaluation"<<std::endl;
            continue;
        }
        else if(operation!="read"&&operation!="delete"&&operation!="write"&&operation!="insert")
        {
            std::cout<<"Operation illegal!"<<std::endl;
            std::cin.clear();
            std::cin.ignore(1000,'\n');
            continue;
        }
        std::cin>>key;
        

        if(operation=="write"||operation=="insert") std::cin>>value;
        
        

        timeval tv1,tv2;
        gettimeofday(&tv1,NULL);
        if(operation=="insert")
        {
            
            if(chain_nodes.key_chain.count(key)>0)
            {
                std::cout<<"Key "<<key<<" already exist!"<<std::endl;
                continue;
            }
            
            chain_nodes.get_nodes(key,do_hashing(key));
            result=send_queue(&chain_nodes,getopfromstr(operation),key,value);
        }
        else
        {
            if(chain_nodes.key_chain.count(key)==0)
            {
                std::cout<<"Key "<<key<<" doesn't exist!"<<std::endl;
                continue;
            }
            
            //chain_nodes.get_nodes(key,do_hashing(key));
            result=send_queue(&chain_nodes,getopfromstr(operation),key,value);
        }
        
        
        if(result.is_success)
        {
            gettimeofday(&tv2,NULL);
            std::cout<<"Queue successful, ";
            switch (getopfromstr(operation))
            {
            case 1:
                std::cout<<operation<<" "<<result.value<<" to "<<key<<"."<<std::endl;
                break;
            case 2:
                std::cout<<operation<<" "<<result.value<<" from "<<key<<"."<<std::endl;
                break;
            case 3:
                std::cout<<operation<<" "<<key<<" to NetChain with value "<<result.value<<" successfully."<<std::endl;
                chain_nodes.save_key("keys.csv");
                break;
            case 4:
                std::cout<<operation<<" "<<key<<" from NetChain."<<std::endl;
                chain_nodes.key_chain.erase(key);
                break;
            default:
                break;
            }
            int sec=tv2.tv_sec-tv1.tv_sec;
            int usec=tv2.tv_usec-tv1.tv_usec;
            
            
            double time_use=sec+usec/1000000.0;
            std::cout<<"Time usage: "<<time_use<<"sec."<<std::endl;
        }
        else
        {
            if(operation=="insert")
                chain_nodes.key_chain.erase(key);
            std::cout<<"Queue timeout"<<std::endl;
        }


    }



}



