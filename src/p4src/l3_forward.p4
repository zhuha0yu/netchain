action drop() {
        mark_to_drop(standard_metadata);
    }
action set_nhop(macAddr_t dstAddr, egressSpec_t port) {

        //set the src mac address as the previous dst, this is not correct right?
        hdr.ethernet.srcAddr = hdr.ethernet.dstAddr;

       //set the destination mac address that we got from the match in the table
        hdr.ethernet.dstAddr = dstAddr;

        //set the output port that we also get from the table
        standard_metadata.egress_spec = port;

        //decrease ttl by 1
        hdr.ipv4.ttl = hdr.ipv4.ttl - 1;
    }



table ipv4_lpm {
    key = {
        hdr.ipv4.dstAddr: lpm;
    }
    actions = {
        set_nhop;
        drop;
    }
    size = 1024;
    default_action = drop;
}
table drop_table{
    key={
        hdr.ipv4.srcAddr:exact;
    }
    actions={
        NoAction;
        drop;
    }
    default_action=drop;
}