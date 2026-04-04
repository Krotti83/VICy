// SPDX-License-Identifier: ISC
/**
 *
 * VICy - Open Cognitive Assist System
 *
 * This file is part of VICy's generic network libraries.
 *
 * Copyright (c) 2026 Johannes Krottmayer <krotti83@proton.me>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/**
 * VIVy Socket (network) classes
 */

#ifndef _VICY_LIBGEN_NETWORK_SOCKET_HPP
#define _VICY_LIBGEN_NETWORK_SOCKET_HPP

#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>

#include <netinet/in.h>
#include <sys/un.h>

namespace VICy::LibGen::Network {
    class SocketBuffer {
    public:
        SocketBuffer();
        ~SocketBuffer();

        long int Write(void *buf , size_t len);
        long int Read(void *buf , size_t len);

    protected:
        std::queue<unsigned char> *m_txBuffer;
        std::queue<unsigned char> *m_rxBuffer;
        std::mutex *m_txMutex;
        std::mutex *m_rxMutex;

    private:
        void destroy();
    };

    class SocketWorker : public SocketBuffer {
    public:
        SocketWorker();
        SocketWorker(int fd);
        ~SocketWorker();

        void Stop();

        void RXWorker();
        void TXWorker();

    private:
        std::atomic<bool> m_txThreadRun;
        std::atomic<bool> m_rxThreadRun;
        int m_fd;
        bool m_fdValid;
        bool m_error;
        std::string m_errorMessage;
    };

    class Socket {
    public:
        enum class SocketTypeEnum {
            Invalid,
            IPv4TCPClient,
            IPv4TCPServer,
            IPv4UDPClient,
            IPv4UDPServer,
            IPv6TCPClient,
            IPv6TCPServer,
            IPv6UDPClient,
            IPv6UDPServer,
            UNIXClient,
            UNIXServer
        };

        enum class SocketListenAddressEnum {
            Invalid,
            Loopback,
            Any,
            Broadcast,
            Custom
        };

        Socket();
        Socket(const Socket&);
        Socket(enum SocketTypeEnum type);
        ~Socket();

        Socket& operator=(const Socket& org);

        std::string GetAddress();
        unsigned short GetPort();

        void SetAddress(std::string addr);
        void SetPort(unsigned short port);

        long int Send(void *buf, size_t len);
        long int Receive(void *buf, size_t len);

    protected:
        bool Run();
        void Stop();
        bool Create();
        void Close();
        bool Connect(std::string server_addr, unsigned int server_port);
        bool Listen(enum SocketListenAddressEnum addr_type, unsigned int port, int num_clients);
        bool Accept(Socket& client);

    private:
        int m_fd;
        enum SocketTypeEnum m_type;
        struct sockaddr_in m_sockAddrInet4;
        struct sockaddr_in6 m_sockAddrInet6;
        struct sockaddr_un m_sockAddrUnix;
        std::string m_addr;
        unsigned short m_port;
        SocketWorker *m_ThreadWorker;
        bool m_error;
        bool m_opened;
        bool m_running;
        std::string m_errorMessage;
        std::thread *m_txThread;
        std::thread *m_rxThread;
    };
}

#endif
