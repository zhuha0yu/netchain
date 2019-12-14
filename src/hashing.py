import hashlib
import random
import struct
import copy
class consistent_hashing:

    def __init__(self,num_virtual_node,num_switch,num_f):
        self.num_virtual_node=num_virtual_node
        self.num_switch=num_switch
        self.num_f=num_f
    def assign_virtual_node_to_sw(self,active_switch):
        self.list_node_sw=[]
        self.list_sw_node={}
        for i in range(len(active_switch)):
            self.list_sw_node.update({active_switch[i]:[]})

        #randomly assign node to switch
        end=self.num_virtual_node*1.0/self.num_switch
        for i in range(self.num_virtual_node):
            sw_ass=random.randint(0,self.num_switch-1)
            while len(self.list_sw_node[active_switch[sw_ass]])>=end:
                sw_ass=random.randint(0,self.num_switch-1)
            sw_ass=active_switch[sw_ass]
            self.list_node_sw.append(sw_ass)
            self.list_sw_node[sw_ass].append(i)
    
    def calculate_hashing(self,key):
        hashing_value=int(hashlib.md5(str(key)).hexdigest()[-8:],16)>>22

        #2^32 values in a ring, assigned to 2^10 virutal nodes, which means each node have 2^22 values,
        #need to divide by 2^22 to see which node it is
        return hashing_value

    
    def get_node_assignment(self):
        return self.list_node_sw
    def update_list_sw_node(self):
        for i in range(len(self.list_node_sw)):
            self.list_sw_node[self.list_node_sw[i]].append(i)
    def find_chain_node(self,node,mode):
        if mode==1:
            #search forward
            list_sw=[]
            while len(list_sw)<=self.num_f:               
                if self.list_node_sw[node] not in list_sw:
                    list_sw.append(self.list_node_sw[node])
                node=(node+1) % self.num_virtual_node
            return list_sw
        elif mode==2:
            #in middle
            list_sw=[]
            this=self.list_node_sw[node]
            while(this==self.list_node_sw[node]):
                node=node-1
            while len(list_sw)<=self.num_f:
                if self.list_node_sw[node] not in list_sw:
                    list_sw.append(self.list_node_sw[node])
                node=(node+1) % self.num_virtual_node
            return list_sw
        elif mode==3:
            #search backward
            list_sw=[]
            while len(list_sw)<=self.num_f:
                if self.list_node_sw[node] not in list_sw:
                    list_sw.append(self.list_node_sw[node])
                node=(node-1) % self.num_virtual_node
            return list_sw
    def replace_chain_node(self,chain,sw_fail,sw_new):
        for node in copy.deepcopy(self.list_sw_node[sw_fail]):
            
            if self.find_chain_node(node,chain.index(sw_fail)+1)==chain:
                self.list_node_sw[node]=sw_new
                self.list_sw_node[sw_fail].remove(node)
                self.list_sw_node[sw_new].append(node)
            
