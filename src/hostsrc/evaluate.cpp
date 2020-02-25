#include"evaluate.h"

void start_evaluate(chain_node* chain_nodes)
{
    while(true)
    {   
        std::ofstream log;
        std::cout<<"Evaluation Start! Please enter command to test. Available commands:"<<std::endl;

        std::cout<<"insert        : insert a number of keys to Netchain"<<std::endl;
        std::cout<<"write         : write for a number of seconds to Netchain"<<std::endl;
        std::cout<<"read          : read for a number of seconds from Netchain"<<std::endl;
        std::cout<<"thrp          : test the read and write throughput of Netchain"<<std::endl;
        std::cout<<"quit/exit     : exit evaluation"<<std::endl;
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
            timeval tv_start,tv_recv,tv_sent;
            gettimeofday(&tv_start,NULL);
            
            time_t now = time(0);
            char* now_str = ctime(&now);
            log.open("../evaluation/Insert_"+std::to_string(count)+" keys_"+now_str+".log",std::ios::out);
            std::cout<<"Inserting starts at "<<now_str<<std::endl;
            log<<"Inserting starts at "<<now_str<<std::endl;
            for(int i=0;i<count;i++)
            {
                int step=rand()%5+1;
                key+=step;
                uint64_t value=rand();
                while(chain_nodes->key_chain.count(key)>0)
                {
                    key+=step;
                    
                }
                
                gettimeofday(&tv_sent,NULL);
                double time_diff=(tv_sent.tv_sec-tv_start.tv_sec)+(tv_sent.tv_usec-tv_start.tv_usec)/1000000.0;
                chain_nodes->get_nodes(key,do_hashing(key));
                std::cout<<std::left <<std::setw(8)<<time_diff<<" s, Inserting No."<<std::left <<std::setw(5)<<i<<"key: "<<std::left <<std::setw(5)<<key<<"value: "<<std::left <<std::setw(10)<<value<<"to NetChain.... ";
                log<<std::left <<std::setw(8)<<time_diff<<" s, Inserting No."<<std::left <<std::setw(5)<<i<<"key: "<<std::left <<std::setw(5)<<key<<"value: "<<std::left <<std::setw(10)<<value<<"to NetChain.... ";
                result=send_queue(chain_nodes,3,key,value);
                gettimeofday(&tv_recv,NULL);
                time_diff=(tv_recv.tv_sec-tv_sent.tv_sec)+(tv_recv.tv_usec-tv_sent.tv_usec)/1000000.0;
                if(result.is_success) 
                {
                    count_success++;
                    std::cout<<"success. At "<<time_diff<<" s."<<std::endl;
                    log<<"success. At "<<time_diff<<" s."<<std::endl;
                }
                else
                {
                    chain_nodes->key_chain.erase(key);
                    count_fail++;
                    std::cout<<"fail. At "<<time_diff<<" s."<<std::endl;
                    log<<"fail. At "<<time_diff<<" s."<<std::endl;
                }
                
            }
            
            gettimeofday(&tv_recv,NULL);
            double time_diff=(tv_recv.tv_sec-tv_start.tv_sec)+(tv_recv.tv_usec-tv_start.tv_usec)/1000000.0;
            std::cout<<"Inserting "<<count<<" keys to Netchain finished. Statistics:"<<std::endl;
            std::cout<<"Succeed insertings : "<<count_success<<std::endl;
            std::cout<<"Failed insertings  : "<<count_fail<<std::endl;
            std::cout<<"Success rate       : "<<count_success/(double)count<<std::endl;
            std::cout<<"Time used          : "<<time_diff<<"\tsec"<<std::endl;
            std::cout<<"Time used per key  : "<<time_diff/(double)count<<"\tsec"<<std::endl;
            std::cout<<"-----------------------------------------"<<std::endl<<std::endl<<std::endl;
            std::cout<<"Saving keys to file......";
            chain_nodes->save_key("keys.csv");
            std::cout<<" success"<<std::endl;
            std::cout<<"-----------------------------------------"<<std::endl<<std::endl<<std::endl;
            
            log<<"Inserting "<<count<<" keys to Netchain finished. Statistics:"<<std::endl;
            log<<"Succeed insertings : "<<count_success<<std::endl;
            log<<"Failed insertings  : "<<count_fail<<std::endl;
            log<<"Success rate       : "<<count_success/(double)count<<std::endl;
            log<<"Time used          : "<<time_diff<<"\tsec"<<std::endl;
            log<<"Time used per key  : "<<time_diff/(double)count<<"\tsec"<<std::endl;
            log<<"-----------------------------------------"<<std::endl;
            log.close();
            
        }
        else if(command=="write")
        {
            std::cout<<"Please enter the seconds you want to write"<<std::endl;
            int second;
            std::cin>>second;
            srand((unsigned)time(NULL));
            timeval tv_start,tv_sent,tv_recv;
            queue_return result;
            int count_success=0;
            int count_fail=0;
            int count=0;
            gettimeofday(&tv_start,NULL);
            time_t now = time(0);
            char* now_str = ctime(&now);
            log.open("../evaluation/Write_"+std::to_string(second)+" sec_"+now_str+".log",std::ios::out);
            std::cout<<"Writing starts at "<<now_str<<std::endl;
            log<<"Writing starts at "<<now_str<<std::endl;

            uint16_t keys[chain_nodes->key_chain.size()];
            
            int i=0;
            for(std::map<uint16_t,chain>::iterator it=chain_nodes->key_chain.begin(); it!=chain_nodes->key_chain.end();it++)
            {
                keys[i++]=it->first;
            }

            while(1)
            {
                gettimeofday(&tv_sent,NULL);
                
                int index=rand()%chain_nodes->key_chain.size();
                double time_diff=(tv_sent.tv_sec-tv_start.tv_sec)+(tv_sent.tv_usec-tv_start.tv_usec)/1000000.0;
                if (time_diff>second) break;
                uint16_t key=keys[index];
                if(write_test(chain_nodes,key,log,&tv_start,count))
                count_success++;
                else count_fail++;
                count++;

            }
            double time_diff=(tv_sent.tv_sec-tv_start.tv_sec)+(tv_sent.tv_usec-tv_start.tv_usec)/1000000.0;
            std::cout<<"Writing "<<count<<" keys to Netchain finished. Statistics:"<<std::endl;

            std::cout<<"Succeed writings   : "<<count_success<<std::endl;
            std::cout<<"Failed writings    : "<<count_fail<<std::endl;
            std::cout<<"Success rate       : "<<count_success/(double)count<<std::endl;
            std::cout<<"Time used          : "<<time_diff<<" sec"<<std::endl;
            std::cout<<"Time used per key  : "<<time_diff/(double)count<<" sec"<<std::endl;
            std::cout<<"-----------------------------------------"<<std::endl<<std::endl<<std::endl;

            log<<"writing "<<count<<" keys to Netchain finished. Statistics:"<<std::endl;
            log<<"Succeed writings   : "<<count_success<<std::endl;
            log<<"Failed writings    : "<<count_fail<<std::endl;
            log<<"Success rate       : "<<count_success/(double)count<<std::endl;
            log<<"Time used          : "<<time_diff<<"sec"<<std::endl;
            log<<"Time used per key  : "<<time_diff/(double)count<<"sec"<<std::endl;
            log<<"-----------------------------------------"<<std::endl;
            log.close();

        }
        else if(command=="read")
        {
            std::cout<<"Please enter the seconds you want to read"<<std::endl;
            int second;
            std::cin>>second;
            srand((unsigned)time(NULL));
            timeval tv_start,tv_sent;
            
            int count_success=0;
            int count_fail=0;
            int count=0;
            gettimeofday(&tv_start,NULL);
            time_t now = time(0);
            char* now_str = ctime(&now);
            log.open("../evaluation/Read_"+std::to_string(second)+" sec_"+now_str+".log",std::ios::out);
            std::cout<<"Reading starts at "<<now_str<<std::endl;
            log<<"Reading starts at "<<now_str<<std::endl;

            uint16_t keys[chain_nodes->key_chain.size()];
            
            int i=0;
            for(std::map<uint16_t,chain>::iterator it=chain_nodes->key_chain.begin(); it!=chain_nodes->key_chain.end();it++)
            {
                keys[i++]=it->first;
            }

            while(1)
            {
                gettimeofday(&tv_sent,NULL);
                
                int index=rand()%chain_nodes->key_chain.size();
                double time_diff=(tv_sent.tv_sec-tv_start.tv_sec)+(tv_sent.tv_usec-tv_start.tv_usec)/1000000.0;
                if (time_diff>second) break;
                uint16_t key=keys[index];
                if(read_test(chain_nodes,key,log,&tv_start,count))
                count_success++;
                else count_fail++;
                count++;

            }
            double time_diff=(tv_sent.tv_sec-tv_start.tv_sec)+(tv_sent.tv_usec-tv_start.tv_usec)/1000000.0;
            std::cout<<"Reading "<<count<<" keys to Netchain finished. Statistics:"<<std::endl;

            std::cout<<"Succeed readings   : "<<count_success<<std::endl;
            std::cout<<"Failed readings    : "<<count_fail<<std::endl;
            std::cout<<"Success rate       : "<<count_success/(double)count<<std::endl;
            std::cout<<"Time used          : "<<time_diff<<" sec"<<std::endl;
            std::cout<<"Time used per key  : "<<time_diff/(double)count<<" sec"<<std::endl;
            std::cout<<"-----------------------------------------"<<std::endl<<std::endl<<std::endl;

            log<<"Reading "<<count<<" keys to Netchain finished. Statistics:"<<std::endl;
            log<<"Succeed readings   : "<<count_success<<std::endl;
            log<<"Failed readings    : "<<count_fail<<std::endl;
            log<<"Success rate       : "<<count_success/(double)count<<std::endl;
            log<<"Time used          : "<<time_diff<<"sec"<<std::endl;
            log<<"Time used per key  : "<<time_diff/(double)count<<"sec"<<std::endl;
            log<<"-----------------------------------------"<<std::endl;
            log.close();

        }

        else if(command=="thrp"||command=="throughput")
        {
            std::cout<<"Please enter the threads you want to test"<<std::endl;
            int threads;
            std::cin>>threads;
            std::cout<<"Please enter the seconds you want to test"<<std::endl;
            int seconds;
            std::cin>>seconds;
            srand((unsigned)time(NULL));

            timeval tv_start,tv_end;
            
            int count_success=0;
            int count_fail=0;
            int count=0;
            gettimeofday(&tv_start,NULL);
            time_t now = time(0);
            char* now_str = ctime(&now);
            std::string filename="../evaluation/Throughput_"+std::to_string(threads)+" threads_"+std::to_string(seconds)+" secs_"+now_str+".log";
            log.open(filename,std::ios::app);
            std::cout<<"Testing starts at "<<now_str<<std::endl;
            log<<"Testing starts at "<<now_str<<std::endl;
            std::thread* th[threads];
            evaluate_result res;
            for(int i=0;i<threads;i++)
            {
                th[i]=new std::thread(thrp_test,*chain_nodes,i,seconds,std::ref(res),filename);

            }
            for(int i=0;i<threads;i++)
            {
                th[i]->join();
            }
            




            
                count=res.count;
                count_success=res.count_success;
                count_fail=res.count_fail;
            
            gettimeofday(&tv_end,NULL);
            double time_diff=(tv_end.tv_sec-tv_start.tv_sec)+(tv_end.tv_usec-tv_start.tv_usec)/1000000.0;
            std::cout<<"Queueing "<<count<<" keys to Netchain finished. Statistics:"<<std::endl;

            std::cout<<"Succeed Queues     : "<<count_success<<std::endl;
            std::cout<<"Failed Queues      : "<<count_fail<<std::endl;
            std::cout<<"Success rate       : "<<count_success/(double)count<<std::endl;
            std::cout<<"Time used          : "<<time_diff<<" sec"<<std::endl;
            std::cout<<"Time used per key  : "<<time_diff/(double)count<<" sec"<<std::endl;
            std::cout<<"Throughput  : "<<count/time_diff<<"Queue/sec"<<std::endl;
            std::cout<<"-----------------------------------------"<<std::endl<<std::endl<<std::endl;

            log<<"Queueing "<<count<<" keys to Netchain finished. Statistics:"<<std::endl;
            log<<"Succeed Queues     : "<<count_success<<std::endl;
            log<<"Failed Queues      : "<<count_fail<<std::endl;
            log<<"Success rate       : "<<count_success/(double)count<<std::endl;
            log<<"Time used          : "<<time_diff<<" sec"<<std::endl;
            log<<"Time used per key  : "<<time_diff/(double)count<<" sec"<<std::endl;
            log<<"Throughput   : "<<count/time_diff<<"Queue/sec"<<std::endl;
            log<<"-----------------------------------------"<<std::endl;
            log.close();

        }

        else if(command=="quit"||command=="exit")
        {
            return;
        }
        
    }
}
bool read_test(chain_node* chain_nodes,uint16_t key,std::ofstream& log,timeval* starttime,int count)
{
    queue_return result;
    timeval tv_sent,tv_recv;

    gettimeofday(&tv_sent,NULL);
    double time_diff=tv_sent.tv_sec-starttime->tv_sec+(tv_sent.tv_usec-starttime->tv_usec)/1000000.0;
    std::cout<<std::left <<std::setw(8)<<time_diff<<"s, Reading No."<<std::left <<std::setw(5)<<count<<" key: "<<std::left <<std::setw(5)<<key<<"from NetChain.... ";
    log<<std::left <<std::setw(8)<<time_diff<<"s, Reading No."<<std::left <<std::setw(5)<<count<<" key: "<<std::left <<std::setw(5)<<key<<"from NetChain.... ";
    result=send_queue(chain_nodes,2,key);
    count++;
    gettimeofday(&tv_recv,NULL);
    time_diff=(tv_recv.tv_sec-tv_sent.tv_sec)+(tv_recv.tv_usec-tv_sent.tv_usec)/1000000.0;
    if(result.is_success) 
    {
        std::cout<<"success. Value:"<<std::left <<std::setw(5)<<result.value<< ".At "<<time_diff<<" s."<<std::endl;
        log<<"success. Value:"<<std::left <<std::setw(5)<<result.value<<".At "<<time_diff<<" s."<<std::endl;
        return true;
    }
    else
    {
        std::cout<<"fail. At "<<time_diff<<" s."<<std::endl;
        log<<"fail. At "<<time_diff<<" s."<<std::endl;
        return false;
    }
}
bool write_test(chain_node* chain_nodes,uint16_t key,std::ofstream& log,timeval* starttime,int count)
{
    queue_return result;
    timeval tv_send,tv_recv;

    uint64_t value=rand();
    gettimeofday(&tv_send,NULL);
    double time_diff=tv_send.tv_sec-starttime->tv_sec+(tv_send.tv_usec-starttime->tv_usec)/1000000.0;
    std::cout<<std::left <<std::setw(8)<<time_diff<<"s, Writing No."<<std::left <<std::setw(5)<<count<<"key: "<<std::left <<std::setw(5)<<key<<"value: "<<std::left <<std::setw(10)<<value<<"to NetChain.... ";
    log<<std::left <<std::setw(8)<<time_diff<<"s, Writing No."<<std::left <<std::setw(5)<<count<<"key: "<<std::left <<std::setw(5)<<key<<"value: "<<std::left <<std::setw(10)<<value<<"to NetChain.... ";

    result=send_queue(chain_nodes,1,key,value);
    count++;
    gettimeofday(&tv_recv,NULL);
    time_diff=(tv_recv.tv_sec-tv_send.tv_sec)+(tv_recv.tv_usec-tv_send.tv_usec)/1000000.0;
    if(result.is_success) 
    {
        std::cout<<"success. At "<<time_diff<<" s."<<std::endl;
        log<<"success. At "<<time_diff<<" s."<<std::endl;
        return true;
    }
    else
    {
        std::cout<<"fail. At "<<time_diff<<" s."<<std::endl;
        log<<"fail. At "<<time_diff<<" s."<<std::endl;
        return false;
    }
}

