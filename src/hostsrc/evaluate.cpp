#include"evaluate.h"

void start_evaluate(chain_node chain_nodes)
{
    while(true)
    {
        std::cout<<"Evaluation Start! Please enter command to test. Available commands:"<<std::endl;

        std::cout<<"insert : insert a number of keys to Netchain"<<std::endl;

        std::cout<<"quit/exit : exit evaluation"<<std::endl;
        std::string command;
        std::cin>>command;
        if(command=="insert")
        {
            std::cout<<"Please enter the count you want to insert"<<std::endl;
            int count;
            std::cin>>count;
            if(count>MAX_KEY_SIZE)
            {
                std::cout<<"Counts too large, must be smaller than 8192"<<std::endl;
                continue;
            }
            srand((unsigned)time(NULL));
            uint16_t key=1024;
            queue_return result;
            int count_success=0;
            int count_fail=0;
            timeval tv_start,tv_recv;
            gettimeofday(&tv_start,NULL);
            
            time_t now = time(0);
            char* now_str = ctime(&now);
            std::cout<<"Inserting starts at"<<now_str<<std::endl;
            for(int i=0;i<count;i++)
            {
                int step=rand()%5+1;
                key+=step;
                uint64_t value=rand();
                while(chain_nodes.key_chain.count(key)>0)
                {
                    key+=step;
                    
                }
                timeval tv_sent;
                gettimeofday(&tv_sent,NULL);
                double time_diff=(tv_sent.tv_sec-tv_start.tv_sec)+(tv_sent.tv_usec-tv_start.tv_usec)/1000000.0;
                chain_nodes.get_nodes(key,do_hashing(key));
                std::cout<<time_diff<<"s, Inserting No."<<i<<" key: "<<key<<" to NetChain.... ";
                result=send_queue(&chain_nodes,3,key,value);
                if(result.is_success) 
                {
                    count_success++;
                    std::cout<<"success."<<std::endl;
                }
                else
                {
                    count_fail++;
                    std::cout<<"fail."<<std::endl;
                }
                
            }
            
            gettimeofday(&tv_recv,NULL);
            double time_diff=(tv_recv.tv_sec-tv_start.tv_sec)+(tv_recv.tv_usec-tv_start.tv_usec)/1000000.0;
            std::cout<<"Inserting "<<count<<" keys to Netchain finished. Statistics:"<<std::endl;

            std::cout<<"Succeed insertings : \t\t\t\t\t\t"<<count_success<<std::endl;
            std::cout<<"Failed insertings  : \t\t\t\t\t\t"<<count_fail<<std::endl;
            std::cout<<"Success rate       : \t\t\t\t\t\t"<<count_success/(double)count<<std::endl;
            std::cout<<"Time used          : \t\t\t\t\t\t"<<time_diff<<std::endl;
            std::cout<<"Time used per key  : \t\t\t\t\t\t"<<time_diff/(double)count<<std::endl;
            std::cout<<"-----------------------------------------"<<std::endl<<std::endl<<std::endl;
            std::cout<<"Saving keys to file......";
            chain_nodes.save_key("keys.csv");
            std::cout<<" success"<<std::endl;
            std::cout<<"-----------------------------------------"<<std::endl<<std::endl<<std::endl;

            
        }
        else if(command=="write")
        {
            std::cout<<"Please enter the seconds you want to write"<<std::endl;
            int second;
            std::cin>>second;
            srand((unsigned)time(NULL));
            timeval tv_start,tv_end;
            queue_return result;
            int count_success=0;
            int count_fail=0;
            int count=0;
            gettimeofday(&tv_start,NULL);
            
            uint16_t keys[chain_nodes.key_chain.size()];
            
            int i=0;
            for(std::map<uint16_t,chain>::iterator it=chain_nodes.key_chain.begin(); it!=chain_nodes.key_chain.end();it++)
            {
                keys[i++]=it->first;
            }

            while(1)
            {
                gettimeofday(&tv_end,NULL);
                if (tv_end.tv_sec-tv_start.tv_sec>second) break;
                int index=rand()%chain_nodes.key_chain.size();
                double time_diff=(tv_end.tv_sec-tv_start.tv_sec)+(tv_end.tv_usec-tv_start.tv_usec)/1000000.0;
                uint16_t key=keys[index];
                std::cout<<time_diff<<"s, Writing No."<<count<<" key: "<<key<<" to NetChain.... ";
                uint64_t value=rand();
                result=send_queue(&chain_nodes,1,key,value);
                count++;
                if(result.is_success) 
                {
                    count_success++;
                    std::cout<<"success."<<std::endl;
                }
                else
                {
                    count_fail++;
                    std::cout<<"fail."<<std::endl;
                }

            }
            double time_diff=(tv_end.tv_sec-tv_start.tv_sec)+(tv_end.tv_usec-tv_start.tv_usec)/1000000.0;
            std::cout<<"Writing "<<count<<" keys to Netchain finished. Statistics:"<<std::endl;

            std::cout<<"Succeed writings : \t\t\t\t\t\t"<<count_success<<std::endl;
            std::cout<<"Failed writings  : \t\t\t\t\t\t"<<count_fail<<std::endl;
            std::cout<<"Success rate       : \t\t\t\t\t\t"<<count_success/(double)count<<std::endl;
            std::cout<<"Time used          : \t\t\t\t\t\t"<<time_diff<<std::endl;
            std::cout<<"Time used per key  : \t\t\t\t\t\t"<<time_diff/(double)count<<std::endl;
            std::cout<<"-----------------------------------------"<<std::endl<<std::endl<<std::endl;
        }
        else if(command=="quit"||command=="exit")
        {
            return;
        }
        
    }
}