/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 15:57:54 by lucasmar          #+#    #+#             */
/*   Updated: 2024/04/28 20:51:21 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Libs.hpp"

void	handleStop(int signal);
WebServ	webServer;

int	main(int argc, char *argv[]){
	const char*	confPath;

	if(argc > 2) {
		Logs::printLog(Logs::ERROR, 0, "Number of invalid arguments!");
		return (1);
	}
	if(argc ==2){
		confPath = argv[1];
	}else{
		confPath = "./config/default.conf";
	}
	if(!CheckFile::check(confPath))
		return(1);
	signal(SIGINT, handleStop);
	if(!webServer.setDataServer(confPath)){
		Logs::printLog(Logs::ERROR, 1, "Erro set data server");
		return(1);
	}

	if(!webServer.connect()){
		Logs::printLog(Logs::ERROR, 1, "Error when trying to connect");
		return(1);
	}
	
	return(webServer.start());
}

void	handleStop(int signal){
	if (signal == SIGINT) {
		Logs::printLog(Logs::INFO, 3, "Stop server :(");
		std::exit(0);
	}
}
