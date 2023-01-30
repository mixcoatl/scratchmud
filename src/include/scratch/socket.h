/*!
 * \file socket.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup socket
 */
#ifndef _SCRATCH_SOCKET_H_
#define _SCRATCH_SOCKET_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct Socket Socket;

/*!
 * The socket structure.
 * \addtogroup socket
 * \{
 */
struct Socket {
  SOCKADDR              address;        /*!< The endpoint address */
  size_t                bytesReceived;  /*!< The number of bytes received */
  size_t                bytesSent;      /*!< The number of bytes sent */
  SOCKET                handle;         /*!< The OS socket handle */
};
/*! \} */

/*!
 * Accepts a connection.
 * \addtogroup socket
 * \param socket the socket upon which to accept a connection
 * \return a new socket or NULL
 * \sa SocketClose(Socket*)
 */
Socket *SocketAccept(Socket *socket);

/*!
 * Constructs a new socket.
 * \addtogroup socket
 * \return the new socket or NULL
 * \sa SocketFree(Socket*)
 * \sa SocketFreeV(void*)
 */
Socket *SocketAlloc(void);

/*!
 * Returns whether a socket is a member of a file descriptor set.
 * \addtogroup socket
 * \param socket the socket whose membership to test
 * \param fdset the file descriptor set to test for the specified socket
 * \return true if the specified file descriptor set contains the specified socket
 */
bool SocketCheck(
	Socket *socket,
	const fd_set *fdset);

/*!
 * Terminates the OS socket library.
 * \addtogroup socket
 * \sa SocketStartup()
 */
void SocketCleanup(void);

/*!
 * Closes a socket.
 * \addtogroup socket
 * \param socket the socket to close
 * \sa SocketClosed(const Socket*)
 */
void SocketClose(Socket *socket);

/*!
 * Returns whether a socket is closed.
 * \addtogroup socket
 * \param socket the socket whose closed state to return
 * \return true if the specified socket is closed
 * \sa SocketClose(Socket*)
 */
bool SocketClosed(const Socket *socket);

/*!
 * Frees a socket.
 * \addtogroup socket
 * \param socket the socket to free
 * \sa SocketAlloc()
 * \sa SocketFreeV(void*)
 */
void SocketFree(Socket *socket);

/*!
 * Frees a socket.
 * \addtogroup socket
 * \param socket the socket to free
 * \sa SocketAlloc()
 * \sa SocketFree(Socket*)
 */
void SocketFreeV(void *socket);

/*!
 * Configures non-blocking mode.
 * \addtogroup socket
 * \param socket the socket whose non-blocking mode to configure
 */
void SocketNonBlocking(Socket *socket);

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
	const uint16_t port);

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
	void *messg, const size_t messglen);

/*!
 * Initializes the OS socket library.
 * \addtogroup socket
 * \sa SocketCleanup()
 */
void SocketStartup(void);

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
	const void *messg, const size_t messglen);

#endif /* _SCRATCH_SOCKET_H_ */
