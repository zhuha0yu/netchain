# Project group15(NetChain)

## Authors

    * Francisco Dumont (fdumontd@student.ethz.ch)
    * Haoyu Zhu (haozhu@student.ethz.ch)

## Introduction

This is the cource project of Advanced Topics in Communication Networks. The project is to reproduce a paper: [NetChain: Scale-Free Sub-RTT Coordination](https://www.usenix.org/system/files/conference/nsdi18/nsdi18-jin.pdf).

This project contains 4 folders:

* `src/*` contains our implementation files and testing scripts.
* `evaluation/*` contains evaluation log and result.
* `report/*` contains report of the project.
* `presentation` contains presentation slide.

## NetChain basis

NetChain is a switch based distributed key-value storage system to archive a quick and reliable key-value storage. The consistent hashing is used to keep scalaibility and virtual nodes is used to keep load balanced. A chain replication is used to boost the sync between switches. A fast fail-over and recovery method is implemented in the project to make it robust.

The Netchain packet is structed using UDP payload and have some special structure to routing through the chain.

## NetChain UDP messgae frame stracture

### Basic Headers

* Ethernet Header, 14Bytes.
* IPv4 Header, 20Bytes.
* UDP Header, 8Bytes.

### NetChain Headers and payloads

* SC(Chain node counts), 2Bytes, maximum 32.
* S0...Sn(Chain node ip addresses), variable length, maximum 32 nodes, 128Bytes.
* OP(Operation of this queue), 2Bytes.
* SEQ(Sequence number), 4Bytes.
* KEY(Queue key), 2Bytes.
* VALUE(Queue return value), default 0 for read and delete, 8Bytes.

### Topology

![alt Topology](./report/figures/Topology.png "Topology")
