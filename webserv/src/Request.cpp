/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 21:28:12 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/28 12:16:39 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Libs.hpp"

Request::Request() { _init(); }

Request::~Request() {}

void Request::_init()
{
    
    _allowMethods.push_back("GET");
    _allowMethods.push_back("POST");
    _allowMethods.push_back("DELETE");
    _paramQuery.clear();
    statusCode     = 0;
    _uri           = "";
    _port          = "";
    _method        = "";
    _boundary      = "";
    _httpVersion   = "";
    _maxBodySize   = 0;
    _contentLength = 0;
    _header.clear();


    this->_uri = "";
    this->_path = "";
    this->_body = "";
    this->_port = "";
    this->_host = "";
    this->_method = "";
    this->_boundary = "";
    this->_httpVersion = "";
    this->_root = "";
    this->_serverIndex = 999;
    this->_locationIndex = 999;
    this->_locationSize = 0;
    this->_maxBodySize = 0;
    this->_contentLength = 0;
    this->_has_body = false;
    this->_has_form = false;
    this->_has_multipart = false;
    this->_autoIndexServer = false;
    this->_autoIndexLoc = false;
    this->_content = "";
}

bool Request::requestStart(std::string request)
{
    std::map<std::string, std::string>::const_iterator it;
    std::string requestLine;
    std::string headersPart;
    size_t firstLineEnd;
    

    firstLineEnd = request.find("\r\n");
    if (firstLineEnd == std::string::npos) {
        this->statusCode = BAD_REQUEST;
        return (false);
    }

    requestLine = request.substr(0, firstLineEnd);
    headersPart = request.substr(firstLineEnd + 2);

    if (!_parseFirstLine(requestLine))
        return (false);

    _parseHeaders(headersPart);
    _getMaxBody();
    _getServerParam();
    _setAutoIndex();
    _getHost();//compreender e melhorar

    if (this->_has_body) {
        this->_has_multipart = false;
        this->_has_form      = false;
        
        it = _header.find("Content-Type");
        if (it->second.find("multipart/form-data") != std::string::npos)
            this->_has_multipart = true;
        if (it->second.find("application/x-www-form-urlencoded") != std::string::npos)
            this->_has_form = true;
    }
    //_setPath();
    if (this->_has_multipart) {
        if (_getMultipartData(request))//compreender e melhorar
            return (false);
    } else if (this->_has_body) {
        if (_getBody(request))//compreender e melhorar
            return (false);
    }
    return (true);
}

bool Request::_parseFirstLine(std::string &requestLine)
{
    std::istringstream iss(requestLine);

    if (!(iss >> this->_method >> this->_uri >> this->_httpVersion)
        || requestLine != this->_method + " " + this->_uri + " " + this->_httpVersion
        || this->_uri[0] != '/'){
        this->statusCode = BAD_REQUEST;
        return (false);
    }
    if (std::find(_allowMethods.begin(), _allowMethods.end(), _method) == _allowMethods.end()){
        this->statusCode = METHOD_NOT_ALLOWED;
        return (false);
    }
    if (this->_httpVersion != "HTTP/1.1") {
        this->statusCode = HTTP_VERSION_NOT_SUPPORTED;
        return (false);
    }
    size_t pos = this->_uri.find('?');
    if (pos != std::string::npos) {
        _parseQuery();
        this->_uri.erase(pos);
    }
    return (true);
}

void Request::_setPath(void)
{
    this->_path = webServer.getLocationValue(this->_serverIndex, this->_locationIndex, "location")[0];
    if (this->_path.empty())
        this->_path = webServer.getServerValue(this->_serverIndex, "location")[0];
}

void Request::_parseQuery(void)
{
    size_t      posQuery = this->_uri.find("?");
    std::string query;

    if (posQuery != std::string::npos) {
        query = this->_uri.substr(posQuery + 1);
        std::string::size_type start = 0;
        while (start < query.length()) {
            std::string::size_type equal = query.find('=', start);
            if (equal != std::string::npos) {
                std::string::size_type ampersand = query.find('&', equal);
                if (ampersand != std::string::npos) {
                    _paramQuery.push_back(query.substr(equal + 1, ampersand - equal - 1));
                    start = ampersand + 1;
                } else {
                    _paramQuery.push_back(query.substr(equal + 1));
                    break;
                }
            } else
                break;
        }
    }
}

