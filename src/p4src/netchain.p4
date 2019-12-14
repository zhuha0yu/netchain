/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

//My includes
#include "include/headers.p4"
#include "include/parsers.p4"

/*************************************************************************
************   C H E C K S U M    V E R I F I C A T I O N   *************
*************************************************************************/

control MyVerifyChecksum(inout headers hdr, inout metadata meta) {
    apply {  }
}

/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {
    
    register<bit<64>>(REG_SIZE) key_value_reg;
    register<bit<32>>(1) seq_reg;
    #include "l3_forward.p4" 
    #include "check_nc_pkt.p4"
    #include "fast_fo.p4"
    apply {
            if(!hdr.netchain.isValid())
                ipv4_lpm.apply();
            else
            {
                if(pkt_for_me.apply().hit)
                {
                    seq_reg.read(meta.seq,(bit<32>)0);
                    if(!seq.apply().hit&&meta.seq>hdr.netchain.sequence)
                    {
                        drop_table.apply();
                    }
                    else
                    {
                        seq_reg.write((bit<32>)0,hdr.netchain.sequence);
                        if(op.apply().hit)
                        {
                            
                        }
                        else
                        
                            if(find_key_index.apply().hit)
                            {
                                meta.fo=0;
                                if(!recovery_my.apply().hit)
                                    fast_failover_my.apply();
                                
                                
                                ipv4_lpm.apply();   
                            }
                        
                        
                        
                        
                        
                    }
                }
                else
                {   
                    if(!recovery.apply().hit)
                        fast_failover.apply();
                    ipv4_lpm.apply();
                }
            }


        }
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyEgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {
    apply {

    }
}

/*************************************************************************
*************   C H E C K S U M    C O M P U T A T I O N   **************
*************************************************************************/

control MyComputeChecksum(inout headers hdr, inout metadata meta) {
     apply {
	update_checksum(
	    hdr.ipv4.isValid(),
            { hdr.ipv4.version,
	          hdr.ipv4.ihl,
              hdr.ipv4.dscp,
              hdr.ipv4.ecn,
              hdr.ipv4.totalLen,
              hdr.ipv4.identification,
              hdr.ipv4.flags,
              hdr.ipv4.fragOffset,
              hdr.ipv4.ttl,
              hdr.ipv4.protocol,
              hdr.ipv4.srcAddr,
              hdr.ipv4.dstAddr },
              hdr.ipv4.hdrChecksum,
              HashAlgorithm.csum16);
    
    /*
    update_checksum(
	    hdr.udp.isValid(),
            { hdr.ipv4.srcAddr,
              hdr.ipv4.dstAddr,
              (bit<8>)0,
              hdr.ipv4.protocol,
              hdr.udp.payloadlen,
              hdr.udp.srcPort,
              hdr.udp.dstPort,
              hdr.udp.payloadlen,
              (bit<16>)0,
              hdr.netchainlen.len,
              hdr.netchain.operation,
              hdr.netchain.sequence,
              hdr.netchain.Key,
              hdr.netchain.Value },
              hdr.udp.checksum,
              HashAlgorithm.csum16);
     */
     
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

//switch architecture
V1Switch(
MyParser(),
MyVerifyChecksum(),
MyIngress(),
MyEgress(),
MyComputeChecksum(),
MyDeparser()
) main;
