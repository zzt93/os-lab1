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
 - We showed nonsocket descriptors for our standard input, output, and error to show that all descriptors end up at a file structure, and it is from that point on that differences appear between socket descriptors and other descriptors.


## 1.10 Input Processing

Input processing is different from the output processing just described because the input is asynchronous. Input processing is different from the output processing just described because the input is asynchronous.

In our example program we never specify the local port number for our application. We'll see in Exercise 23.3 that a side effect of writing the first UDP datagram to a socket that has not yet bound a local port number is the automatic assignment by the kernel of a local port number (termed an ephemeral port) to that socket. That's how the inp_lport member of the PCB for our socket gets set to some nonzero value.

# Chapter 2 Mbufs
## 2.1 Introduction

Networking protocols place many demands on the memory management facilities of the kernel. These demands include easily manipulating buffers of varying sizes, prepending and appending data to the buffers as the lower layers encapsulate data from higher layers, removing data from buffers (as headers are removed as data packets are passed up the protocol stack), and minimizing the amount of data copied for all these operations. The performance of the networking protocols is directly related to the memory management scheme used within the kernel.

## 2.4 mbuf Structure

M_EOR is set in an mbuf containingthe end of a record. The Internet protocols (e.g., TCP) never set this flag, since TCP provides a byte-stream service without any record boundaries. The OSI and XNS transport layers, however, do usethis flag. We will encounter this flag in the socket layer, since this layer is protocol independent andhandles data to and from all the transport layers.