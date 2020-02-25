#include"chain.h"
#include"queue.h"
#include"hashing.h"
#include<sys/time.h>
#include<fstream>
#include<iomanip>
#include<thread>
#define MAX_KEY_SIZE 8192
#ifndef _EVALUATE_H
#define _EVALUATE_H
struct evaluate_result
{
    int count=0;
    int count_success=0;
    int count_fail=0;
};
#endif
void start_evaluate(chain_node* chain_nodes);
void thrp_test(chain_node chain_nodes,int num_thread,int seconds,evaluate_result& ret,std::string filename);
bool read_test(chain_node* chain_nodes,uint16_t key,std::ofstream& log,timeval* starttime,int count);
bool write_test(chain_node* chain_nodes,uint16_t key,std::ofstream& log,timeval* starttime,int count);
