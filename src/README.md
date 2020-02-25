# Source Code

## Implementation TODO list

* Data plane
  * [x] Netchain Header defined, Parser of NetChain finished, Write and Read Queue. Haoyu.
  * [x] Forwarding and routing . Haoyu Francisco.
  * [x] Implement sequence on switch but not host to enable parallal operations.
  * [x] Fast-recovery dealing. Haoyu.
* Controller
  * [x] Topology init and routing table filling. Haoyu.
  * [x] Check NetChain table filling. Francisco.
  * [x] Filling index table and implement Insert and Delete function. Francisco.
  * [x] Fast-failover table filling. Francisco.
  * [x] Fast recovery when failure occurs. Francisco.
* Host
  * [x] Generate and send Netchain queues. Francisco.
  * [x] Scapy Netchain header defined and multiple node routing is tested. Haoyu.
  * [x] To make the host looks better and show what we received. Haoyu.

## Structure

our source code will be 3 parts:

* Controller
  * `routingcontroller.py` : defines all the stuff that controller will do for initialization, including routing table establishment, mandatory table filling.
  * `NetChain-controller.py` : includes the handler for insert/delete operations and script for fast fail-over and fast recovery.
  * `hashing.py` : includes the consistent hashing algorithm and node assignment.
* Data plane
  * `p4src/netchain.p4` : the main entrance of data plane functions.
  * `p4src/include/*` : defines some basic components in data plane: parser, deparser, headers and constant valuse.
  * `p4src/l3_forwarding.p4` : defines the l3 forwarding stargy.
  * `p4src/check_nc_pkt.p4` : do all things about netchain: check if it is a netchain packet, queueing and sent it away or back.
  * `p4src/fast-fo.p4` : do fast fail-over processes.
* Host
  * `hostsrc/send.cpp` : the main entrance of host functions, initials the chain state, parses user input and sends the NetChain packet.
  * `hostsrc/chain.cpp` : defines the data stucture uses to store chain information, and the functions that updates it.
  * `hostsrc/md5.cpp` : defines an function to calculate the MD5 hashing value of a string.
  * `hostsrc/hashing.cpp` : defined a series of functions to obtain the hashing result of a key.
  * `hostsrc/queue.cpp` : generate and send the NetChain packet, and proceed the response.

## Build and installation

### start NetChain without fast fail-over and fast recovery

To start, please run command:

    sudo make switch

In another terminal please run:

    sudo make controller

to start the p4 controller.

Then please start a xterm window for h1 or h2 and start host program using:

    ./host

Now everything is ready, please start to queue or test. To start evaluate, please enter `evaluate` when asked to enter commands. To load the key and chain assignment, please enter `update`.

To test fast fail-over and fast-recovery, please input a legal switch name in `NetChain-controller.py`. e.g. `s2`. The fast fail-over will perform. After it finishes,please enter anything and press enter to start fast-recovery.

