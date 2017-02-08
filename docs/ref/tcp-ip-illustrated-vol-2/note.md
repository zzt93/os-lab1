## 1.6 System Calls and Library Function
From an implementor's point of view, the distinction between a system call and a library function is fundamental

- re-design my system call and library function?


## 1.7 Network Implementation Overview

- The socket layer is a protocol-independent interface to the protocol-dependent layer below.All system calls start at the protocol-independent socket layer. For example, the protocol-independent code in the socket layer for the bind system call comprises a few dozen lines ofcode: these verify that the first argument is a valid socket descriptor and that the secondargument is a valid pointer in the process. The protocol-dependent code in the layer below isthen called, which might comprise hundreds of lines of code.
- The protocol layer contains the implementation of the four protocol families that wementioned earlier (TCP/IP, XNS, OSI, and Unix domain). Each protocol suite may have itsown internal structure, which we don't show in Figure 1.3. For example, in the Internetprotocol suite, IP is the lowest layer (the network layer) with the two transport layers (TCPand UDP) above IP.
- The interface layer contains the device drivers that communicate with the network devices.

## 1.8 Descriptors

 - The call to socket by our process ends up allocating the lowest unused descriptor (3 in our example). This descriptor is used by the process in all subsequent system calls that refer to this socket.
 - The following kernel structures are allocated and linked together: a file structure of type DTYPE_SOCKET, a socket structure, and an inpcb structure. Lots of initialization is performed on these structures that we don't show: the file structure is marked for read and write (since the call to socket always returns a descriptor that can be read or written), the default sizes of the input and output buffers are set in the socket structure, and so on.
 - We showed non socket descriptors for our standard input, output, and error to show that all descriptors end up at a file structure, and it is from that point on that differences appear between socket descriptors and other descriptors.

## 1.9 Mbufs (Memory Buffers) and Output Processing

- sendto:One of the first things done by the socket layer for this system call is to verify that these arguments are valid (i.e., the pointer points to a piece of memory in the address space of the process)
- then copy the socket addressstructure into an mbuf
- the socket layer copies the data buffer specified in the call to sendto into one or more mbufs
- the protocol layer corresponding to the socket descriptor (a UDP socket) is called.
- The UDP output routine then fills in the UDP header and as much of the IP header as it can. Forexample, the destination address in the IP header can be set, but the IP checksum will be left for the IPoutput routine to calculate and store.
- The UDP checksum is calculated and stored in the UDP header
- The IP output routine fills in the remaining fields in the IP header including the IP checksum,determines the outgoing interface to which the datagram should be given (this is the IP routingfunction), fragments the IP datagram if necessary, and calls the interface output function.
- The first function of the Ethernet output function is to convert the 32-bit IP address into its corresponding 48-bit Ethernet address -- ARP
- The Ethernet output routine then prepends a 14-byte Ethernet header to the first mbuf in the chain,
  immediately before the IP header


## 1.10 Input Processing

Input processing is different from the output processing just described because the input is asynchronous. Input processing is different from the output processing just described because the input is asynchronous.

- The Ethernet device receive frame, read the destination address and determine whether to keep the remaining IP packet
- The Ethernet device driver processes the interrupt and, assuming it signifies a normal receive-complete condition, the data bytes are read from the device into an mbuf chain.
- The device driver passes the mbuf to a general Ethernet input routine which looks at the type field in the Ethernet frame to determine which protocol layer should receive the packet
- In this example, the type field will specify an IP datagram, causing the mbuf to be added to the IP input queue.
- The IP input routine verifies the IP header checksum,
  - processes any IP options,
  - verifies that the datagram was delivered to the right host (by comparing thedestination IP address of the datagram with the host's IP addresses),
  - and forwards the datagram if thesystem was configured as a router and the datagram is destined for some other IP address.
  - If the IPdatagram has reached its final destination, the protocol field in the IP header specifies which protocol'sinput routine is called: ICMP, IGMP, TCP, or UDP
- The UDP input routine verifies the fields in the UDP header (the length and optional checksum) and then determines whether or not a process should receive the datagram
  - A process can receive all datagrams destined to a specified UDP port, or the process can tell the kernel to restrict the datagrams it receives based on the source and destination IP addresses and source and destination port numbers
- In our example, the UDP input routine starts at the global variable udb (Figure 1.5) and goes through the linked list of UDP protocol control blocks, looking for one with a local port number ( inp_lport ) that matches the destination port number of the received UDP datagram.
- inp_socket member of this PCB points to the corresponding socket structure, allowing the received data to be queued for the correct socket
- Our process has been asleep in the kernel, blocked in its call to recvfrom, and the process now wakes up



In our example program we never specify the local port number for our application. We'll see in Exercise 23.3 that a side effect of writing the first UDP datagram to a socket that has not yet bound a local port number is the automatic assignment by the kernel of a local port number (termed an ephemeral port) to that socket. That's how the inp_lport member of the PCB for our socket gets set to some nonzero value.

# Chapter 2 Mbufs

- use linked list like structure to facilitate memory management: reduce copy

## 2.1 Introduction

Networking protocols place many demands on the memory management facilities of the kernel. These demands include easily manipulating buffers of varying sizes, prepending and appending data to the buffers as the lower layers encapsulate data from higher layers, removing data from buffers (as headers are removed as data packets are passed up the protocol stack), and minimizing the amount of data copied for all these operations. The performance of the networking protocols is directly related to the memory management scheme used within the kernel.

## 2.4 mbuf Structure

M_EOR is set in an mbuf containing the end of a record. The Internet protocols (e.g., TCP) never set this flag, since TCP provides a byte-stream service without any record boundaries. The OSI and XNS transport layers, however, do usethis flag. We will encounter this flag in the socket layer, since this layer is protocol independent and handles data to and from all the transport layers.