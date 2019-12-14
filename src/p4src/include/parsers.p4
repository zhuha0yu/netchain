/*************************************************************************
*********************** P A R S E R  *******************************
*************************************************************************/

parser MyParser(packet_in packet,
                out headers hdr,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {

    state start {

        transition parse_ethernet;

    }

    state parse_ethernet {

        packet.extract(hdr.ethernet);
        transition select(hdr.ethernet.etherType){
            TYPE_IPV4: parse_ipv4;
            default: accept;
        }
    }

    state parse_ipv4 {
        packet.extract(hdr.ipv4);
        transition select(hdr.ipv4.protocol){
            6 : parse_tcp;
            17: parse_udp;
            default: accept;
        }
    }
    
    state parse_udp {
        packet.extract(hdr.udp);
        transition select(hdr.udp.dstPort){
            NETCHAIN_PORT : parse_netchain_len;
            default: accept;
        }
    }
    state parse_netchain{
        packet.extract(hdr.netchain);
        transition accept;
    }

    state parse_netchain_len{
        packet.extract(hdr.netchainlen);
        transition select(hdr.netchainlen.len)
        {
            0:parse_netchain;
            default:parse_netchain_ip1;
        }
    }


    state parse_netchain_ip1
    {
        packet.extract(hdr.netchainip[0]);     
        transition select(hdr.netchainlen.len)
        {
            1 : parse_netchain;
            default: parse_netchain_ip2;
        }  
    }
    state parse_netchain_ip2
    {
        packet.extract(hdr.netchainip[1]);     
        transition select(hdr.netchainlen.len)
        {
            2 : parse_netchain;
            default: parse_netchain_ip3;
        }  
    }

    state parse_netchain_ip3
    {
        packet.extract(hdr.netchainip[2]);     
        transition select(hdr.netchainlen.len)
        {
            3 : parse_netchain;
            default: parse_netchain_ip4;
        }  
    }

    state parse_netchain_ip4
    {
        packet.extract(hdr.netchainip[3]);     
        transition select(hdr.netchainlen.len)
        {
            4 : parse_netchain;
            default: parse_netchain_ip5;
        }  
    }

    state parse_netchain_ip5
    {
        packet.extract(hdr.netchainip[4]);     
        transition select(hdr.netchainlen.len)
        {
            5 : parse_netchain;
            default: parse_netchain_ip6;
        }  
    }

    state parse_netchain_ip6
    {
        packet.extract(hdr.netchainip[5]);     
        transition select(hdr.netchainlen.len)
        {
            6 : parse_netchain;
            default: parse_netchain_ip7;
        }  
    }

    state parse_netchain_ip7
    {
        packet.extract(hdr.netchainip[6]);     
        transition select(hdr.netchainlen.len)
        {
            7 : parse_netchain;
            default: parse_netchain_ip8;
        }  
    }

    state parse_netchain_ip8
    {
        packet.extract(hdr.netchainip[7]);     
        transition parse_netchain;
        
    }

    







    state parse_tcp {
        packet.extract(hdr.tcp);
        transition accept;
    }
}

/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control MyDeparser(packet_out packet, in headers hdr) {
    apply {

        //parsed headers have to be added again into the packet.
        packet.emit(hdr.ethernet);
        packet.emit(hdr.ipv4);

        //Only emited if valid
        packet.emit(hdr.tcp);
        packet.emit(hdr.udp);
        packet.emit(hdr.netchainlen);
        packet.emit(hdr.netchainip);
        packet.emit(hdr.netchain);
    }
}