void Request::_parseHeaders(const std::string &request)
{
    std::istringstream iss(request);
    std::string        headerLine;
    this->_has_body = false;

    while (std::getline(iss, headerLine, '\r')) {
        headerLine.erase(std::remove(headerLine.begin(), headerLine.end(), '\n'), headerLine.end());

        if (headerLine.empty())
            break;

        size_t colonPos = headerLine.find(": ");
        if (colonPos != std::string::npos) {
            std::string key    = headerLine.substr(0, colonPos);
            std::string value  = headerLine.substr(colonPos + 1);
            this->_header[key] = value;
            this->_findHeaders(key, value);
        }
    }
}

void Request::_findHeaders(std::string key, std::string value)
{
    if (key == "Host")
    {
        size_t pos = value.find(":");
        if (pos != std::string::npos) {
            std::string tmp = value.substr(pos + 1);
            this->_port     = tmp;
        }
    }
    if (key == "Content-Length")
    {
        int length = atoi(value.c_str());
        if (length > 0) {
            this->_has_body = true;
            this->_contentLength = length;
        }
    }
}

void Request::_getMaxBody(void)
{
    std::vector<std::string> maxBody;

    this->_serverIndex = webServer.searchServer(this->_port);

    if (this->_serverIndex == -1)
        return ;
    maxBody = webServer.getServerValue(this->_serverIndex, "client_max_body_size");
    if (!maxBody.empty() && !maxBody[0].empty())
                this->_maxBodySize = std::atoi(maxBody[0].c_str());
}

void Request::_getServerParam(void)
{
    std::vector<int> serverSize = webServer.getAllQtLocations();

    this->_locationSize  = serverSize[this->_serverIndex];
    this->_locationIndex = webServer.searchLocation(this->_serverIndex, this->_catchPathURI());
    this->_setRoot();
    this->_setLimitExcept();
    this->_setErrorPage();
}

std::string Request::_catchPathURI(void)
{
    size_t pos;

    if (this->_uri == "/")
        return this->_uri;

    pos = this->_uri.find('/', 1);
    if (pos != std::string::npos)
        return this->_uri.substr(0, pos);
    else
        return this->_uri;
}

void Request::_setRoot(void)
{
    std::vector<std::string> rootParam;

    this->_root = DEFAULT_ROOT;

    rootParam = webServer.getServerValue(this->_serverIndex, "root");

    if (!rootParam.empty())
        this->_root = rootParam[0];

    if (this->_locationSize != this->_locationIndex) {
        rootParam = webServer.getLocationValue(this->_serverIndex, this->_locationIndex, "root");
        if (!rootParam.empty()) {
            this->_root = rootParam[0];
        }
    }
    return;
}

void Request::_setLimitExcept(void)
{
    this->_limitExcept.clear();
    if (this->_locationSize != this->_locationIndex) {
        this->_limitExcept
            = webServer.getLocationValue(this->_serverIndex, this->_locationIndex, "limit_except");
    }
}

void Request::_setErrorPage(void)
{
    if (this->_locationSize != this->_locationIndex) {
        this->_errorPageConfig
            = webServer.getLocationValue(this->_serverIndex, this->_locationIndex, "error_page");
        if (!this->_errorPageConfig.empty())
            return;
    }
    this->_errorPageConfig = webServer.getServerValue(this->_serverIndex, "error_page");
}

void Request::_setAutoIndex(void)
{
    std::vector<std::string>    serverValue;
    std::vector<std::string>    autoindexParam;

    this->_autoIndexServer = false;
    this->_autoIndexLoc    = false;
    serverValue            = webServer.getServerValue(this->_serverIndex, "autoindex");

    if (!serverValue.empty())
    {
        if(serverValue[0] == "on")
            this->_autoIndexServer = true;
    }

    autoindexParam = webServer.getLocationValue(this->_serverIndex, this->_locationIndex, "autoindex");
    if(autoindexParam.empty())
        return;

    if (autoindexParam[0] == "on")
    {
        this->_autoIndexLoc = true;
    }

}

