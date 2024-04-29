/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 16:11:35 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/27 19:22:38 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket(std::string port, std::string ip) :
    _fd(-1), _port(port), _ip(ip), _serverInfo(NULL)
{
}

Socket::~Socket(void)
{
    this->finish();
}

void Socket::init(void)
{
    struct addrinfo hints;
    std::string     msg;
    int             active = 1;
    int result;

    memset(&hints, 0, sizeof hints);

    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;


    result = getaddrinfo(this->_ip.c_str(), this->_port.c_str(), &hints, &this->_serverInfo);

    if (result != 0) {
        msg = "Error getting address information: " + std::string(gai_strerror(result));
        std::cout << msg << std::endl;
        exit(1);
    }
    this->_fd = socket( this->_serverInfo->ai_family,
                        this->_serverInfo->ai_socktype,
                        this->_serverInfo->ai_protocol);

    if (this->_fd < 0) {
        msg = "Error creating server socket: " + std::string(strerror(errno));
        std::cout << msg << std::endl;
        exit(1);
    }

    setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &active, sizeof(int));
}

void Socket::bindAddr(void)
{
    std::string msg;
    int         result;

    result = bind(this->_fd, this->_serverInfo->ai_addr, this->_serverInfo->ai_addrlen);

    if (result < 0)
    {
        msg = "Error binding socket to port: " + std::string(strerror(errno));
        std::cout << msg << std::endl;
        exit(1);
    }
    freeaddrinfo(this->_serverInfo);
    this->_serverInfo = NULL;
}

void Socket::listenConnections(void)
{
    std::string msg;
    int         result;
    result = listen(this->_fd, SOMAXCONN);

    if (result < 0) {
        msg = "Error listening for connections: " + std::string(strerror(errno));
        Logs::printLog(Logs::ERROR, 1, msg);
        exit(1);
    }
    Logs::printLog(Logs::INFO, 1, "Listening at " + to_string(this->_ip) + ":" + to_string(this->_port));
}

void Socket::finish(void)
{
    if (this->_fd != -1) {
        close(this->_fd);
        this->_fd = -1;
    }
    if (this->_serverInfo != NULL) {
        freeaddrinfo(this->_serverInfo);
        this->_serverInfo = NULL;
    }
}

int Socket::acceptClient(int socketFd)
{
    struct sockaddr_storage clientAddr;
    std::string             msg;
    int                     newFdClient;
    char                    remoteIP[INET6_ADDRSTRLEN];
    std::stringstream       ss;

    socklen_t   addr_size = sizeof clientAddr;
    newFdClient = accept(socketFd, (struct sockaddr *)&clientAddr, &addr_size);
    if (newFdClient == -1) {
        msg = "Error accepting client connection: " + std::string(strerror(errno));
        Logs::printLog(Logs::ERROR, 1, msg);
    }
    ss << "New connection from client "
       << inet_ntop(clientAddr.ss_family,
                    get_in_addr((struct sockaddr *)&clientAddr),
                    remoteIP,
                    INET6_ADDRSTRLEN)
       << " on socket " << newFdClient;
    msg = ss.str();
    Logs::printLog(Logs::INFO, 1, msg);

    return (newFdClient);
}

void *Socket::get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int Socket::getFd(void) { return (this->_fd); }