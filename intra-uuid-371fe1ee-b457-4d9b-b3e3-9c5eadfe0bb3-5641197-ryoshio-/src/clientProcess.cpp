/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clientProcess.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 00:09:57 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/28 19:52:52 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "clientProcess.hpp"

void processClientData(int fd)
{
    std::string     clientReq;
    responseData    res;
    Request         reqClient;

    res = setResponseData(0, "", "", 0, "");
    clientReq = readClientData(fd);

    if ((webServer.getBytesRead() == -1)) {
        Logs::printLog(Logs::ERROR, 3, "Client closed: " + to_string(fd) + " - Read is not possible");
        webServer.addFdToClose(fd);
        return;
    }
    if ((webServer.getBytesRead() == -2)) {
        Logs::printLog(Logs::ERROR, 3, "Client closed: " + to_string(fd) + " - Body of request too large");
        res = getErrorPageStandard(413);
    }
    else if (!reqClient.requestStart(clientReq))
        res = getErrorPageStandard(reqClient.statusCode);
    else
        res = processResponse(reqClient);
        
    sendResponse(fd, res);
}

void  sendResponse(int fd, responseData res)
{
    std::string response_header = "";
    std::string content_final = "";
    int bytes_sent;

    response_header =  "HTTP/1.1 " + res.statusCode + "\r\n";
    if (res.contentLength < 0)
        return;

    if (res.status == PERMANENT_REDIRECT || res.status == TEMPORARY_REDIRECT)
        response_header += "location: " + res.location + "\r\n\r\n";
    else if (res.contentType.empty())
        response_header += "\r\n";
    else if (res.status == METHOD_NOT_ALLOWED)
    {
        response_header += "Allow: GET\r\nContent-Type: " + res.contentType;
        response_header += "\r\nContent-Length: " + to_string(res.contentLength) + "\r\n\r\n";
    }
    else
    {
        response_header += "Content-Type: " + res.contentType;
        response_header += "\r\nContent-Length: " + to_string(res.contentLength) + "\r\n\r\n";
    }

    content_final = response_header;
    if (res.contentLength)
        content_final += res.content;

    bytes_sent = send(fd, content_final.c_str(), strlen(response_header.c_str()) + res.contentLength, MSG_NOSIGNAL);
    if ((bytes_sent == -1) || (bytes_sent == -1))
        Logs::printLog(Logs::ERROR, 3, "Client connection closed: " + to_string(fd) + " - Write is not possible");
    
    webServer.addFdToClose(fd);
}

size_t getFirstLineSize(const std::string& request) {
    size_t crlfPos = request.find("\r\n\r\n");
    
    if (crlfPos != std::string::npos) {
        return crlfPos;
    } else {
        return request.size();
    }
}

std::string findHeaders(std::string request)
{
    std::string port = "";

    std::string::size_type hostPos = request.find("\r\n");
    if (hostPos != std::string::npos) {
        std::string::size_type second = request.find("\r\n");
        if (second != std::string::npos){
        
            std::string::size_type colonPos = request.find(":", hostPos);
            colonPos = request.find(":",colonPos+1);
            if (colonPos != std::string::npos) {
                std::string portSubstring = request.substr(colonPos + 1);
                std::string::size_type spacePos = portSubstring.find_first_of(" \r");
                if (spacePos != std::string::npos) {
                    port = portSubstring.substr(0, spacePos);
                }
           
            }
        }
    }
   
    return (port);
}

std::string readClientData(int fd)
{
    char        buffer[1024] = {0};
    std::string clientReq;
    int bytesRead;
    int totalRead = 0;
    int maxBodylimit = -1;
    std::string temp = "0";

    while ((bytesRead = recv(fd, buffer, sizeof(buffer), 0)) > 0) {
        webServer.setBytesRead(bytesRead);
        if (bytesRead < 1)
            break;
        totalRead += bytesRead;
        clientReq.append(buffer, bytesRead);
        
        if (clientReq.find("\r\n\r\n") != std::string::npos)
        {
           
            std::string port = findHeaders(clientReq);
            temp = webServer.getServerValue(webServer.searchServer(port), "client_max_body_size")[0];
            maxBodylimit = std::atoi(temp.c_str());
            size_t contentTypePos1 = clientReq.find("Content-Length: ") + 16;
            size_t lineEnd1 = clientReq.find("\r\n", contentTypePos1 );
            std::string contentTypeLine1 = clientReq.substr(contentTypePos1, lineEnd1 - contentTypePos1);

            if (maxBodylimit <  std::atoi(contentTypeLine1.c_str()))
            {
                webServer.setBytesRead(-2);
                break;
            }
               
        }
        if (clientReq.find("Expect: 100-continue") != std::string::npos) {
            sleep(2);
            continue;
        }
        buffer[bytesRead] = '\0';
        if (clientReq.find("multipart/form-data") != std::string::npos) {
            std::string boundary;
            size_t      contentTypePos = clientReq.find("Content-Type: ");

            if (contentTypePos != std::string::npos) {
                size_t lineEnd = clientReq.find("\r\n", contentTypePos);
                if (lineEnd != std::string::npos) {
                    std::string contentTypeLine
                        = clientReq.substr(contentTypePos, lineEnd - contentTypePos);
                    size_t boundaryPos = contentTypeLine.find("boundary=");
                    if (boundaryPos != std::string::npos) {
                        boundary = contentTypeLine.substr(boundaryPos + 9);
                    }
                }
            }
            std::string boundaryEnd = "\r\n--" + boundary + "--";
            if (clientReq.find(boundaryEnd) != std::string::npos)
                break;
        } else if (clientReq.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    }
    return (clientReq);
}

std::string mergeStrVector(std::vector<std::string> vec, std::string delimiter)
{
    std::string                        result = "";
    std::vector<std::string>::iterator it = vec.begin();

    while (it != vec.end())
    {
        result += *it;
        result += delimiter;
        it++;
    }
    return (result);
}
/*
responseData setResponseData(int status, std::string contentType, std::string content,
                             int contentLength, std::string location) 
{
    responseData response;

    std::string msg;
    std::string str_status = to_string(status);

    msg = webServer.getStatusCode(str_status);
    if (msg != "")
        str_status += " " + msg;

    response.status        = status;
    response.statusCode    = str_status;
    response.contentType   = contentType;
    response.content       = content;
    response.contentLength = contentLength;
    response.location      = location;
    return (response);
}
*/
