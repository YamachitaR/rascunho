/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/25 19:21:24 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/27 23:44:51 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Libs.hpp"

PostMethod::PostMethod(): created(false) {}

PostMethod::~PostMethod() {}

PostMethod::PostMethod(Request request): _req(request), created(false){}

responseData PostMethod::handleMethod()
{
    created = false;

    if (_req.getHasBody())
    {
        if (_req.getHasMultipart())
        {
            if (handleMultipart())
            {
                _res = getErrorPageContent(_req.getErrorPageConfig(), ENTITY_TOO_LARGE, _req.getUri(), _req.getRoot());
                Logs::printLog(Logs::ERROR, 1, "Request Entity Too Large.");
                return (_res);
            }
            if (created && _file == true)
            {
                _res = getJson("{\"status\": \"success\", \"message\": \"Resource created successfully\"}", 201);
                Logs::printLog(Logs::INFO, 1, "File created.");
                return (_res);
            }
            else if (!created && _file == true)
            {;
                _res = getErrorPageContent(_req.getErrorPageConfig(), INTERNAL_SERVER_ERROR, _req.getUri(), _req.getRoot());
                Logs::printLog(Logs::ERROR, 1, "Unable to create file.");
                return (_res);
            }
        }
        if (_req.getHasForm())
            handleForm();
        //else
            //std::cout << "Body: " << _req.getBody() << "\n";

        _res = getJson("{\"status\": \"success\", \"message\": \"Successful operation\"}", OK);
        Logs::printLog(Logs::INFO, 1, "Post request completed successfully.");
    }
    else if (!_req.getHasBody())
    {
        _res = getErrorPageContent(_req.getErrorPageConfig(), BAD_REQUEST, _req.getUri(), _req.getRoot());
        Logs::printLog(Logs::ERROR, 1, "No content.x");
    }
    else
    {
        _res = getErrorPageContent(_req.getErrorPageConfig(), INTERNAL_SERVER_ERROR, _req.getUri(), _req.getRoot());
        Logs::printLog(Logs::ERROR, 1, "Internal Server Error.");
    }
    return (_res);
}

bool PostMethod::handleMultipart()
{
    std::string boundary = _req.getBoundary();
    std::string body     = _req.getBody();
    size_t      endPos;

    _formData.clear();
    _file       = false;
    size_t pos  = 0;
    _bodySize   = 0;
    while ((pos = body.find(boundary, pos)) != std::string::npos)
    {
        pos += boundary.length();
        endPos = body.find(boundary, pos);
        if (endPos != std::string::npos)
            parseMultipartFormData(pos, endPos);
    }
    if (verifyLimit())
        return (true);
    //if (_file == false)
        //print();
    return (false);
}

bool    PostMethod::verifyLimit()
{
    if ((_bodySize / 1024) > _req.getMaxBodySize() || (_req.getContentLength() / 1024) > _req.getMaxBodySize())
        return (true);
    return (false);
}

std::string setFileName(size_t pos, std::string data)
{
    std::srand(static_cast<unsigned int>(std::time(NULL)));
    int randomNumber = std::rand() % 500;

    std::ostringstream oss;
    oss << "file" << randomNumber;

    size_t filenameEnd = data.find("\"", pos);
    std::string aux         = data.substr(pos, filenameEnd - pos);
    size_t      dot         = aux.find(".");

    if (dot != std::string::npos)
    {
        std::string extension = aux.substr(dot);
        oss << extension;
    }

    std::string fileName = oss.str();
    return (fileName);
}

