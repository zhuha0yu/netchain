from scapy.all import *

class NetChain(Packet):
    name = "NetChain"
    fields_desc=[ShortField("SwitchCount",0),
                 
                 FieldListField("SwitchIPs", ["127.0.0.1"], IPField("", "127.0.0.1"), count_from = lambda pkt: pkt.SwitchCount),
                 ShortEnumField("Operation" , 1 ,{ 1: "write", 2: "read" , 3: "insert" ,4:"delete",5:'fin',6:'transfer'} ),
                 IntField("Sequence",1),     
                 ShortField("Key",0),
                 LongField("Value",0)     
                ]
    def mysummary(self):
	return self.sprintf("switch count: %NetChain.SwitchCount% \nOperation: %NetChain.Operation% \nIPs chain: %SwitchIPs%")
	
