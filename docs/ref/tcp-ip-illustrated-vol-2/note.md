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