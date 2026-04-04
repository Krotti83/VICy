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

#include <cstring>
#include <cerrno>
#include <chrono>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <Network/Socket.hpp>

using namespace VICy::LibGen::Network;

SocketBuffer::SocketBuffer()
{
    m_txMutex = new std::mutex;
    m_rxMutex = new std::mutex;
    m_txBuffer = new std::queue<unsigned char>;
    m_rxBuffer = new std::queue<unsigned char>;
}

SocketBuffer::~SocketBuffer()
{
    destroy();
}

void SocketBuffer::destroy()
{
    if (m_txMutex) {
        delete m_txMutex;
    }

    if (m_rxMutex) {
        delete m_rxMutex;
    }

    if (m_txBuffer) {
        delete m_txBuffer;
    }

    if (m_rxBuffer) {
        delete m_rxBuffer;
    }
}

long int SocketBuffer::Write(void *buf , size_t len)
{
    bool ret;
    unsigned char *ptr;

    ptr = static_cast<unsigned char *>(buf);

    if (ptr == nullptr) {
        return -1;
    }

    if (len > 0) {
        ret = m_txMutex->try_lock();

        while (ret == false) {
            ret = m_txMutex->try_lock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        for (size_t i = 0; i < len; i++) {
            m_txBuffer->push(static_cast<unsigned char>(*ptr));
            ptr++;
        }

        m_txMutex->unlock();
    } else {
        return 0;
    }

    return static_cast<long int>(len);
}

long int SocketBuffer::Read(void *buf , size_t len)
{
    bool ret;
    long int ret_recv = 0;

    unsigned char *ptr;

    ptr = static_cast<unsigned char *>(buf);

    if (ptr == nullptr) {
        return -1;
    }

    if (len > 0) {
        ret = m_rxMutex->try_lock();

        while (ret == false) {
            ret = m_rxMutex->try_lock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        for (size_t i = 0; i < len; i++) {
            if (m_rxBuffer->empty()) {
                break;
            }

            *ptr = m_rxBuffer->front();
            m_rxBuffer->pop();
            ptr++;
            ret_recv++;
        }

        m_rxMutex->unlock();
    } else {
        return 0;
    }

    return static_cast<long int>(ret_recv);
}

void SocketWorker::TXWorker()
{
    bool ret;
    unsigned char buf[128];
    size_t size;
    int ret_send;

    if (!m_txMutex) {
        return;
    }

    while (m_txThreadRun.load()) {
        ret = m_txMutex->try_lock();

        while (ret == false) {
            ret = m_txMutex->try_lock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (m_txBuffer->empty()) {
            m_txMutex->unlock();
            return;
        }

        size = m_txBuffer->size();

        if (size > 128) {
            while (size) {
                for (size_t i = 0; i < 128; i++) {
                    buf[i] = m_txBuffer->front();
                    m_txBuffer->pop();
                }

                size -= 128;
            }
        } else {
            for (size_t i = 0; i < size; i++) {
                buf[i] = m_txBuffer->front();
                m_txBuffer->pop();
            }

            ret_send = send(m_fd, buf, size, 0);

            if (ret_send < 0) {
                m_txMutex->unlock();
                m_error = true;
                m_errorMessage = strerror(errno);
                return;
            }
        }

        m_txMutex->unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void SocketWorker::RXWorker()
{
    bool ret;
    unsigned char buf[128];
    int ret_read;

    if (!m_rxMutex) {
        return;
    }

    while (m_rxThreadRun.load()) {
        ret = m_rxMutex->try_lock();

        while (ret == false) {
            ret = m_rxMutex->try_lock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        ret_read = recv(m_fd, buf, 128, MSG_DONTWAIT);

        if (ret_read < 0) {
            m_rxMutex->unlock();
            m_error = true;
            m_errorMessage = strerror(errno);
            return;
        }

        for (int i = 0; i < ret_read; i++) {
            m_rxBuffer->push(buf[i]);
        }

        m_rxMutex->unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

Socket::Socket()
{
    m_type = SocketTypeEnum::Invalid;
    m_txThread = nullptr;
    m_rxThread = nullptr;
}

Socket::Socket(enum SocketTypeEnum type)
{
    m_type = type;
    m_txThread = nullptr;
    m_rxThread = nullptr;
}

Socket::~Socket()
{
    Stop();
    Close();
}

std::string Socket::GetAddress()
{
    return m_addr;
}

unsigned short Socket::GetPort()
{
    return m_port;
}

void Socket::SetAddress(std::string addr)
{
    m_addr = addr;
}

void Socket::SetPort(unsigned short port)
{
    m_port = port;
}

bool Socket::Create()
{
    switch (m_type) {
        case SocketTypeEnum::IPv4TCPClient:
            m_fd = socket(AF_INET, SOCK_STREAM, 0);
            m_sockAddrInet4.sin_family = AF_INET;
            m_sockAddrInet4.sin_port = htons(m_port);

            break;
        case SocketTypeEnum::IPv4TCPServer:
            m_fd = socket(AF_INET, SOCK_STREAM, 0);
            break;
        case SocketTypeEnum::IPv4UDPClient:
            m_fd = socket(AF_INET, SOCK_DGRAM, 0);
            break;
        case SocketTypeEnum::IPv4UDPServer:
            m_fd = socket(AF_INET, SOCK_DGRAM, 0);
            break;
        case SocketTypeEnum::IPv6TCPClient:
            m_fd = socket(AF_INET, SOCK_STREAM, 0);
            break;
        case SocketTypeEnum::IPv6TCPServer:
            m_fd = socket(AF_INET, SOCK_STREAM, 0);
            break;
        case SocketTypeEnum::IPv6UDPClient:
            m_fd = socket(AF_INET, SOCK_DGRAM, 0);
            break;
        case SocketTypeEnum::IPv6UDPServer:
            m_fd = socket(AF_INET, SOCK_DGRAM, 0);
            break;
        default:
            return false;
    }

    if (m_fd < 0) {
        m_error = true;
        m_errorMessage = strerror(errno);
        return false;
    }

    return true;
}

long int Socket::Send(void *buf, size_t len)
{
    if (!m_ThreadWorker) {
        return -1;
    }

    return m_ThreadWorker->Write(buf, len);
}

long int Socket::Receive(void *buf, size_t len)
{
    if (!m_ThreadWorker) {
        return -1;
    }

    return m_ThreadWorker->Read(buf, len);
}

bool Socket::Connect(std::string server_addr, unsigned int server_port)
{
    int ret;

    if (m_error) {
        return false;
    }

    switch (m_type) {
        case SocketTypeEnum::IPv4TCPClient:
            m_sockAddrInet4.sin_family = AF_INET;
            m_sockAddrInet4.sin_port = htons(server_port);
            ret = inet_pton(AF_INET, server_addr.c_str(), &m_sockAddrInet4.sin_addr);

            if (ret < 0) {
                m_error = true;
                m_errorMessage = strerror(errno);
                return false;
            }

            ret = connect(m_fd,
                          reinterpret_cast<struct sockaddr *>(&m_sockAddrInet4),
                          sizeof(m_sockAddrInet4));

            break;
        case SocketTypeEnum::IPv6TCPClient:
            m_sockAddrInet6.sin6_family = AF_INET6;
            m_sockAddrInet6.sin6_port = htons(server_port);
            ret = inet_pton(AF_INET6, server_addr.c_str(), &m_sockAddrInet6.sin6_addr);

            if (ret < 0) {
                m_error = true;
                m_errorMessage = strerror(errno);
                return false;
            }

            ret = connect(m_fd,
                          reinterpret_cast<struct sockaddr *>(&m_sockAddrInet6),
                          sizeof(m_sockAddrInet6));

            break;
        default:
            return false;
    }

    if (ret < 0) {
        m_error = true;
        m_errorMessage = strerror(errno);
        return false;
    }

    return true;
}

bool Socket::Run()
{
    m_ThreadWorker = new SocketWorker(m_fd);

    if (!m_ThreadWorker) {
        return false;
    }

    m_txThread = new std::thread(&SocketWorker::TXWorker, m_ThreadWorker);

    if (!m_txThread) {
        return false;
    }

    m_rxThread = new std::thread(&SocketWorker::RXWorker, m_ThreadWorker);

    if (!m_rxThread) {
        return false;
    }

    m_running = true;
    return true;
}

void Socket::Stop()
{
    m_ThreadWorker->Stop();
    m_txThread->join();
    m_rxThread->join();
    m_running = false;
}

void Socket::Close()
{
    if (m_opened) {
        close(m_fd);
    }
}

bool Socket::Listen(enum SocketListenAddressEnum addr_type, unsigned int port, int num_clients)
{
    int ret;
    int opt = 1;

    if (m_error) {
        return false;
    }

    if (num_clients < 1) {
        return false;
    }

    ret = setsockopt(m_fd,
                     SOL_SOCKET,
                     SO_REUSEADDR | SO_REUSEPORT,
                     &opt,
                     sizeof(opt));

    if (ret != 0) {
        m_error = true;
        m_errorMessage = strerror(errno);
        return false;
    }

    switch (m_type) {
        case SocketTypeEnum::IPv4TCPServer:
        {
            m_sockAddrInet4.sin_family = AF_INET;
            m_sockAddrInet4.sin_port = htons(port);

            switch (addr_type) {
                case SocketListenAddressEnum::Loopback:
                    m_sockAddrInet4.sin_addr.s_addr = INADDR_LOOPBACK;
                    break;
                case SocketListenAddressEnum::Any:
                    m_sockAddrInet4.sin_addr.s_addr = INADDR_ANY;
                    break;
                case SocketListenAddressEnum::Broadcast:
                    m_sockAddrInet4.sin_addr.s_addr = INADDR_BROADCAST;
                    break;
                case SocketListenAddressEnum::Custom:
                    ret = inet_pton(AF_INET, m_addr.c_str(), &m_sockAddrInet4.sin_addr.s_addr);


                    if (ret != 0) {
                        m_error = true;
                        m_errorMessage = strerror(errno);
                        return false;
                    }

                    break;
                default:
                    return false;
            }

            ret = bind(m_fd,
                       reinterpret_cast<struct sockaddr *>(&m_sockAddrInet4),
                       sizeof(m_sockAddrInet4));

            if (ret != 0) {
                m_error = true;
                m_errorMessage = strerror(errno);
                return false;
            }
        }

            break;
        case SocketTypeEnum::IPv6TCPServer:
        {
            m_sockAddrInet6.sin6_family = AF_INET6;
            m_sockAddrInet6.sin6_port = htons(port);

            switch (addr_type) {
                case SocketListenAddressEnum::Loopback:
                    m_sockAddrInet6.sin6_addr = in6addr_loopback;
                    break;
                case SocketListenAddressEnum::Any:
                    m_sockAddrInet6.sin6_addr = in6addr_any;
                    break;
                case SocketListenAddressEnum::Custom:
                    ret = inet_pton(AF_INET6, m_addr.c_str(), &m_sockAddrInet6.sin6_addr);


                    if (ret != 0) {
                        m_error = true;
                        m_errorMessage = strerror(errno);
                        return false;
                    }

                    break;
                default:
                    return false;
            }

            ret = bind(m_fd,
                       reinterpret_cast<struct sockaddr *>(&m_sockAddrInet6),
                       sizeof(m_sockAddrInet6));

            if (ret != 0) {
                m_error = true;
                m_errorMessage = strerror(errno);
                return false;
            }
        }

            break;
        default:
            return false;
    }

    if (ret != 0) {
        m_error = true;
        m_errorMessage = strerror(errno);
        return false;
    }

    ret = listen(m_fd, num_clients);

    if (ret != 0) {
        m_error = true;
        m_errorMessage = strerror(errno);
        return false;
    }

    return true;
}

bool Socket::Accept(Socket& client)
{
    int ret;

    if (m_error) {
        return false;
    }

    return true;
}