void thrp_test(chain_node chain_nodes,int num_thread,int seconds,evaluate_result& ret,std::string filename)
{
    std::ofstream log;
    log.open(filename,std::ios::app);
    uint16_t keys[chain_nodes.key_chain.size()];
    timeval tv_start,tv_end;
    int i=0;
    for(std::map<uint16_t,chain>::iterator it=chain_nodes.key_chain.begin(); it!=chain_nodes.key_chain.end();it++)
    {
        keys[i++]=it->first;
    }
    
    queue_return result;
    gettimeofday(&tv_start,NULL);
    while(1)
    {
        gettimeofday(&tv_end,NULL);
        
        int index=rand()%chain_nodes.key_chain.size();
        double time_diff=(tv_end.tv_sec-tv_start.tv_sec)+(tv_end.tv_usec-tv_start.tv_usec)/1000000.0;
        if (time_diff>seconds) break;
        uint16_t key=keys[index];
        std::cout<<"Thread "<<std::left<<std::setw(3)<<num_thread<<": ";
        log<<"Thread "<<std::left<<std::setw(3)<<num_thread<<": ";
        bool result;
        
        if(rand()%2)
        {
            result=write_test(&chain_nodes,key,log,&tv_start,ret.count);
        }
        else
        {
            result=read_test(&chain_nodes,key,log,&tv_start,ret.count);
        }
        ret.count++;
        if(result) ret.count_success++;
        else ret.count_fail++;

        

    }
    
}
