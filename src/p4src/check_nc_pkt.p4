
action NetChain_delete()
{  
    clone(CloneType.I2E, 100);
    
}
action NetChain_insert()
{
    clone(CloneType.I2E, 100);
}
action Forward_back()
{
    
    bit<32> temp;
    bit<16> tempport;
    temp=hdr.ipv4.dstAddr;
    hdr.ipv4.dstAddr=hdr.ipv4.srcAddr;
    hdr.ipv4.srcAddr=temp;
    tempport=hdr.udp.srcPort;
    hdr.udp.srcPort=hdr.udp.dstPort;
    hdr.udp.dstPort=tempport;
    hdr.ipv4.ttl=255;   
    hdr.netchain.operation=5;
    
}
action Forward_next_node()
{
    hdr.netchainlen.len=hdr.netchainlen.len-1;
    hdr.netchainip[0].setInvalid();
    
    hdr.ipv4.totalLen=hdr.ipv4.totalLen-4;
    hdr.udp.payloadlen=hdr.udp.payloadlen-4;
    hdr.udp.checksum=0;
    if(hdr.netchainlen.len==0)
        Forward_back();
    else
    {
       
        hdr.ipv4.dstAddr=hdr.netchainip[1].ipaddr;
        hdr.ipv4.ttl=255;
    }

}
action Forward_next_2node()
{
    hdr.netchainlen.len=hdr.netchainlen.len-1;
    hdr.netchainip[0].setInvalid();
    
    hdr.ipv4.totalLen=hdr.ipv4.totalLen-4;
    hdr.udp.payloadlen=hdr.udp.payloadlen-4;
    hdr.udp.checksum=0;
    if(hdr.netchainlen.len==0)
        Forward_back();
    else
    {
       
        hdr.ipv4.dstAddr=hdr.netchainip[2].ipaddr;
        hdr.ipv4.ttl=255;
    }

}


action NetChain_write(bit<14> index)
{
    key_value_reg.write((bit<32>)index,hdr.netchain.Value);
    Forward_next_node();  
}

action NetChain_read(bit<14> index)
{
    
    key_value_reg.read(hdr.netchain.Value,(bit<32>)index);
    hdr.netchainlen.len=1;
    hdr.ipv4.totalLen=50;
    hdr.udp.payloadlen=30;
    hdr.netchainip[1].setInvalid();
    hdr.netchainip[2].setInvalid();
    Forward_next_node();
    
}
action NetChain_transfer(bit<14> index)
{
    key_value_reg.read(hdr.netchain.Value,(bit<32>)index);
    hdr.netchain.operation=1;
    Forward_next_node();
}



action assignseq()
{
    meta.seq=meta.seq+1;
    hdr.netchain.sequence=meta.seq;
    seq_reg.write((bit<32>)0,meta.seq);
}
action modify_dst(ip4Addr_t newip)
{
    hdr.ipv4.dstAddr=newip;
    hdr.netchainip[0].ipaddr=newip;
}
action modify_dst_my(ip4Addr_t newip)
{
    hdr.ipv4.dstAddr=newip;
    hdr.netchainip[1].ipaddr=newip;
}


table find_key_index {
    key={
        hdr.netchain.Key : exact;
        hdr.netchain.operation: exact;
    }
    actions= {
        
        NetChain_read;
        NetChain_write;
        NetChain_transfer;
        drop;

    }
    size=REG_SIZE;
    default_action=drop;
}


table pkt_for_me {
    key={
        hdr.ipv4.dstAddr : lpm;
        hdr.udp.dstPort : exact;
    }
    actions= {
        NoAction;
    }
    size=128;
    default_action=NoAction;
}
table op{
    key={
        hdr.netchain.operation:exact;
    }
    actions={
        NetChain_insert;
        NetChain_delete;
        drop;
    }
    default_action=drop;

}

table seq{
    key=
    {
        hdr.netchain.sequence:exact;
    }
    actions={
        assignseq;
        NoAction;
    }
    default_action=NoAction;
}
table recovery{
    key=
    {
        hdr.ipv4.dstAddr:lpm;
        hdr.netchain.Key:exact;
    }
    actions={

        modify_dst;
        NoAction;
    }
    default_action=NoAction;
}

table recovery_my{
    key=
    {
        hdr.ipv4.dstAddr:lpm;
        hdr.netchain.Key:exact;
    }
    actions={

        modify_dst_my;
        NoAction;
    }
    default_action=NoAction;
}
