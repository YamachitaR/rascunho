/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clientProcess.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 00:10:21 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/25 19:22:01 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef clientProcess_HPP
#define clientProcess_HPP

#include "./Libs.hpp"

//responseData setResponseData(int status, std::string contentType, std::string content,
//                             int contentLength, std::string location);

void        processClientData(int fd);
std::string getStatusCodes(std::string status);
std::string readClientData(int fd);
std::string mergeStrVector(std::vector<std::string> vec, std::string delimiter);
void        sendResponse(int fd, responseData res);

#endif
