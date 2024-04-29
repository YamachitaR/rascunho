/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 16:02:53 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/27 19:22:46 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Socket_HPP
#define Socket_HPP

#include "./Libs.hpp"

#define IP "127.0.0.1"


class Socket {

    private:
    int              _fd;
    std::string      _port;
    std::string      _ip;
    struct addrinfo *_serverInfo;//struct de terceiros

    public:
    Socket(std::string port = "8080", std::string ip = IP);
    ~Socket(void);

    void    init(void);
    void    bindAddr(void);
    void    listenConnections(void);
    void    *get_in_addr(struct sockaddr *sa);
    int     acceptClient(int socketFd);
    void    finish(void);

    int     getFd(void);
};

#endif
