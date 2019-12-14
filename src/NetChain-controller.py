from p4utils.utils.topology import Topology
from p4utils.utils.sswitch_API import *

from routingcontroller import *
import socket, struct, os, pickle,thread

from scapy.all import *
from queue import NetChain
from hashing import consistent_hashing
from threading import Thread

num_virtual_node=1024#virtual nodes
num_f=2#f in paper

REG_SIZE = 4096
operations = {'write': 1, 'read': 2, 'insert': 3, 'delete': 4}
saveFiles = {'keys': 1, 'chain': 2}

class NetChainController(object):

    def __init__(self):
        
        
        self.active_switches = ['s2','s3','s4','s5']
        self.failSwitch =set()
        self.sw_memory_key_index = {}	# sw name: 	[keys saved in order]
        self.keys={}
        self.node_keys=[[] for i in range(num_virtual_node)]
        self.ip_addr={}
        self.hashing=consistent_hashing(num_virtual_node,len(self.active_switches),num_f)
        self.hashing.assign_virtual_node_to_sw(self.active_switches)
        self.save_nodes_assignment()
        self.init()
        

    def init(self):
        #Create routing and inital forward in tables
        routeControl = RoutingController()
        for sw, control in routeControl.controllers.items():
            intf = routeControl.topo.get_cpu_port_intf(sw)
            port = routeControl.topo[sw]['interfaces_to_port'][intf]
            print("mirror adding for "+intf+" in port "+str(port))
            control.mirroring_add(100, port)
            #routeControl.main()

        for sw in routeControl.topo.get_p4switches():
            
            #routeControl.read_write_trans(sw)
            routeControl.op(sw)
            routeControl.pkt_for_me(sw)
            routeControl.ipv4_lpm(sw)
            

        self.topo = routeControl.topo
        self.controllers = routeControl.controllers
        self.get_ip_addr()
        self.init_switches_index(self.active_switches)

        
    def assign_key_to_nodes(self, key):
        node=self.hashing.calculate_hashing(key)
        list_sw=self.hashing.find_chain_node(node,1)
        self.node_keys[node].append(key)
        for sw in list_sw:
            if sw in self.active_switches:
                self.insert_index_to_switch_table(sw, key)
        self.keys.update({key:list_sw})
            
        

    def insert_index_to_switch_table(self,sw,key):
        try: 
            idx = self.sw_memory_key_index[sw].index('None')
            self.sw_memory_key_index[sw][idx]=key
                
        except:    
            idx = len(self.sw_memory_key_index[sw])
            self.sw_memory_key_index[sw].append(key)
            
                
        
        print ('\n#### %s: insert to table ####' %sw)
        self.controllers[sw].table_add("find_key_index", "NetChain_read",[str(key),str(2)],[str(idx)])
        self.controllers[sw].table_add("find_key_index", "NetChain_write",[str(key),str(1)],[str(idx)])
        self.controllers[sw].table_add("find_key_index", "NetChain_transfer",[str(key),str(6)],[str(idx)])

    def eliminate_key_from_group(self, key):
        
        node=self.hashing.calculate_hashing(key)
        list_sw=self.hashing.find_chain_node(node,1)
        self.keys.pop(key)
        self.node_keys[node].remove(key)
        for sw in list_sw:
            idx = self.sw_memory_key_index[sw].index(key)
            self.sw_memory_key_index[sw][idx] = 'None'
            if sw not in self.active_switches:
                continue
            self.delete_key_from_switch(sw, key)
        
    
    def delete_key_from_switch(self, sw_name, key):
        print('\n#### %s: delete from table ####' %sw_name) 
        self.controllers[sw_name].table_delete_match("find_key_index",[str(key),])
        


    def save_files_for_host(self, options):
        for op in options: 
            if op == saveFiles['keys']:
                with open('Keys.csv', 'w') as f: 
                    for key, group in self.grpForKey.items():
                        f.write('%s,%s\n'%(key,group[6]))

            elif op == saveFiles['chain']:
                self.save_ipChain()

    def load_nodes_assignment(self):
        try:
            with open('nodes.csv', 'r') as f: 
                self.hashing.list_node_sw=f.readlines()
                self.hashing.update_list_sw_node()
                f.close()
            return True
        except:
            return False
    def save_nodes_assignment(self):
        with open('nodes.csv', 'w') as f: 
            for sw in self.hashing.get_node_assignment():
                f.write(sw+'\n')
            f.close()
    def save_keys_assignment(self):
        a=1

    def init_switches_index(self,sws):
        
        for sw in sws:
            self.sw_memory_key_index.update({sw:[]})

    def get_ip_addr(self):
        self.ip_addr={}
        for sw in self.active_switches:
            self.ip_addr.update({sw:'20.0.'+sw.split('s')[-1]+'.1'})




    def send_back_to_host(self, intf, ether, ip, udp, netchain):
        pkt = Ether(src=get_if_hwaddr(intf), dst='ff:ff:ff:ff:ff:ff')
        if (operations['insert'] == netchain.Operation):
            ips=[]
            for sw in self.keys.get(netchain.Key):
                ips.append(self.ip_addr.get(sw))
            pkt = pkt / IP(src=ip.src,dst=ips[0]) / UDP(dport=udp.dport, sport=udp.sport)
            pkt = pkt/ NetChain(SwitchCount = len(ips), 
                    SwitchIPs = ips, 
                    Operation = 'write', 
                    Sequence = 0, 
                    Key = int(netchain.Key), Value = netchain.Value)
        elif(operations['delete'] == netchain.Operation):
            pkt = pkt / IP(src=ip.dst,dst=ip.src) / UDP(dport=udp.sport, sport=udp.dport)
            pkt = pkt/ NetChain(SwitchCount = 0, 
                    SwitchIPs = [], 
                    Operation = 'fin', 
                    Sequence = 0, 
                    Key = int(netchain.Key), Value = 1)


        sendp(pkt, iface = intf, verbose = False)
        
    def print_packet(self, intf, ip, udp, netchain):
        print "\n\n      Packet received      "
        print "######## interface ########"
        print intf
        print "########### IP ############" 
        print ip.mysummary()
        print "########### UDP ###########"
        print udp.mysummary()
        print "######## NetChain #########"
        print netchain.mysummary()
        print "########### FIN ###########\n"
    

        
   
    def recv_msg_cpu(self,pkt):
    
        interface = pkt.sniffed_on
        switch_name = interface.split("-")[0]
        packetEther = Ether(str(pkt))
        packetIP = IP(packetEther.payload)
        packetUDP = UDP(packetIP.payload)
        packetNetChain = NetChain(packetUDP.payload)		
                #self.print_packet(interface, packetIP, packetUDP, packetNetChain)
                #self.ActiveSwitches = self.check_active_switches()
                
        if packetNetChain.Operation == operations['insert']:
            
            if not self.keys.has_key(packetNetChain.Key):
                self.assign_key_to_nodes(packetNetChain.Key)
                self.send_back_to_host(interface, packetEther, packetIP, packetUDP, packetNetChain)
                

        elif packetNetChain.Operation == operations['delete']:
            if  self.keys.has_key(packetNetChain.Key):
                self.eliminate_key_from_nodes(packetNetChain.Key)
                self.send_back_to_host(interface, packetEther, packetIP, packetUDP, packetNetChain)
                
    def run_cpu_port_loop(self):
        thread.start_new_thread(self.wait_fail_over,())
        cpu_interfaces = [str(self.topo.get_cpu_port_intf(sw_name).replace("eth0", "eth1")) 
                                for sw_name in self.controllers]
        try: 
            sniff(iface=cpu_interfaces,filter='udp dst port 35678 and not ether dst ff:ff:ff:ff:ff:ff', prn=self.recv_msg_cpu)
        except KeyboardInterrupt: 
            self.save_nodes_assignment()
            sys.exit() #on ctrl-c terminate program 

    def wait_fail_over(self):
        while True:
            print 'Enter switch name(i.g. s2) to do fast-failover'
            sw=raw_input()
            if sw not in ['s2','s3','s4','s5']:
                print 'switch name illegal'
                continue
            
            for sw2 in self.topo.get_neighbors(sw):
                if sw2=='sw-cpu':
                    continue
                self.controllers[sw2].table_add('fast_failover','Forward_next_node',["20.0."+str(sw[1:])+".0/24"],[])
                self.controllers[sw2].table_add('fast_failover_my','Failover_my',["20.0."+str(sw[1:])+".0/24"],[])
            print 'Fast_failover finished, to start recovery, enter anything'
            sw3=raw_input()
            self.recovery(sw)
            for sw2 in self.topo.get_neighbors(sw):
                if sw2=='sw-cpu':
                    continue
                self.controllers[sw2].table_delete_match('fast_failover',["20.0."+str(sw[1:])+".0/24"])
                self.controllers[sw2].table_delete_match('fast_failover_my',["20.0."+str(sw[1:])+".0/24"])
            print 'recovery finished'
            
    def recovery(self,failsw):
        
        #24 possible chains will be replaced       
        
        self.possible_chain_permu=[]
        self.get_chain_permutation([],failsw)
        #move all chains:
        for chain in self.possible_chain_permu:
            possible_new_sw=copy.deepcopy(self.active_switches)
            for sw in chain:
                possible_new_sw.remove(sw)
            new_sw=possible_new_sw[random.randint(0,len(possible_new_sw)-1)]
            keys=[]
            for key,chain_key in self.keys.items():
                if chain_key==chain:
                    keys.append(key)
            if(len(keys)!=0):
                
                if(chain.index(failsw)<num_f):
                    #not at tail
                    self.do_recovery(chain[chain.index(failsw)+1],new_sw,failsw,keys)
                else:
                    #at tail
                    self.do_recovery(chain[chain.index(failsw)-1],new_sw,failsw,keys)

            self.hashing.replace_chain_node(chain,failsw,new_sw)

        '''
        for node in self.hashing.list_sw_node[sw]:
            if len(self.node_keys[node])==0:
                continue
            #find all virtual node needs recovery
            newsw=self.active_switches[random.randint(0,len(self.active_switches)-1)]
            self.hashing.list_node_sw[node]=newsw
            self.hashing.list_sw_node[newsw].append(node)
            chain=self.hashing.find_chain_node(node,1)
            self.do_recovery(,self.active_switches[newsw],sw,node)
            #fail at head
            chain=self.hashing.find_chain_node(node,2)
            self.do_recovery(chain[2],self.active_switches[newsw],sw,node)
            #fail at middle
            chain=self.hashing.find_chain_node(node,3)
            self.do_recovery(chain[1],self.active_switches[newsw],sw,node)
            #fail at tail
            '''
            

    def get_chain_permutation(self,chain,sw_fail):
        if len(chain)==num_f+1:
            if sw_fail in chain:
                self.possible_chain_permu.append(copy.deepcopy(chain))
                return
        elif len(chain)>num_f+1:
            return
        
        for sw in self.active_switches:
            if sw not in chain:
                chain.append(sw)
                self.get_chain_permutation(chain,sw_fail)
                chain.remove(sw)



    def do_recovery(self,refsw,newsw,failsw,keys):

        #add new entry, don't need to hold write operation
        #pre sync
        print keys
        for key in keys:
            
            self.insert_index_to_switch_table(newsw,key)
            val1=self.controllers[refsw].register_read('key_value_reg',self.sw_memory_key_index[refsw].index(key))
            self.controllers[newsw].register_write('key_value_reg',self.sw_memory_key_index[newsw].index(key),val1)
            self.keys[key][self.keys[key].index(failsw)]=newsw
            
        #start a thread to transfer, in paper it says that 
        #it don't need to stop writing, just keep syncing
        t = Thread(target=self.sync_value, args=(refsw,newsw,keys))
        t.start()
        t.join()
        #start to add rules to recovery table, need to stop writing
        print'thread'
        for key in keys:
            for sw in self.keys[key]:
                try:
                    self.controllers[sw].table_delete_match('find_key_index',[hex(key),str(1)])
                except:
                    print 'sw '+sw+' have no entry about key '+str(key)
                    continue
            
        for neighborsw in self.topo.get_neighbors(failsw):
            if neighborsw=='sw-cpu':
                continue
            failswip="20.0."+str(failsw[1:])+".0/24"
            for key in keys:
                self.controllers[neighborsw].table_add('recovery','modify_dst',[failswip,str(key)],[self.ip_addr[newsw]])
                self.controllers[neighborsw].table_add('recovery_my','modify_dst_my',[failswip,str(key)],[self.ip_addr[newsw]])

        #re-activate, add write operations
        for key in self.keys:
            for sw in self.keys[key]:
                self.controllers[sw].table_add('find_key_index','NetChain_write',[str(key),str(1)],[str(self.sw_memory_key_index[sw].index(key))])
        


             
    def sync_value(self,refsw,newsw,keys):
        while 1:
            is_sync=True
            for key in keys:
                try:
                    val1=self.controllers[refsw].register_read('key_value_reg',self.sw_memory_key_index[refsw].index(key))
                    val2=self.controllers[newsw].register_read('key_value_reg',self.sw_memory_key_index[newsw].index(key))
                    if(val2!=val1):
                        is_sync=False
                        self.controllers[newsw].register_write('key_value_reg',self.sw_memory_key_index[newsw].index(key),val1)
                except:
                    print self.sw_memory_key_index[refsw].index(key)
                    print self.sw_memory_key_index[newsw].index(key)
                    
            if is_sync:
                return
        





        
   




if __name__ == "__main__":
   controller = NetChainController()
   controller.run_cpu_port_loop() 
            
