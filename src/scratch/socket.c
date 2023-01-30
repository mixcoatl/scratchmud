/*!
 * \file socket.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup socket
 */
#define _SCRATCH_SOCKET_C_

#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>
#include <scratch/socket.h>
#include <scratch/utility.h>

/*!
 * Accepts a connection.
 * \addtogroup socket
 * \param socket the socket upon which to accept a connection
 * \return a new socket or NULL
 * \sa SocketClose(Socket*)
 */
Socket *SocketAccept(Socket *socket) {
  Socket *accepted = NULL;
  if (!socket) {
    Log(L_ASSERT, "Invalid `socket` Socket.");
  } else if (socket->handle == INVALID_SOCKET) {
    Log(L_ASSERT, "Control socket already closed.");
  } else {
    /* Peer address */
    SOCKADDR peer;
    socklen_t peerSZ = sizeof(peer);

    /* Accepted connection */
    MemoryCreate(accepted, Socket, 1);
    MemoryZero(&accepted->address, SOCKADDR, 1);
    accepted->bytesReceived = 0;
    accepted->bytesSent = 0;
    accepted->handle = accept(socket->handle, &peer, &peerSZ);

    if (accepted->handle == INVALID_SOCKET) {
      Log(L_SYSTEM, "accept() failed: errno=%d.", errno);
      SocketFree(accepted), accepted = NULL;
    } else {
      MemoryCopy(&accepted->address, &peer, SOCKADDR, 1);
      SocketNonBlocking(accepted);
    }
  }
  return (accepted);
}

/*!
 * Constructs a new socket.
 * \addtogroup socket
 * \return a new socket or NULL
 * \sa SocketFree(Socket*)
 * \sa SocketFreeV(void*)
 */
Socket *SocketAlloc(void) {
  Socket *newSocket;
  MemoryCreate(newSocket, Socket, 1);
  newSocket->handle = socket(AF_INET, SOCK_STREAM, 0);
  if (newSocket->handle == INVALID_SOCKET) {
    Log(L_SYSTEM, "socket() failed: errno=%d.", errno);
    MemoryFree(newSocket);
  } else {
    MemoryZero(&newSocket->address, SOCKADDR, 1);
    newSocket->bytesReceived = 0;
    newSocket->bytesSent = 0;
  }
  return (newSocket);
}

/*!
 * Returns whether a socket is a member of a file descriptor set.
 * \addtogroup socket
 * \param socket the socket whose membership to test
 * \param fdset the file descriptor set to test for the specified socket
 * \return true if the specified file descriptor set contains the specified socket
 */
bool SocketCheck(
	Socket *socket,
	const fd_set *fdset) {
  if (!socket || !fdset)
    return (false);

  if (socket->handle == INVALID_SOCKET)
    return (false);

  return FD_ISSET(socket->handle, fdset);
}

/*!
 * Terminates the OS socket library.
 * \addtogroup socket
 * \sa SocketStartup()
 */
void SocketCleanup(void) {
#ifdef _WIN32
  if (WSACleanup() != 0) {
    const int error = WSAGetLastError();
    Log(L_SYSTEM, "WSACleanup() failed: error=0x%x.", error);
  }
#endif /* _WIN32 */
}

/*!
 * Closes a socket.
 * \addtogroup socket
 * \param socket the socket to close
 * \sa SocketClosed(const Socket*)
 */
void SocketClose(Socket *socket) {
  if (!socket) {
    Log(L_ASSERT, "Invalid `socket` Socket.");
  } else {
    if (socket->handle != INVALID_SOCKET) {
#ifdef _WIN32
      if (closesocket(socket->handle) != 0) {
        const int error = WSAGetLastError();
        Log(L_SYSTEM, "closesocket() failed: error=0x%x.", error);
      }
#else
      if (close(socket->handle) < 0)
        Log(L_SYSTEM, "close() failed: errno=%d.", errno);
#endif /* _WIN32 */
    }
    MemoryZero(&socket->address, SOCKADDR, 1);
    socket->bytesReceived = 0;
    socket->bytesSent = 0;
    socket->handle = INVALID_SOCKET;
  }
}

/*!
 * Returns whether a socket is closed.
 * \addtogroup socket
 * \param socket the socket whose closed state to return
 * \return true if the specified socket is closed
 * \sa SocketClose(Socket*)
 */
bool SocketClosed(const Socket *socket) {
  return (!socket || socket->handle == INVALID_SOCKET);
}

/*!
 * Frees a socket.
 * \addtogroup socket
 * \param socket the socket to free
 * \sa SocketAlloc()
 * \sa SocketFreeV(void*)
 */
void SocketFree(Socket *socket) {
  if (socket) {
    SocketClose(socket);
    MemoryFree(socket);
  }
}

/*!
 * Frees a socket.
 * \addtogroup socket
 * \param socket the socket to free
 * \sa SocketAlloc()
 * \sa SocketFree(Socket*)
 */
void SocketFreeV(void *socket) {
  SocketFree(socket);
}

#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif /* O_NONBLOCK */

/*!
 * Configures non-blocking mode.
 * \addtogroup socket
 * \param socket the socket whose non-blocking mode to configure
 */
