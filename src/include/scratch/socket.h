/*!
 * \file socket.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup socket
 */
#ifndef _SCRATCH_SOCKET_H_
#define _SCRATCH_SOCKET_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct _Socket Socket;

/*!
 * The socket structure.
 * \addtogroup socket
 * \{
 */
struct _Socket {
  SOCKADDR      address;        /*!< The endpoint address */
  size_t        bytesReceived;  /*!< The number of bytes received */
  size_t        bytesSent;      /*!< The number of bytes sent */
  SOCKET        handle;         /*!< The OS socket handle */
};
/*! \} */

/*!
 * Accepts a connection.
 * \addtogroup socket
 * \param theSocket the control socket
 * \return a new socket or NULL
 */
Socket *SocketAccept(Socket *theSocket);

/*!
 * Constructs a new socket.
 * \addtogroup socket
 * \return a new socket or NULL
 */
Socket *SocketAlloc(void);

/*!
 * Terminates OS socket library.
 * \addtogroup socket
 * \sa SocketStartup()
 */
void SocketCleanup(void);

/*!
 * Closes a socket.
 * \addtogroup socket
 * \param theSocket the socket to close
 * \sa SocketFree(Socket*)
 * \sa SocketClosed(const Socket*)
 */
void SocketClose(Socket *theSocket);

/*!
 * Returnss whether a socket is closed.
 * \addtogroup socket
 * \param theSocket the socket whose closed state to return
 * \return true if the specified socket is closed
 * \sa SocketClose(Socket*)
 */
bool SocketClosed(const Socket *theSocket);

/*!
 * Frees a socket.
 * \addtogroup socket
 * \param theSocket the socket to free
 * \sa SocketAlloc()
 */
void SocketFree(Socket *theSocket);

/*!
 * Sets a socket into non-blocking mode.
 * \addtogroup socket
 * \param theSocket the socket whose non-blocking mode to set
 * \return true if the specified socket is successfully
 *     configured for non-blocking mode
 */
bool SocketNonBlocking(Socket *theSocket);

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
	const uint16_t port);

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
	void *messg, const size_t messglen);

/*!
 * Initializes OS socket library.
 * \addtogroup socket
 * \sa SocketCleanup()
 */
void SocketStartup(void);

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
	const void *messg, const size_t messglen);

#endif /* _SCRATCH_SOCKET_H_ */
