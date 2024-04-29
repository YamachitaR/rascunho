/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 15:35:54 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/26 20:30:27 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef cgi_HPP
#define cgi_HPP

#include "./Libs.hpp"

class Request;

cgi_infos initCgiInfos(void);
cgi_infos isCGI(Request &request);
cgi_infos programExists(std::string &path, Request &request);
std::vector<std::string> assembleCGIEnv(map_ss map_envs);
map_ss      assembleCGIHeaders(Request &request, cgi_infos infos);
std::string getBin(const std::string &url);
std::string executeCGI(Request &request, cgi_infos infos);
std::string queryToString(std::vector<std::string> query);

#endif