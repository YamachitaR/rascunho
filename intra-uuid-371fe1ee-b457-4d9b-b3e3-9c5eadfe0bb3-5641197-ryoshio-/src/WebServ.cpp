/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 15:53:27 by lucasmar          #+#    #+#             */
/*   Updated: 2024/04/27 14:07:35 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"


WebServ::WebServ(void)
{
	this->_statusCodes = setStatus();
	this->_types = setTypes();
}

WebServ::~WebServ(void)
{
	this->finish();
}

bool WebServ::setDataServer(const char *pathConf)
{
	std::vector<std::vector<std::string> >	locations;
	std::vector<std::vector<std::string> >	servers;
	std::vector<std::string>                locationBlocks;
	std::vector<std::string>				serverBlocks;
	std::vector<int>         				serverInfo;
	std::string                             currentLocationBlock;
	std::string								currentServerBlock;
	std::string								line;
	bool									inLocation = false;
	bool									inServer = false;
	bool									blockEnd = false;
	int										numLocation;

	numLocation = 0;
	std::ifstream conf(pathConf);
	if (!conf.is_open())
	{
		return (false);
	}
	conf.clear();
	conf.seekg(0, std::ios::beg);
	while (!conf.eof())
	{
		std::getline(conf, line);

		if (verifyLineEmpty(line))
			continue;

		if (line.find("server ") != std::string::npos)
			inServer = true;

		if (line.find("location ") != std::string::npos)
			inLocation = true;

		if (inServer && !inLocation)
			currentServerBlock += line + "\n";

		if (inLocation)
		{
			numLocation++;
			currentLocationBlock += line + "\n";
		}

		if (endBlock(line))
		{
			if (inLocation)
			{
				locationBlocks.push_back(currentLocationBlock);
				currentLocationBlock.clear();
				inLocation = false;
			}
			else if (inServer && !inLocation && numLocation > 0)
			{
				serverBlocks.push_back(currentServerBlock);
				currentServerBlock.clear();
				inServer = false;
				blockEnd = true;
			}
		}

		if (!inServer && !inLocation && blockEnd)
		{
			servers.push_back(serverBlocks);
			locations.push_back(locationBlocks);
			serverBlocks.clear();
			locationBlocks.clear();
			blockEnd = false;
		}
	}
	conf.close();
	if (servers.size() < 1){
		Logs::printLog(Logs::ERROR, 3, "The server was not configured correctly");
		return (false);
	}
	populateConfs(servers, locations);
	return (true);
}

void WebServ::populateConfs(std::vector<std::vector<std::string> > servers, std::vector<std::vector<std::string> > locations)
{

	_sizeServers = servers.size();
	_dataServers = new conf_servers[_sizeServers];

	for (size_t i = 0; i < _sizeServers; i++)
	{
		allocateServers(&_dataServers[i], locations[i].size());
		_dataServers[i].server = setParams(servers[i][0], _dataServers[i].server);
		for (size_t j = 0; j < locations[i].size(); j++)
		{
			_dataServers[i].locations[j] =
				setParams(locations[i][j], _dataServers[i].locations[j]);
		}
		_qtLocation.push_back(locations[i].size());
	}
}

dic *WebServ::setParams(const std::string str, dic *vconfs)
{
	std::vector<std::string> tokens;
	std::string tmp_str = str;
	std::string token;

	size_t pos = str.find('\n');
	while (pos != std::string::npos)
	{
		token = tmp_str.substr(0, pos);
		tokens.push_back(rmSpaces(token));
		tmp_str = tmp_str.substr(pos + 1);
		pos = tmp_str.find('\n');
	}
	for (size_t i = 0; i < tokens.size(); i++)
	{
		pos = tokens[i].find(' ');
		if (pos != std::string::npos)
		{
			std::string key = tokens[i].substr(0, pos);
			std::vector<std::string> value = splitTokens(tokens[i].substr(pos + 1));
			if (key != "server")
				(*vconfs)[key] = value;
		}
	}
	return vconfs;
}

bool WebServ::connect(void)
{

	size_t	qtServers;
	size_t	i = 0;
	std::vector<std::string> port;

	qtServers = this->getQtSevers();

	if (qtServers == 0)
	{
		Logs::printLog(Logs::ERROR, 1, "The server was not configured correctly");
		this->finish();
		exit(1);
	}

	while (i < qtServers)
	{
		Socket *socket;

		port = this->getServerValue(i, "listen");

		if (port.empty())
			socket = new Socket();
		else
			socket = new Socket(port[0]);

		socket->init();
		socket->bindAddr();
		socket->listenConnections();
		this->_sockets.push_back(socket);
		this->_poll.init(socket->getFd());
		i++;
	}
	return (true);
}

int WebServ::start(void)
{
	while (true)
	{
		if (this->_poll.execute() == -1)//coloca um socket dentro da poll
		{
			Logs::printLog(Logs::ERROR, 1, "Error creating poll");
			return (1);
		}
		for (size_t i = 0; i < this->_poll.getSize(); ++i)
		{
			//std::cout << "socket fd : " << this->_poll.getPollFd(i);
			if (this->_poll.isRead(i))//veridica se o socket tem alguma conexão pendente
			{
				//std::cout << " - Conexão pendente\n";
				if (this->_poll.isSocketServer(i))//verifica se o socket é do server ou cliente
				{
					//std::cout << " - Socket Servidor\n";
					if (!this->_newCliet(i))//aceita e cria o novo cliente
						continue;
				}
				else
				{
					//std::cout << "- Socket Cliente\n";
					int clientSocket = this->_poll.getPollFd(i);
					if (clientSocket < 0)
					{
						Logs::printLog(Logs::ERROR, 1, "I didn't find the index");
						continue;
					}
					processClientData(clientSocket);
				}
			
			}
			//std::cout << std::endl;
		}
		this->_poll.removeMarkedElements();
	}
	return (0);
}

void WebServ::finish(void)
{
	for (std::vector<Socket *>::iterator it = this->_sockets.begin();
			it != this->_sockets.end(); ++it) {
		delete *it;
	}
	this->_sockets.clear();
	
	for (size_t i = 0; i < _sizeServers; i++)
	{
		deallocateServers(&_dataServers[i], _qtLocation[i]);
	}
	delete[] _dataServers;
	//this->_parser.clearParams();
	//this->_poll.closePoll();
}

void WebServ::addFdToClose(int fd)
{
	_poll.addFdToClose(fd);
}

/////////////////////////////////////////////////

bool WebServ::_newCliet(size_t i) //mudar de nome
{
	try {
		Socket *client;
		int     clientSocketFd;

		client         = new Socket();
		clientSocketFd = client->acceptClient(this->_poll.getPollFd(i));
		this->_poll.addPoll(clientSocketFd, POLLIN | POLLOUT);
		delete client;

		return (true);
	} catch (const std::exception &e) {
		Logs::printLog(Logs::ERROR, 1, e.what());
		return (false);
	}
}

// SEARCH

int	WebServ::searchServer(std::string port)
{
	int i = 0;
	std::string	temp;
 
	while (i < (int)_sizeServers)
	{
		temp = this->getServerValue(i, "listen")[0];
		if (temp == port)
			return (i);
		i++;
	}
	return (-1);
}

int WebServ::searchLocation(size_t iS, std::string path)
{
	int locationSize = this->getAllQtLocations()[iS];
	std::vector<std::string> locationParam;
	int i = locationSize;
	
	for (i = 0; i < locationSize + 1; i++)
	{
		locationParam = this->getLocationValue(iS, i, "location");
		if (std::find(locationParam.begin(), locationParam.end(), path) != locationParam.end())
			break ;
	}
	return (i);
}

//SETERS

void WebServ::_setStatusCode(void)
{
	this->_statusCodes["100"] = "Continue";
	this->_statusCodes["101"] = "Switching Protocols";
	this->_statusCodes["102"] = "Processing";
	this->_statusCodes["103"] = "Early Hints";
	this->_statusCodes["200"] = "OK";
	this->_statusCodes["201"] = "Created";
	this->_statusCodes["202"] = "Accepted";
	this->_statusCodes["203"] = "Non-Authoritative Information";
	this->_statusCodes["204"] = "No Content";
	this->_statusCodes["205"] = "Reset Content";
	this->_statusCodes["206"] = "Partial Content";
	this->_statusCodes["207"] = "Multi-Status";
	this->_statusCodes["208"] = "Already Reported";
	this->_statusCodes["226"] = "IM Used";
	this->_statusCodes["300"] = "Multiple Choices";
	this->_statusCodes["301"] = "Moved Permanently";
	this->_statusCodes["302"] = "Found";
	this->_statusCodes["303"] = "See Other";
	this->_statusCodes["304"] = "Not Modified";
	this->_statusCodes["307"] = "Temporary Redirect";
	this->_statusCodes["308"] = "Permanent Redirect";
	this->_statusCodes["400"] = "Bad Request";
	this->_statusCodes["401"] = "Unauthorized";
	this->_statusCodes["402"] = "Payment Required";
	this->_statusCodes["403"] = "Forbidden";
	this->_statusCodes["404"] = "Not Found";
	this->_statusCodes["405"] = "Method Not Allowed";
	this->_statusCodes["406"] = "Not Acceptable";
	this->_statusCodes["407"] = "Proxy Authentication Required";
	this->_statusCodes["408"] = "Request Timeout";
	this->_statusCodes["409"] = "Conflict";
	this->_statusCodes["410"] = "Gone";
	this->_statusCodes["411"] = "Length Required";
	this->_statusCodes["412"] = "Precondition Failed";
	this->_statusCodes["413"] = "Payload Too Large";
	this->_statusCodes["414"] = "URI Too Long";
	this->_statusCodes["415"] = "Unsupported Media Type";
	this->_statusCodes["416"] = "Range Not Satisfiable";
	this->_statusCodes["417"] = "Expectation Failed";
	this->_statusCodes["418"] = "I'm a teapot";
	this->_statusCodes["421"] = "Misdirected Request";
	this->_statusCodes["422"] = "Unprocessable Content";
	this->_statusCodes["423"] = "Locked";
	this->_statusCodes["424"] = "Failed Dependency";
	this->_statusCodes["425"] = "Too Early";
	this->_statusCodes["426"] = "Upgrade Required";
	this->_statusCodes["428"] = "Precondition Required";
	this->_statusCodes["429"] = "Too Many Requests";
	this->_statusCodes["431"] = "Request Header Fields Too Large";
	this->_statusCodes["451"] = "Unavailable For Legal Reasons";
	this->_statusCodes["500"] = "Internal Server Error";
	this->_statusCodes["501"] = "Not Implemented";
	this->_statusCodes["502"] = "Bad Gateway";
	this->_statusCodes["503"] = "Service Unavailable";
	this->_statusCodes["504"] = "Gateway Timeout";
	this->_statusCodes["505"] = "HTTP Version Not Supported";
	this->_statusCodes["506"] = "Variant Also Negotiates";
	this->_statusCodes["507"] = "Insufficient Storage";
	this->_statusCodes["508"] = "Loop Detected";
	this->_statusCodes["510"] = "Not Extended";
	this->_statusCodes["511"] = "Network Authentication Required";
}

void WebServ::setBytesRead(int nbr)
{
	_bytesRead = nbr;
}

// GETERS

int WebServ::getBytesRead(void)
{
	return (_bytesRead);
}

std::string WebServ::getStatusCode(std::string code)
{
	std::map<std::string, std::string>::const_iterator it = this->_statusCodes.find(code);

	if (it == this->_statusCodes.end())
		return ("");
	return (it->second);
}

size_t WebServ::getQtSevers(void)
{ 
	return (this->_sizeServers);
}

std::vector<int> WebServ::getAllQtLocations(void)
{
	std::vector<int> sizeServers;

	sizeServers.push_back(_sizeServers); // Numero total de servers
	for (size_t i = 0; i < _sizeServers; i++)
		sizeServers.push_back(_qtLocation[i]); // quantidade de locations de casa server
	return sizeServers;
}

std::vector<std::string> WebServ::getServerValue(size_t iS, std::string key)
{
	if (iS >= _sizeServers){
		Logs::printLog(Logs::WARNING, 3, "There is no such index on the server");
		return std::vector<std::string>();
	}
		
	if (_dataServers[iS].server->find(key) != _dataServers[iS].server->end())
		return (*_dataServers[iS].server)[key];;

	//Logs::printLog(Logs::WARNING, 3, "Parameter was not found:" + key);
	return std::vector<std::string>();
}

std::vector<std::string> WebServ::getLocationValue(size_t iS, size_t iL, std::string key)
{
	if (iS >=(size_t) _sizeServers)
		return std::vector<std::string>();
	
	if (iL >= (size_t) _qtLocation[iS])
		return std::vector<std::string>();

	if (_dataServers[iS].locations[iL]->find(key) == _dataServers[iS].locations[iL]->end())
		return std::vector<std::string>();
	return ((*_dataServers[iS].locations[iL])[key]); 
}

map_ss	WebServ::getDicStatusCodes(void)
{
	return (this->_statusCodes);
}

map_ss	WebServ::getDicTypes(void)
{
	return (this->_types);
}
