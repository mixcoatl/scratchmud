/*!
 * \file socket.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
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
#include <scratch/string.h>

/*!
 * Accepts a connection.
 * \addtogroup socket
 * \param theSocket the control socket
 * \return a new socket or NULL
 */
Socket *SocketAccept(Socket *theSocket) {
  Socket *accepted = NULL;
  if (!theSocket) {
    Log("invalid `theSocket` Socket");
  } else if (theSocket->handle == INVALID_SOCKET) {
    Log("control socket already closed");
  } else {
    /* Peer address */
    SOCKADDR peer;
    socklen_t peerSZ = sizeof(peer);

    /* Accepted connection */
    MemoryCreate(accepted, Socket, 1);
    MemoryZero(&accepted->address, SOCKADDR, 1);
    accepted->bytesReceived = 0;
    accepted->bytesSent = 0;
    accepted->handle = accept(theSocket->handle, &peer, &peerSZ);

    if (accepted->handle == INVALID_SOCKET) {
      Log("accept() failed: errno=%d", errno);
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
 */
Socket *SocketAlloc(void) {
  Socket *theSocket;
  MemoryCreate(theSocket, Socket, 1);
  theSocket->handle = socket(AF_INET, SOCK_STREAM, 0);
  if (theSocket->handle == INVALID_SOCKET) {
    Log("socket() failed: errno=%d", errno);
    MemoryFree(theSocket);
  } else {
    MemoryZero(&theSocket->address, SOCKADDR, 1);
    theSocket->bytesReceived = 0;
    theSocket->bytesSent = 0;
  }
  return (theSocket);
}

/*!
 * Terminates OS socket library.
 * \addtogroup socket
 * \sa SocketStartup()
 */
void SocketCleanup(void) {
#ifdef _WIN32
  if (WSACleanup() != 0) {
    const int error = WSAGetLastError();
    Log("WSACleanup() failed: error=0x%x", error);
  }
#endif /* _WIN32 */
}

/*!
 * Closes a socket.
 * \addtogroup socket
 * \param theSocket the socket to close
 * \sa SocketFree(Socket*)
 * \sa SocketClosed(const Socket*)
 */
void SocketClose(Socket *theSocket) {
  if (!theSocket) {
    Log("invalid `theSocket` Socket");
  } else {
    if (theSocket->handle != INVALID_SOCKET) {
#ifdef _WIN32
      if (closesocket(theSocket->handle) != 0) {
        const int error = WSAGetLastError();
        Log("closesocket() failed: error=0x%x", error);
      }
#else
      if (close(theSocket->handle) < 0)
        Log("close() failed: errno=%d", errno);
#endif /* _WIN32 */
    }
    MemoryZero(&theSocket->address, SOCKADDR, 1);
    theSocket->bytesReceived = 0;
    theSocket->bytesSent = 0;
    theSocket->handle = INVALID_SOCKET;
  }
}

/*!
 * Returnss whether a socket is closed.
 * \addtogroup socket
 * \param theSocket the socket whose closed state to return
 * \return true if the specified socket is closed
 * \sa SocketClose(Socket*)
 */
bool SocketClosed(const Socket *theSocket) {
  register bool closed = true;
  if (!theSocket) {
    Log("invalid `theSocket` Socket");
  } else {
    closed = (theSocket->handle == INVALID_SOCKET);
  }
  return (closed);
}

/*!
 * Frees a socket.
 * \addtogroup socket
 * \param theSocket the socket to free
 * \sa SocketAlloc()
 */
void SocketFree(Socket *theSocket) {
  if (theSocket) {
    SocketClose(theSocket);
    MemoryFree(theSocket);
  }
}

#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif /* O_NONBLOCK */

/*!
 * Sets a socket into non-blocking mode.
 * \addtogroup socket
 * \param theSocket the socket whose non-blocking mode to set
 * \return true if the specified socket is successfully
 *     configured for non-blocking mode
 */
bool SocketNonBlocking(Socket *theSocket) {
  register bool result = false;
  if (!theSocket) {
    Log("invalid `theSocket` Socket");
  } else {
    /* Get the flags currently set on the socket. */
    int flags = fcntl(theSocket->handle, F_GETFL, 0);

    if (flags < 0) {
      Log("fcntl() failed: errno=%d", errno);
    } else {
      /* Set the flags for the socket. */
      if (fcntl(theSocket->handle, F_SETFL, flags | O_NONBLOCK) < 0) {
        Log("fcntl() failed: errno=%d", errno);
      } else {
        result = true;
      }
    }
  }
  return (result);
}

/*!
 * Binds a socket and listens.
 * \addtogroup socket
 * \param theSocket the socket to open
 * \param address the endpoint address or NULL
 * \param port the endpoint port
 * \return true if the specified socket was successfully opened
 */
bool SocketOpen(
	Socket *theSocket,
	const char *address,
	const uint16_t port) {
  bool result = false;
  if (theSocket == NULL) {
    Log("invalid `theSocket` Socket");
  } else {
    SOCKADDR_IN peer;
    socklen_t peerSZ = sizeof(peer);
    MemoryZero(&peer, SOCKADDR_IN, 1);
    peer.sin_family = AF_INET;
    peer.sin_port = htons(port);
    if (inet_pton(AF_INET, StringBlank(address), &peer.sin_addr) < 0) {
      Log("inet_pton() failed: %d", errno);
    } else if (bind(theSocket->handle, (SOCKADDR*) &peer, peerSZ) < 0) {
      Log("bind() failed: %d", errno);
    } else if (listen(theSocket->handle, 5) < 0) {
      Log("listen() failed: %d", errno);
    } else {
      /* Determine the local name */
      peerSZ = sizeof(theSocket->address);
      if (getsockname(theSocket->handle, &(theSocket->address), &peerSZ) < 0) {
        Log("getsockname() failed: %d", errno);
        MemoryZero(&theSocket->address,  SOCKADDR, 1);
      }
      result = true;
    }
  }
  return (result);
}

/*!
 * Reads from a socket.
 * \addtogroup socket
 * \param theSocket the socket from which to read
 * \param messg the message buffer
 * \param messglen the length of the specified buffer
 * \return the number of bytes actually read, or -1
 * \sa SocketWrite(Socket*, const void*, const size_t)
 */
ssize_t SocketRead(
	Socket *theSocket,
	void *messg, const size_t messglen) {
  register ssize_t result = -1;
  if (!socket) {
    Log("invalid `theSocket` Socket");
  } else if (!messg && messglen) {
    Log("invalid `messg` buffer");
  } else {
    /* Try to read from the socket */
    result = read(theSocket->handle, messg, messglen);

    /* Update statistics */
    if (result > 0)
      theSocket->bytesReceived += result;

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
 * Initializes OS socket library.
 * \addtogroup socket
 * \sa SocketCleanup()
 */
void SocketStartup(void) {
#ifdef _WIN32
  WORD wVersionRequested = MAKEWORD(2, 2);
  WSADATA wsaData;

  const int error = WSAStartup(wVersionRequested, &wsaData);
  if (error) {
    Log("WSAStartup() failed: error=0x%x", error);
    exit(EXIT_FAILURE);
  }
#endif /* _WIN32 */
}

/*!
 * Writes to a socket.
 * \addtogroup socket
 * \param theSocket the socket to which to write
 * \param messg the message buffer
 * \param messglen the length of the specified buffer
 * \return the number of bytes written, or -1
 * \sa SocketRead(Socket*, const void*, const size_t)
 */
ssize_t SocketWrite(
	Socket *theSocket,
	const void *messg, const size_t messglen) {
  register ssize_t result = -1;
  if (!theSocket) {
    Log("invalid `theSocket` Socket");
  } else if (!messg && messglen) {
    Log("invalid `messg` buffer");
  } else {
    /* Try to write to the socket */
    result = write(theSocket->handle, messg, messglen);

    /* Update statistics */
    if (result > 0)
      theSocket->bytesSent += result;

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