void PostMethod::parseMultipartFormData(size_t pos, size_t endPos)
{
    size_t bodyEnd, namePos, nameEnd, filenamePos, filenameEnd;
    std::string body, partData, fileName, data, value, name;

    body     = _req.getBody();
    partData = body.substr(pos, endPos - pos);
    bodyEnd  = partData.find("\r\n\r\n");  

    if (bodyEnd != std::string::npos)
    {
        data  = partData.substr(0, bodyEnd);
        value = partData.substr(bodyEnd + 4);

        namePos = data.find("name=\"");
        if (namePos != std::string::npos)
        {
            namePos += 6;
            nameEnd = data.find("\"", namePos);
            if (nameEnd != std::string::npos)
            {
                name = data.substr(namePos, nameEnd - namePos);
                filenamePos = data.find("filename=\"");

                if (filenamePos != std::string::npos)
                {
                    filenamePos += 10;
                    filenameEnd = data.find("\"", filenamePos);
                    if (filenameEnd != std::string::npos)
                    {
                        fileName = setFileName(filenamePos, data);
                        if ((_req.getContentLength() / 1024) < _req.getMaxBodySize())
                        {
                            saveFile(fileName, value);
                        }
                        _file = true;
                    }
                }
                else
                {
                    _bodySize += value.size();
                    _formData[name] = value;
                }
            }
        }
    }
}

void PostMethod::saveFile(std::string &fileName, const std::string &value)
{
    std::string resourcePath = getDir();

    resourcePath = resourcePath + "/" + this->_req.getRoot() + "/method/" + fileName;
    std::ifstream checkFile(resourcePath.c_str());
    if (checkFile.good())
    {
        created = false;
        Logs::printLog(Logs::ERROR, 1, "The file already exists.");
        return;
    }

    std::ofstream file(resourcePath.c_str(), std::ios::binary);
    if (file.is_open())
    {
        file.write(value.c_str(), value.length());
        file.close();
        created = true;
        Logs::printLog(Logs::INFO, 1, "File path: " + resourcePath);
    }
    else
        created = false;
}

std::string replaceChar(const std::string &input)
{
    std::string output;
    size_t      pos = 0;
    int         decodedChar;

    while (pos < input.length())
    {
        if (input[pos] == '%')
        {
            if (pos + 2 < input.length())
            {
                char hex[3]      = {input[pos + 1], input[pos + 2], '\0'};
                decodedChar = strtol(hex, NULL, 16);
                output.push_back(static_cast<char>(decodedChar));
                pos += 2;
            }
        }
        else if (input[pos] == '+')
            output.push_back(' ');
        else
            output.push_back(input[pos]);
        pos++;
    }
    return output;
}

void PostMethod::handleForm()
{
    std::string        body = _req.getBody();
    std::istringstream ss(body);
    std::string        pair;
    std::string fieldName;
    std::string fielddata;
    size_t equal;

    while (std::getline(ss, pair, '&'))
    {
        equal = pair.find('=');
        if (equal != std::string::npos)
        {
            fieldName = replaceChar(pair.substr(0, equal));
            fielddata = replaceChar(pair.substr(equal + 1));
            _formData[fieldName]  = fielddata + "\r\n";
        }
    }
    //print();
}

void PostMethod::print()
{
    std::cout << P_BLUE << std::endl;
    std::cout << "      ------------ Content Form ------------" << std::endl;
    std::cout << std::left << std::setw(20) << "Key" << std::setw(30) << "| Value" << std::endl;
    std::cout << std::setfill('-') << std::setw(50) << "-" << std::setfill(' ') << std::endl;

    std::map<std::string, std::string>::iterator it;
    for (it = _formData.begin(); it != _formData.end(); ++it)
        std::cout << std::left << std::setw(19) << it->first << " | " << it->second;
    std::cout << std::setfill('-') << std::setw(50) << "-" << std::setfill(' ') << std::endl;
    std::cout << RESET_COLOR << std::endl;
}

responseData getJson(std::string body, int status)
{
    responseData data;

    data = setResponseData(status, "application/json", body, (int)body.length(), "");
    return (data);
}

std::string getDir(void)
{
    char        cwd[1024];
    size_t      pos;
    std::string rPath;

    rPath = "./";

    if (getcwd(cwd, 1024) != NULL)
    {
        std::string dir(cwd);
        pos = dir.find(rPath);
        if (pos != std::string::npos)
            dir = dir.substr(0, pos + 2);
        return dir;
    }
    else
    {
        Logs::printLog(Logs::ERROR, 1, "Error getting current working directory");
        return "";
    }
}