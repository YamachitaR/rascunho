/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logs.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 16:13:52 by lucasmar          #+#    #+#             */
/*   Updated: 2024/04/25 18:59:00 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logs.hpp"

Logs::Logs(void){};
Logs::~Logs(void){};

std::string Logs::_getFullData(void)
{
	std::time_t currentTime;
	std::tm *localTime;
	char timeStr[24];

	currentTime = std::time(0);
	localTime = std::localtime(&currentTime);
	std::strftime(timeStr, sizeof(timeStr), "[%d/%m/%Y - %H:%M:%S]", localTime);
	return (std::string(timeStr));
}

void Logs::printLog(typeLog type, int numberLog, std::string message)
{
	std::string color;
	std::string strType;

	switch (type)
	{
	case Logs::ERROR:
		color = COLOR_RED;
		strType = "ERROR";
		break;
	case Logs::INFO:
		color = COLOR_BLUE;
		strType = "INFO";
		break;
	case Logs::WARNING:
		color = COLOR_YELLOW;
		strType = "WARNING";
		break;
	case Logs::VERBOSE:
		color = COLOR_GREEN;
		strType = "VERBOSE";
		break;
	default:
		break;
	}

	std::cout
		<< color
		<< " - "
		<< Logs::_getFullData()
		<< " "
		<< strType
		<< ": "
		<< numberLog
		<< " - "
		<< message
		<< COLOR_RESET
		<< std::endl;
}
