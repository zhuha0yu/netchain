
action Failover_my()
{
    hdr.netchainip[1].setInvalid();
    Forward_next_2node();
}
table fast_failover {
    key={
        hdr.ipv4.dstAddr : lpm;
    }
    actions= {
        NoAction;
        Forward_next_node;
        
    }
    size=64;
    default_action=NoAction;
}

table fast_failover_my {
    key={
        hdr.ipv4.dstAddr : lpm;
    }
    actions= {
        NoAction;
        Failover_my;
        
    }
    size=64;
    default_action=NoAction;
}
