/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/
#define REG_SIZE 4096
#define NETCHAIN_PORT 35678
const bit<16> TYPE_IPV4 = 0x800;
const bit<16> PORT_NETCHAIN=35678;

typedef bit<9>  egressSpec_t;
typedef bit<48> macAddr_t;
typedef bit<32> ip4Addr_t;


header ethernet_t {
    macAddr_t dstAddr;
    macAddr_t srcAddr;
    bit<16>   etherType;
}

header ipv4_t {
    bit<4>    version;
    bit<4>    ihl;
    bit<6>    dscp;
    bit<2>    ecn;
    bit<16>   totalLen;
    bit<16>   identification;
    bit<3>    flags;
    bit<13>   fragOffset;
    bit<8>    ttl;
    bit<8>    protocol;
    bit<16>   hdrChecksum;
    ip4Addr_t srcAddr;
    ip4Addr_t dstAddr;
}

header tcp_t{
    bit<16> srcPort;
    bit<16> dstPort;
    bit<32> seqNo;
    bit<32> ackNo;
    bit<4>  dataOffset;
    bit<4>  res;
    bit<1>  cwr;
    bit<1>  ece;
    bit<1>  urg;
    bit<1>  ack;
    bit<1>  psh;
    bit<1>  rst;
    bit<1>  syn;
    bit<1>  fin;
    bit<16> window;
    bit<16> checksum;
    bit<16> urgentPtr;
}
header udp_t{
    bit<16> srcPort;
    bit<16> dstPort;
    bit<16> payloadlen;
    bit<16> checksum;
}
header netchainlen_t{
    bit<16> len;
}
header netchainip_t{
    bit<32> ipaddr;
}

header netchain_t{
    bit<16> operation;
    bit<32> sequence;
    bit<16> Key;
    bit<64> Value;
}


struct metadata {
    bit<32> seq;
    bit<16> temp;
    bit<14> index;
    bit<2> fo;
}

struct headers {
    ethernet_t      ethernet;
    ipv4_t          ipv4;
    tcp_t           tcp;
    udp_t           udp;
    netchainlen_t   netchainlen;
    netchainip_t[32] netchainip;
    netchain_t      netchain;
}

