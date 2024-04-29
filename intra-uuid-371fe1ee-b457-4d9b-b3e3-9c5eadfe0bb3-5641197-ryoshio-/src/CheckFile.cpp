/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckFile.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ryoshio- <ryoshio-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/30 11:04:53 by ryoshio-          #+#    #+#             */
/*   Updated: 2024/04/27 22:28:02 by ryoshio-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CheckFile.hpp"

bool CheckFile::check(std::string path)
{
    long unsigned int i;
    std::string text;
    std::set<std::string> validWords;
    std::vector<std::string> separateServer;

    text = readFileContents(path);

    if (!checkBraces(text))
    {
        Logs::printLog(Logs::ERROR, 11, "The " + path + " file is not correct closing/opening brackets");
        return false;
    }

    int j = _hasDuplicateListenPorts(text);
    if(j != -1){
        Logs::printLog(Logs::ERROR, 12, "Duplicate listen:" + to_string(j));
        return false;
    }
    
    validWords = _getValidFirstWords();
    if (_isFirstWordInSet(text, validWords) > -1)
    {
        Logs::printLog(Logs::ERROR, 12, "The " + path + " file contains an error on line " + to_string(_isFirstWordInSet(text, validWords)) + ".");
        return false;
    }

    separateServer = separateServerBlocks(text);
    if (separateServer.size() < 1)
    {
        Logs::printLog(Logs::ERROR, 13, "Server not found or in the wrong position.");
        return false;
    }

    i = 0;
    while (i < separateServer.size())
    {
        if (!_checkServerParams(separateServer[i]))
        {
            return false;
        }

        if (!_checkSLocationParams(separateServer[i]))
        {
            return false;
        }

        i++;
    }

    return true;
}

std::set<std::string> CheckFile::_getValidFirstWords(void)
{
    std::set<std::string> validWords;

    validWords.insert("server");
    validWords.insert("server_name");
    validWords.insert("listen");
    validWords.insert("host");
    validWords.insert("error_page");
    validWords.insert("index");
    validWords.insert("client_max_body_size");
    validWords.insert("root");
    validWords.insert("word");
    validWords.insert("location");
    validWords.insert("autoindex");
    validWords.insert("allow_method");
    validWords.insert("limit_except");
    validWords.insert("redirect");
    validWords.insert("alias");
    validWords.insert("cgi_path");
    validWords.insert("cgi_ext");

    validWords.insert("}");
    return validWords;
}

std::set<std::string> CheckFile::_getValidServerWords(void)
{
    std::set<std::string> validWords;

    validWords.insert("server_name");
    validWords.insert("listen");
    validWords.insert("host");
    validWords.insert("error_page");
    validWords.insert("index");
    validWords.insert("client_max_body_size");
    validWords.insert("root");
    validWords.insert("word");
    validWords.insert("autoindex");

    return (validWords);
}

std::set<std::string> CheckFile::_getValidLocationWords(void)
{
    std::set<std::string> validWords;

    validWords.insert("location");
    validWords.insert("error_page");
    validWords.insert("index");
    validWords.insert("autoindex");
    validWords.insert("allow_method");
    validWords.insert("limit_except");
    validWords.insert("redirect");
    validWords.insert("alias");
    validWords.insert("cgi_path");
    validWords.insert("cgi_ext");
    validWords.insert("}");

    return (validWords);
}

int CheckFile::_hasDuplicateListenPorts(const std::string& config) {
    std::istringstream iss(config);
    std::string line;
    std::set<int> listenPorts;

    while (std::getline(iss, line)) {
        
        if (line.find("listen") != std::string::npos) {
            std::istringstream lineStream(line);
            std::string key, value;
            lineStream >> key >> value;
            int port = std::atoi(value.c_str());

            if (listenPorts.find(port) != listenPorts.end()) {
                return (port); 
            } else {
                listenPorts.insert(port);
            }
        }
    }

    return (-1); 
}

int CheckFile::_isFirstWordInSet(const std::string &text, const std::set<std::string> &wordSet)
{
    std::istringstream iss(text);
    std::string line;
    int nbrLine;

    nbrLine = 1;
    while (std::getline(iss, line))
    {
        std::istringstream lineStream(line); 
        std::string firstWord;

        if (lineStream >> firstWord)
        {
            bool found = false;
            for (std::set<std::string>::const_iterator it = wordSet.begin(); it != wordSet.end(); ++it)
            {
                if (*it == firstWord)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                return nbrLine; 
            }
        }
        nbrLine++;
    }
    return -1;
}

bool CheckFile::_checkServerParams(std::string element)
{
    std::string serverParams;
    std::set<std::string> validWords;

    serverParams = extractServerParams(element);

    if (serverParams.length() < 1)
    {
        Logs::printLog(Logs::ERROR, 14, "not found Server");
        return false;
    }

    validWords = _getValidServerWords();
    if (_isFirstWordInSet(serverParams, validWords) != -1)
    {
        Logs::printLog(Logs::ERROR, 15, "This element is in the wrong position: " + getFirstWord(serverParams, _isFirstWordInSet(serverParams, validWords) - 1));
        return false;
    }

    for (std::set<std::string>::iterator it = validWords.begin(); it != validWords.end(); ++it)
    {
        if (countWordOccurrencesLine(serverParams, *it) > 1)
        {
            Logs::printLog(Logs::ERROR, 17, *it + " is duplicated");
            return false;
        }
    }
    
    if (!_checkServerParamsValue(serverParams))
        return false;
        
    return true;
}

bool CheckFile::_checkSLocationParams(std::string text)
{
    long unsigned int i;
    std::set<std::string> validWords;
    std::vector<std::string> location;

    location = extractLocations(text);

    if (location.size() < 1)
    {
        Logs::printLog(Logs::ERROR, 17, "not found Location");
        return false;
    }

    validWords = _getValidLocationWords();

    i = 0;
    while (i < location.size())
    {
        if (_isFirstWordInSet(location[i], validWords) != -1)
        {
            Logs::printLog(Logs::ERROR, 18, "This element is in the wrong position: " + getFirstWord(location[i], _isFirstWordInSet(location[i], validWords) - 1));
            return false;
        }

        for (std::set<std::string>::iterator it = validWords.begin(); it != validWords.end(); ++it)
        {
            if (countWordOccurrencesLine(location[i], *it) > 1)
            {
                Logs::printLog(Logs::ERROR, 19, *it + " is duplicated");
                return false;
            }
        }

        i++;
    }
    return true;
}

bool CheckFile::_checkServerParamsValue(std::string text)
{
    std::string value;

    value = getParameterValue(text, "listen");
    if (!isNumeric(value))
    {
        Logs::printLog(Logs::ERROR, 20, "listen is not correct:" + value);
        return false;
    }

    return true;
}

bool checkBraces(const std::string &text)
{
    std::stack<char> stack;
    char c;
    size_t i;

    i = 0;
    while (i < text.length())
    {
        c = text[i];
        if (c == '{')
        {
            stack.push(c);
        }
        else if (c == '}')
        {
            if (stack.empty() || stack.top() != '{')
            {
                return false;
            }
            stack.pop();
        }
        i++;
    }

    return stack.empty();
}

bool fileExists(char *path)
{
    std::ifstream file(path);
    bool exists = file.good();
    file.close();
    return exists;
}

std::string readFileContents(const std::string &filename)
{
    std::ifstream file(filename.c_str());
    std::stringstream buffer;

    if (file)
    {
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }
    else
    {
        return "";
    }
}