void Request::_getHost(void)
{
    std::string       uri = _header["Host"];
    std::stringstream ss(uri);

    if (std::getline(ss, _host, ':'))
        _host.erase(std::remove_if(_host.begin(), _host.end(), ::isspace), _host.end());
}

bool Request::_getMultipartData(std::string request)
{
    std::string contentType = _header["Content-Type"];

    size_t pos = contentType.find("boundary=");
    if (pos != std::string::npos) {
        this->_boundary = contentType.substr(pos + 9);
        this->_boundary = "--" + _boundary;
    } else {
        this->statusCode = BAD_REQUEST;
        return (true);
    }

    size_t startBody = request.find("\r\n\r\n") + 4;

    if (startBody != std::string::npos)
        _body = request.substr(startBody);
    else {
        this->statusCode = BAD_REQUEST;
        return (true);
    }
    return (false);
}

bool Request::_getBody(std::string request)
{
    std::size_t bodyStart = request.find("\r\n\r\n") + 4;

    if (bodyStart != std::string::npos)
        this->_body = request.substr(bodyStart);


    if (_maxBodySize > 0) {
        if ((_body.size() / 1024) > _maxBodySize) {
            this->statusCode = ENTITY_TOO_LARGE;
            return (true);
        }
    } else {
        Logs::printLog(Logs::ERROR, 1, "Invalid client_max_body_size.");
        return (true);
    }
    return (false);
}

std::vector<std::string>    Request::getErrorPageConfig(void)
{
    return (this->_errorPageConfig);
}

std::vector<std::string>    Request::getLimitExcept(void)
{
    return (this->_limitExcept);
}

std::string Request::getUri(void)
{
    return (this->_uri);
}

std::string Request::getRoot(void)
{
    return (this->_root);
}

std::string Request::getMethod(void)
{
    return (this->_method);
}

std::string Request::getHost(void)
{
    return (this->_host);
}

std::string Request::getPort(void)
{
    return (this->_port);
}

std::string Request::getPath(void)
{
    return (this->_path);
}

std::string Request::getBody(void)
{
    return (this->_body);
}

std::string Request::getBoundary(void)
{
    return (this->_boundary);
}

std::vector<std::string> Request::getQuery(void)
{
    return (this->_paramQuery);
}


int Request::getQtLocationsInServer(void)
{
    return (this->_locationSize);
}

int Request::getServerIndex(void)
{
    return (this->_serverIndex);
}

int Request::getLocationIndex(void)
{
    return (this->_locationIndex);
}

size_t Request::getMaxBodySize(void)
{
    return (this->_maxBodySize);
}

size_t Request::getContentLength(void)
{
    return (this->_contentLength);
}

bool        Request::getHasBody(void)
{
    return (this->_has_body);
}

bool        Request::getHasForm(void)
{
    return (this->_has_form);
}

bool        Request::getHasMultipart(void)
{
    return (this->_has_multipart);
}

bool        Request::getAutoIndexServer(void)
{
    return (this->_autoIndexServer);
}

bool        Request::getAutoIndexLoc(void)
{
    return (this->_autoIndexLoc);
}

std::string Request::getContent(void)
{
    return (this->_content);
}

void    Request::printInfos(void)
{
    std::cout << "===== REQUEST INFOS =====\n";
    std::cout << "statusCode: " << statusCode << std::endl;
    std::cout << "Uri: " << _uri << std::endl;
    std::cout << "Body: " << _body << std::endl;
    std::cout << "Port: " << _port << std::endl;
    std::cout << "Host: " << _host << std::endl;
    std::cout << "Method: " << _method << std::endl;
    std::cout << "Boundary: " << _boundary << std::endl;
    std::cout << "htppVersion: " << _httpVersion << std::endl;
    std::cout << "allowMethods[0]: " << _allowMethods[0] << std::endl;
    std::cout << "errorPageConfig[0]: " << _errorPageConfig[0] << std::endl;
    //std::cout << "paramQuery[0]: " << _paramQuery[0] << std::endl;
    std::cout << "=========================\n"; 
}