/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   negativeResponse.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 20:39:03 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/22 23:17:11 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NEGATIVERESPONSE_HPP
#define NEGATIVERESPONSE_HPP

#include "./Libs.hpp"

responseData    getErrorPageStandard(int statusCode);

//testar
responseData    getErrorPageContent(std::vector<std::string> errorPage,
                                    int statusCode, std::string uri, std::string root);
//testar
bool            hasErrorPageConfig(std::vector<std::string> errorPage, int statusCode);
//testar
std::string     getPath(std::string uri);

responseData    getContent(std::string root, std::string file, int status);
std::string     extractFileExtension(std::string file);
responseData    setResponseData(int         status,
                                 std::string contentType,
                                 std::string content,
                                 int         contentLength,
                                 std::string location);

#endif