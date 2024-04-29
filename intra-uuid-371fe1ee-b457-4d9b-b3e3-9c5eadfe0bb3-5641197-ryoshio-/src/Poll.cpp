/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poll.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 21:29:35 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/26 22:23:52 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Libs.hpp"

Poll::Poll(void) {}

Poll::~Poll(void)
{
   // this->closePoll();
}

void Poll::init(int fd)
{
    struct pollfd pollFd;

    pollFd.fd      = fd;
    pollFd.events  = POLLIN;
    pollFd.revents = 0;
    this->_sockets.push_back(fd);
    this->_pollFds.push_back(pollFd);
}

int Poll::execute(void){ 
    int result;
    
    result = poll(&this->_pollFds[0], this->_pollFds.size(), 0);
    return result; 
}

bool    Poll::isRead(size_t i) const{ 
    bool    result;
        
    result = this->_pollFds[i].revents & POLLIN;
    if(result)
        return (true);
    else
        return (false);
}

bool Poll::isSocketServer(size_t i)
{
    bool result;

    result =  i < this->_sockets.size() && this->_pollFds[i].fd == this->_sockets[i];
    
    return (result);
}

void Poll::addPoll(int socketFd, short events)
{
    struct pollfd clientPoolFd;

    clientPoolFd.fd      = socketFd;
    clientPoolFd.events  = events;
    clientPoolFd.revents = 0;
    this->_pollFds.push_back(clientPoolFd);
}

int Poll::getPollFd(size_t i) const
{
    if (i < this->_pollFds.size())
    {
        return this->_pollFds[i].fd;
    }
    return (-1);
}

size_t Poll::getSize(void) const{ 
    size_t result;

    result = this->_pollFds.size();
    return (result); 
}

void Poll::addFdToClose(int fd)
{
    this->_fdToClose.push_back(fd);
}

void Poll::removeMarkedElements(void)
{
    int fd;
    std::vector<int>::iterator it;

    it = this->_fdToClose.begin();

    while (it != this->_fdToClose.end())
    {
        fd = *it;

        removeListeningSocket(fd);
        removePollFd(fd);
        close(fd);//fecha o socket
        Logs::printLog(Logs::INFO, 3, "Client connection closed:" + to_string(fd));
        it++;
    }
    this->_fdToClose.clear();
}

void Poll::removeListeningSocket(int fd)
{
    // Encontre e remova o socketFd do vetor _listeningSockets
    std::vector<int>::iterator it
        = std::find(_sockets.begin(), _sockets.end(), fd);

    if (it != _sockets.end()) {
        _sockets.erase(it);
    }
}

void Poll::removePollFd(int fd)
{
    std::vector<struct pollfd>::iterator it = _pollFds.begin();

    while (it != _pollFds.end()) {
        if (it->fd == fd) {
            it = _pollFds.erase(it);
        }
        else {
            ++it;
        }
    }
}