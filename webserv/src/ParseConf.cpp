/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConf.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ryoshio- <ryoshio-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 09:23:58 by ryoshio-          #+#    #+#             */
/*   Updated: 2024/04/27 22:32:09 by ryoshio-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseConf.hpp"

std::vector<std::string> separateServerBlocks(const std::string &config)
{
    std::vector<std::string> blocks;
    std::string::size_type pos = 0;

    while ((pos = config.find("server {", pos)) != std::string::npos)
    {
        std::string::size_type blockStart = pos;

        std::string::size_type blockEnd = config.find("}", blockStart);
        if (blockEnd == std::string::npos)
        {
            break; 
        }
        blockEnd = config.find("\n}", blockEnd); 
        if (blockEnd == std::string::npos)
        {
            break; 
        }
        
        blockEnd += 2; 
        blocks.push_back(config.substr(blockStart, blockEnd - blockStart));
        pos = blockEnd;
    }
    return blocks;
}

std::string extractServerParams(const std::string &serverBlock)
{
    std::istringstream iss(serverBlock);
    std::string line;
    std::string serverParams;
    bool insideLocation = false;

    while (std::getline(iss, line))
    {
        if (line.find("location") != std::string::npos)
        {
            insideLocation = true;
            continue; 
        }
        if (line.find("}") != std::string::npos)
        {
            insideLocation = false;
            continue; 
        }
        if (insideLocation)
        {
            continue; 
        }
        if (line.find("server {") != std::string::npos)
        {
            continue; 
        }
        serverParams += line + "\n";
    }
    return serverParams;
}

std::vector<std::string> extractLocations(const std::string &serverBlock)
{
    std::istringstream iss(serverBlock);
    std::string line;
    std::vector<std::string> locations;
    bool insideLocation = false;
    std::string currentLocation;

    while (std::getline(iss, line))
    {
        if (line.find("location") != std::string::npos)
        {
            insideLocation = true;
            currentLocation = line + "\n";
            continue; 
        }
        if (line.find("}") != std::string::npos && insideLocation)
        {
            insideLocation = false;
            locations.push_back(currentLocation + line); 
            continue;                                    
        }
        if (insideLocation)
        {
            currentLocation += line + "\n"; 
        }
    }
    return locations;
}

std::string getFirstWord(const std::string &text, int position)
{
    std::istringstream iss(text); 
    std::string line;
    int nbrLine;

    
    nbrLine = 0;
    while (std::getline(iss, line))
    {
        std::istringstream lineStream(line); 
        std::string firstWord;

        if (nbrLine == position)
        {
            lineStream >> firstWord;
            return firstWord;
        }
        nbrLine++;
    }
    return "";
}

int countWordOccurrencesLine(const std::string &text, const std::string &word)
{
    std::istringstream iss(text);
    std::string line;
    int count;

    count = 0;
    while (std::getline(iss, line))
    {
        std::istringstream lineStream(line); 
        std::string firstWord;

        lineStream >> firstWord;
        if (firstWord == word)
        {
            count++;
        }
    }
    return count;
}

std::string getParameterValue(const std::string &text, const std::string &parameter)
{
   
    size_t pos = text.find(parameter);
    if (pos == std::string::npos)
    {
        return "";
    }

   
    pos += parameter.length();
    while (pos < text.length() && text[pos] == ' ')
        pos++;


    size_t end_pos = text.find("\n", pos);
    if (end_pos == std::string::npos)
        return text.substr(pos);
    
    return text.substr(pos, end_pos - pos);
}

bool isNumeric(const std::string &str)
{
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (!(str[i] >= '0' && str[i] <= '9'))
        {
            return false;
        }
    }
    return true;
}

bool verifyLineEmpty(const std::string &text)
{

	bool emptyLine = true;

	for (size_t i = 0; i < text.length(); i++)
	{
		if (!isspace(text[i]))
		{
			emptyLine = false;
			return emptyLine;
		}
	}
	return emptyLine;
}

std::vector<std::string> splitTokens(const std::string str)
{
	std::vector<std::string> vtokens;
	std::istringstream iss(str);
	std::string token;

	while (iss >> token)
	{
		if (token == "{" || token == "}")
			continue;
		vtokens.push_back(token);
	}
	return vtokens;
}

std::string rmSpaces(const std::string &input)
{
	std::string result;
	bool previousCharWasSpace = false;

	for (size_t i = 0; i < input.length(); i++)
	{
		if (input[i] != ' ')
		{
			result += input[i];
			previousCharWasSpace = false;
		}
		else if (!previousCharWasSpace)
		{
			result += ' ';
			previousCharWasSpace = true;
		}
	}
	if (result[0] == ' ')
	{
		result.erase(0, 1);
	}
	if (!result.empty() && result[result.length() - 1] == ' ')
	{
		result.erase(result.length() - 1);
	}
	return result;
}

bool endBlock(const std::string &line)
{
	std::string::size_type pos = 0;
	bool onlyClosingBrace = true;
	int countBrace = 0;
	int countSpace = 0;

	while (pos < line.length() && line[pos] != '\n')
	{
		if (line[pos] != '}' && line[pos] != ' ')
		{
			onlyClosingBrace = false;
			break;
		}
		if (line[pos] == '}')
			countBrace++;
		if (line[pos] == ' ')
			countSpace++;
		++pos;
	}
	if (countBrace != 1)
		onlyClosingBrace = false;
	if (countSpace == 0 && countBrace == 1)
		onlyClosingBrace = true;

	return onlyClosingBrace;
}

void allocateServers(conf_servers *stConfServer, int qtLocation)
{

	stConfServer->server = new dic;
	stConfServer->locations = new dic *[qtLocation];

	for (int i = 0; i < qtLocation; i++)
		stConfServer->locations[i] = new dic;
}

void deallocateServers(conf_servers *stConfServer, int qtLocation)
{
	if (stConfServer)
	{
		for (int i = 0; i < qtLocation; i++)
		{
			if (stConfServer->locations[i])
			{
				delete stConfServer->locations[i];
			}
		}
		if (stConfServer->locations)
		{
			delete[] stConfServer->locations;
		}
		delete stConfServer->server;
	}
}
