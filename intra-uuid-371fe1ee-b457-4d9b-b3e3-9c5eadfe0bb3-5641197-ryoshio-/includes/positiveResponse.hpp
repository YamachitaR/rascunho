/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   positiveResponse.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/21 20:51:33 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/23 21:07:10 by esilva-s         ###   ########.fr       */
/*                                                                             */
/* ************************************************************************** */

#ifndef positiveResponse_HPP
#define positiveResponse_HPP

#include "./Libs.hpp"

class Request;

responseData    processResponse(Request &request);
bool            verifyServerName(Request &request);
responseData    verifyRedirection(Request &request);//Não esta correto
int             resolveOption(std::string method);
bool            methodAllowed(Request &request);

responseData    getHandler(Request &request);
responseData    postHandler(Request &request);
responseData    deleteHandler(Request &request);
responseData    handleMethodDelete(Request &request);
responseData    handleMethodPost(Request &request);

responseData autoIndex(std::string root, std::string path, std::string port, Request request);

#endif