void SocketNonBlocking(Socket *socket) {
  if (!socket) {
    Log(L_ASSERT, "Invalid `socket` Socket.");
  } else {
    /* Get current socket flags */
    int flags = fcntl(socket->handle, F_GETFL, 0);

    /* Check for errors, then set socket flags */
    if (flags < 0) {
      Log(L_SYSTEM, "fcntl() failed: errno=%d.", errno);
    } else if (fcntl(socket->handle, F_SETFL, flags | O_NONBLOCK) < 0) {
      Log(L_SYSTEM, "fcntl() failed: errno=%d.", errno);
    }
  }
}

/*!
 * Binds a socket and listens.
 * \addtogroup socket
 * \param socket the socket to open
 * \param address the endpoint address or NULL
 * \param port the endpoint port
 * \return true if the specified socket was successfully opened
 */
bool SocketOpen(
	Socket *socket,
	const char *address,
	const uint16_t port) {
  bool result = false;
  if (!socket) {
    Log(L_ASSERT, "Invalid `socket` Socket.");
  } else {
    /* Peer */
    SOCKADDR_IN peer;
    socklen_t peerSZ = sizeof(peer);
    MemoryZero(&peer, SOCKADDR_IN, 1);
    peer.sin_family = AF_INET;
    peer.sin_port = htons(port);

    /* Decode network address */
    if (inet_pton(AF_INET, address, &peer.sin_addr) < 0) {
      Log(L_SYSTEM, "inet_pton() failed: %d.", errno);
    /* Bind to network interface */
    } else if (bind(socket->handle, (SOCKADDR*) &peer, peerSZ) < 0) {
      Log(L_SYSTEM, "bind() failed: %d.", errno);
    /* Listen for connections */
    } else if (listen(socket->handle, 5) < 0) {
      Log(L_SYSTEM, "listen() failed: %d.", errno);
    } else {
      /* Determine local name */
      peerSZ = sizeof(socket->address);
      if (getsockname(socket->handle, &(socket->address), &peerSZ) < 0) {
        Log(L_SYSTEM, "getsockname() failed: %d.", errno);
        MemoryZero(&socket->address,  SOCKADDR, 1);
      }
      result = true;
    }
  }
  return (result);
}

/*!
 * Reads from a socket.
 * \addtogroup socket
 * \param socket the socket from which to read
 * \param messg the message buffer
 * \param messglen the length of the specified buffer
 * \return the number of bytes read from the network stream, or -1
 * \sa SocketWrite(Socket*, const void*, const size_t)
 */
ssize_t SocketRead(
	Socket *socket,
	void *messg, const size_t messglen) {
  register ssize_t result = -1;
  if (!socket) {
    Log(L_ASSERT, "Invalid `socket` Socket.");
  } else if (!messg && messglen) {
    Log(L_ASSERT, "Invalid `messg` buffer.");
  } else {
    /* Read from network stream */
    result = read(socket->handle, messg, messglen);

    /* Update statistics */
    if (result > 0)
      socket->bytesReceived += result;

#ifdef EINTR
    if (result < 0 && errno == EINTR)
      result = 0;
#endif /* EINTR */

#ifdef EAGAIN /* POSIX */
    if (result < 0 && errno == EAGAIN)
      result = 0;
#endif /* EAGAIN */

#ifdef EWOULDBLOCK /* BSD */
    if (result < 0 && errno == EWOULDBLOCK)
      result = 0;
#endif /* EWOULDBLOCK */

#ifdef EDEADLK /* Macintosh */
    if (result < 0 && errno == EDEADLK)
      result = 0;
#endif /* EDEADLK */

#ifdef ECONNRESET
    if (result < 0 && errno == ECONNRESET)
      result = -1;
#endif /* ECONNRESET */
  }
  return (result);
}

/*!
 * Initializes the OS socket library.
 * \addtogroup socket
 * \sa SocketCleanup()
 */
void SocketStartup(void) {
#ifdef _WIN32
  WORD wVersionRequested = MAKEWORD(2, 2);
  WSADATA wsaData;

  const int error = WSAStartup(wVersionRequested, &wsaData);
  if (error) {
    Log(L_SYSTEM, "WSAStartup() failed: error=0x%x.", error);
    abort(); /* Required operation */
  }
#endif /* _WIN32 */
}

/*!
 * Writes to a socket.
 * \addtogroup socket
 * \param socket the socket to which to write
 * \param messg the message buffer
 * \param messglen the length of the specified buffer
 * \return the number of bytes written to the network stream, or -1
 * \sa SocketRead(Socket*, const void*, const size_t)
 */
ssize_t SocketWrite(
	Socket *socket,
	const void *messg, const size_t messglen) {
  register ssize_t result = -1;
  if (!socket) {
    Log(L_ASSERT, "Invalid `socket` Socket.");
  } else if (!messg && messglen) {
    Log(L_ASSERT, "Invalid `messg` buffer.");
  } else {
    /* Write to network stream */
    result = write(socket->handle, messg, messglen);

    /* Update statistics */
    if (result > 0)
      socket->bytesSent += result;

    /*
     * result < 0, so an error was encountered - is it transient?
     * Unfortunately, different systems use different constants to
     * indicate this.
     */
#ifdef EAGAIN /* POSIX */
    if (result < 0 && errno == EAGAIN)
      result = 0;
#endif /* EAGAIN */

#ifdef EWOULDBLOCK /* BSD */
    if (result < 0 && errno == EWOULDBLOCK)
      result = 0;
#endif /* EWOULDBLOCK */

#ifdef EDEADLK /* Macintosh */
    if (result < 0 && errno == EDEADLK)
      result = 0;
#endif /* EDEADLK */
  }
  return (result);
}
