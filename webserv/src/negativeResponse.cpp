/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   negativeResponse.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 20:39:55 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/28 19:27:36 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "negativeResponse.hpp"

responseData getErrorPageStandard(int statusCode)
{
    responseData res;
    std::string file;

    file = "/";
    file += to_string(statusCode);
    file += ".html";
    res = getContent(DEFAULT_ERROR_ROOT, file, statusCode);
    
    if (res.contentLength)
        return (res);

    res = getContent(DEFAULT_ERROR_ROOT, "/not_configured.html", statusCode);
    return (res);
}

responseData getErrorPageContent(std::vector<std::string> errorPage, int statusCode, std::string uri, std::string root)
{
    std::string  filePath;
    responseData res;

    (void)uri;
    if (hasErrorPageConfig(errorPage, statusCode)) {
        filePath = '/' + errorPage.back();
        res = getContent(root, filePath, statusCode);
        if (res.contentLength)
            return (res);
    }
    res = getErrorPageStandard(statusCode);
    return (res);
}

bool hasErrorPageConfig(std::vector<std::string> errorPage, int statusCode)
{
    std::vector<std::string>::iterator  it;
    
    it = errorPage.begin();
    while (it != errorPage.end())
    {
        if (*it == to_string(statusCode))
            return (true);
        it++;
    }
    return (false);
}

std::string getPath(std::string uri)
{
    size_t firstSlashPos;
    size_t lastSlashPos;
    size_t dotPos;

    if (uri.length() == 1)
        return ("/");

    firstSlashPos = uri.find('/', 1);

    if (firstSlashPos == std::string::npos)
        return ("/");

    dotPos = uri.rfind('.');

    if (dotPos == std::string::npos)
        return (uri);

    lastSlashPos = uri.rfind('/');

    return (uri.substr(0, lastSlashPos));
}

responseData getContent(std::string root, std::string file, int status)
{
    std::stringstream fullPathStream;
    std::string       fullPath;
    responseData      data;
    std::string       extension;

    data      = setResponseData(0, "", "", 0 , "");
    extension = extractFileExtension(file);

    fullPathStream << root << file;
    fullPath = fullPathStream.str();
    std::ifstream ifs(fullPath.c_str());
    if (ifs.is_open())
    {
        std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        data = setResponseData(status, getTypes(extension, webServer.getDicTypes()), content, (int)content.length(), "");
        ifs.close();
    }
    return (data);
}

std::string extractFileExtension(std::string file)
{
    size_t dotPos = file.rfind('.');

    if (dotPos != std::string::npos) {
        std::string extension = file.substr(dotPos);
        return extension;
    }
    return "";
}

responseData setResponseData(int status, std::string contentType, std::string content,
                             int contentLength, std::string location)
{
    responseData res;

    res.status        = status;
    res.statusCode    = getStatus(to_string(status), webServer.getDicStatusCodes());
    res.contentType   = contentType;
    res.content       = content;
    res.contentLength = contentLength;
    res.location      = location;
    return (res);
}