from p4utils.utils.topology import Topology
from p4utils.utils.sswitch_API import SimpleSwitchAPI

class RoutingController(object):

    def __init__(self):

        self.topo = Topology(db="topology.db")
        self.controllers = {}
        self.init()

    def init(self):
        self.connect_to_switches()
        self.reset_states()
        self.set_table_defaults()

    def reset_states(self):
        [controller.reset_state() for controller in self.controllers.values()]

    def connect_to_switches(self):
        for p4switch in self.topo.get_p4switches():
            thrift_port = self.topo.get_thrift_port(p4switch)
            self.controllers[p4switch] = SimpleSwitchAPI(thrift_port)

    def set_table_defaults(self):
        for controller in self.controllers.values():
            controller.table_set_default("ipv4_lpm", "drop", [])
            


    def ipv4_lpm(self,sw):
        for host in self.topo.get_hosts_connected_to("s1"):
            dstip=self.topo.get_host_ip(host)
            shortestways=self.topo.get_shortest_paths_between_nodes(sw, host)
            nhopmac=self.topo.node_to_node_mac(shortestways[0][1],sw)
            nhopport=self.topo.node_to_node_port_num(sw, shortestways[0][1])
            self.controllers[sw].table_add("ipv4_lpm", "set_nhop", [dstip+'/24'],[nhopmac, str(nhopport)])
            print("From switch "+str(sw)+" to host "+str(dstip)+", next hop is "+str(shortestways[0][1])+", egress port is "+str(nhopport)+'\n')

        for sw2 in self.controllers.keys():
            if sw==sw2 :
                continue
            shortestways=self.topo.get_shortest_paths_between_nodes(sw, sw2)
            nhopmac=self.topo.node_to_node_mac(shortestways[0][1],sw)
            nhopport=self.topo.node_to_node_port_num(sw, shortestways[0][1])
            dstip="20.0."+str(sw2[1:])+".0/24"
            self.controllers[sw].table_add("ipv4_lpm", "set_nhop", [dstip],[nhopmac, str(nhopport)])
            print("From switch "+str(sw)+" to switch "+str(sw2)+" at "+str(dstip)+", next hop is "+str(shortestways[0][1])+", egress port is "+str(nhopport)+'\n')
    def op(self,sw):
        self.controllers[sw].table_add("op","NetChain_insert",[str(3)],[])
        self.controllers[sw].table_add("op","NetChain_delete",[str(4)],[])
    def pkt_for_me(self,sw):
        self.controllers[sw].table_add("pkt_for_me", "NoAction", ["20.0."+str(sw[1:])+".0/24",'35678'],[])

    def seq(self,sw):
        self.controllers[sw].table_add("seq","assignseq",[str(0)],[])
                
    def read_write_trans(self,sw):
            self.controllers[sw].table_add("read_write_trans","NetChain_write",["20.0."+str(sw[1:])+".1",str(1)],[])
            self.controllers[sw].table_add("read_write_trans","NetChain_read",["20.0."+str(sw[1:])+".1",str(2)],[])
            self.controllers[sw].table_add("read_write_trans","NetChain_transfer",["20.0."+str(sw[1:])+".1",str(6)],[])


    




if __name__ == "__main__":
    controller = RoutingController()
