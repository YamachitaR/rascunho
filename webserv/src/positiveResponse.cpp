/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   positiveResponse.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/21 20:48:20 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/28 12:58:12 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "positiveResponse.hpp"

responseData processResponse(Request &request)
{
    responseData res;
    responseData temp;

    res = setResponseData(0, "", "", -1, "");

    if (!verifyServerName(request))
    {
        res = getErrorPageContent(request.getErrorPageConfig(), BAD_REQUEST, request.getUri(), request.getRoot());
        return (res);
    }

    temp = verifyRedirection(request);
    
    if (temp.status != 999)
        return (temp);

    if (!methodAllowed(request))
    {
        res = getErrorPageContent(request.getErrorPageConfig(), METHOD_NOT_ALLOWED, request.getUri(), request.getRoot());
        return (res);
    }

    int opt = resolveOption(request.getMethod());

    if (opt == 0)
        res = getHandler(request);
    else if (opt == 1)
        res = postHandler(request);
    else if (opt == 2)
        res = deleteHandler(request);

    return (res);
}

bool verifyServerName(Request &request)
{
    std::vector<std::string>            server_names;
    std::vector<std::string>::iterator  it;

    server_names = webServer.getServerValue(request.getServerIndex(), "server_name");
    if (server_names.empty())
        return (true);
    it = server_names.begin();
    while (it != server_names.end())
    {
        if (*it == request.getHost() || request.getHost() == "127.0.0.1")
            return (true);
        it++;
    }
    return (false);
}

responseData verifyRedirection(Request &request)
{
    std::vector<std::string>::iterator  it_s;
    std::vector<std::string>::iterator  it_p;
    std::vector<std::string>            server_redirection;
    std::vector<std::string>            location_redirection;
    responseData                        res;

    server_redirection = webServer.getServerValue(request.getServerIndex(), "redirect");
    it_s = server_redirection.begin();

    res = setResponseData(999, "", "", -1, "");

    if (!server_redirection.empty())
    {
        res = setResponseData(std::atoi(it_s[0].c_str()), "", "", 0, it_s[1]);
        return (res);
    }

    location_redirection = webServer.getLocationValue(request.getServerIndex(), request.getLocationIndex(), "redirect");
    it_p = location_redirection.begin();

    if (!location_redirection.empty())
    {
        res = setResponseData(std::atoi(it_p[0].c_str()), "", "", 0, it_p[1]);
        return (res);
    }
    return (res);
}

bool methodAllowed(Request &request)
{
    size_t i = 0;

    if (request.getLimitExcept().empty())
        return (true);

    while (i < request.getLimitExcept().size())
    {
        if (request.getLimitExcept()[i] == request.getMethod())
            return (true);
        i++;
    }
    return (false);
}

int resolveOption(std::string method)
{
    std::string option[] = {"GET", "POST", "DELETE"};
    int i        = 0;

    while (i < 3 && method != option[i])
        i++;
    return (i);
}


responseData getCgi(Request &request, cgi_infos infos)
{
    responseData res;

    std::string cgi_response = executeCGI(request, infos);
    if (cgi_response == "erro504")
        res = getErrorPageContent(request.getErrorPageConfig(), 504, request.getUri(), request.getRoot());
    else if (cgi_response == "erro500")
        res = getErrorPageContent(request.getErrorPageConfig(), 500, request.getUri(), request.getRoot());
    else
        res = setResponseData(OK, "text/html", cgi_response.c_str(), (int)cgi_response.length(), "");
    return (res);
}

std::string cutToBars(std::string entrada) {
    size_t pos = entrada.find_last_of("/");
    if (pos != std::string::npos) {
        return entrada.substr(0, pos + 1);
    } else {
        return entrada;
    }
}

responseData getHandler(Request &request)
{
    Location        location(request);
    responseData    res;
    cgi_infos       infos;
    std::string     path;
    std::string uriBar = cutToBars(request.getUri()) + "autoindex";
 
    res = setResponseData(0, "", "", 0, ""); 

    if (request.getAutoIndexServer() && (request.getUri() == "/autoindex"))
        res = autoIndex(request.getRoot(), "/", request.getPort(), request);
    else if (request.getAutoIndexLoc() && (request.getUri() == uriBar))
    {
        path = webServer.getLocationValue(request.getServerIndex(), request.getLocationIndex(), "location")[0];
        res = autoIndex(request.getRoot(), path, request.getPort(), request);
    }
    else if (extractFileExtension(request.getUri()) == ".py" && isCGI(request).correct)
    {
        infos = isCGI(request);
        res = getCgi(request, infos);
    }
    else
    {
        location.setup();
        res = location.getLocationContent();
    }
    return (res);
}

responseData postHandler(Request &request)
{
    PostMethod      post_method(request);
    responseData    res;
    cgi_infos       infos;

    res = setResponseData(0, "", "", 0, "");

    if (extractFileExtension(request.getUri()) == ".py" && isCGI(request).correct)
    {
        infos = isCGI(request);
        res = getCgi(request, infos);
    }
    else 
        res = post_method.handleMethod();
    return (res);
}

responseData autoIndex(std::string root, std::string path, std::string port, Request request)
{
    std::string     dirPath = root + path;
    std::string     entryPath;
    std::string     content;
    struct dirent   *entry;
    responseData    res;
    DIR             *dir;

    dir = opendir(dirPath.c_str());

    if (dir == NULL)
    {
        res = getErrorPageContent(request.getErrorPageConfig(), NOT_FOUND, request.getUri(), request.getRoot());
        return (res);
    }

    content = "<html><body><h2>Index of: " + dirPath + "</h2><ul>";
    
    entry = readdir(dir);
    while (entry)
    {
        entryPath = path;
        if (path[path.size() - 1] != '/')
            entryPath += "/";
        if (entry->d_type == DT_DIR)
            entryPath += std::string(entry->d_name) + "/autoindex";
        else
            entryPath += std::string(entry->d_name);
        content += "<li><a href=\"http://localhost:" + port + entryPath + "\">" + std::string(entry->d_name) + "</a></li>\n";
        entry = readdir(dir);
    }

    content += "</ul></body></html>";
    res = setResponseData(OK, "text/html", content, content.length(), path );

    closedir(dir);
    return (res);
}

responseData deleteHandler(Request &request)
{
    responseData    res;

    res = handleMethodDelete(request);
    Logs::printLog(Logs::WARNING, 30, "DELETE METHOD");
    return (res);
}

responseData handleMethodDelete(Request &request)
{
    std::string resourcePath;
     responseData res;
    
    resourcePath = getDir();

    resourcePath = resourcePath + "/" + request.getRoot() + request.getUri();

    Logs::printLog(Logs::WARNING, 30, "resourcePath: " + resourcePath);

    std::ifstream file(resourcePath.c_str());

    if (file.is_open()) {
        file.close();
        if (std::remove(resourcePath.c_str()) == 0) {
            res = setResponseData(NO_CONTENT, "", "", 0, "");
            Logs::printLog(Logs::INFO, 30, "Resource deleted!");
        } else {
            res =  getErrorPageContent(request.getErrorPageConfig(),
                                                              INTERNAL_SERVER_ERROR,
                                                              request.getUri(),
                                                              request.getRoot());
           
            Logs::printLog(Logs::INFO, 30, "Error delet resource!");
        }
    } else {
            res = getErrorPageContent(
                    request.getErrorPageConfig(), NOT_FOUND, request.getUri(), request.getRoot());
        Logs::printLog(Logs::WARNING, 30, "RESOURCE NOT FOUND!");
    }
    return (res);
}