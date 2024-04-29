/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/26 15:34:13 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/28 19:56:23 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Libs.hpp"

std::string queryToString(std::vector<std::string> query)
{

    std::string resp;

    if (query.empty()) {
        return (resp);
    }

    for (std::vector<std::string>::iterator it = query.begin(); it != query.end(); it++) {
        resp += *it;
        if (it != query.end() - 1) {
            resp += "&";
        }
    }
    return (resp);
}

map_ss assembleCGIHeaders(Request &request, cgi_infos infos) // descomentar variaveis
{

    map_ss map_envs;

    map_envs["AUTH_TYPE"]         = "";
    map_envs["REDIRECT_STATUS"]   = "200";
    map_envs["GATEWAY_INTERFACE"] = "CGI/1.1";
    map_envs["SCRIPT_NAME"]       = infos.cgi_path;
    map_envs["SCRIPT_FILENAME"]   = infos.cgi_path;
    map_envs["REQUEST_METHOD"]    = request.getMethod();
    map_envs["CONTENT_LENGTH"]    = to_string(request.getBody().size());
    
    map_envs["CONTENT_TYPE"]      = "text/html";
    map_envs["PATH_INFO"]         = infos.cgi_path;
    map_envs["PATH_TRANSLATED"]   = infos.cgi_path;
    map_envs["QUERY_STRING"]      = queryToString(request.getQuery());
    map_envs["REQUEST_BODY"]      = request.getBody();
    map_envs["REMOTE_ADDR"]       = "";
    map_envs["REMOTE_IDENT"]      = "";
    map_envs["REMOTE_USER"]       = "";
    map_envs["REQUEST_URI"]       = infos.cgi_path;
    map_envs["SERVER_NAME"]       = request.getHost();
    map_envs["SERVER_PROTOCOL"]   = "HTTP/1.1";
    map_envs["SERVER_SOFTWARE"]   = "Webserv/1.0";

    return (map_envs);
}

std::vector<std::string> assembleCGIEnv(map_ss map_envs)
{
    int                         i;
    map_ss::iterator            it;
    std::string                 env;
    std::vector<std::string>    envs;

    it = map_envs.begin();
    i = 0;
    while (it != map_envs.end())
    {
        env = it->first + "=" + it->second;
        envs.push_back(env);
        it++;
        i++;
    }
    return (envs);
}

cgi_infos initCgiInfos(void)
{
    cgi_infos infos;

    infos.bin = "";
    infos.cgi_path = "";
    infos.correct = false;
    
    return (infos);
}

cgi_infos isCGI(Request &request)
{
    cgi_infos   infos;
    std::string defaultRoot = DEFAULT_ROOT_CGI;
    std::string rootTmp     = getDir();
    std::string tmp_path;
    std::vector<std::string> rootParam;
    size_t pos;
    int serverIndex;

    serverIndex = request.getServerIndex();
    rootParam = webServer.getServerValue(serverIndex, "root");
    pos = defaultRoot.find("/");

    if (pos != std::string::npos && rootParam.size() > 0)
        tmp_path = rootTmp + "/" + rootParam[0] + defaultRoot.substr(pos) + request.getUri();
    else
        tmp_path = rootTmp + "/" + defaultRoot + request.getUri();

    infos = programExists(tmp_path, request);
    if (infos.correct)
        infos.cgi_path = tmp_path;
    return (infos);
}

cgi_infos programExists(std::string &path, Request &request)
{
    struct stat info;
    struct stat info2;
    cgi_infos   cgi_infos = initCgiInfos();
    std::string bin = getBin(request.getUri());

    if (stat(path.c_str(), &info) != 0 || bin == "" || stat(bin.c_str(), &info2) != 0)
        return (cgi_infos);

    if (!(S_ISREG(info2.st_mode) && (info2.st_mode & S_IRUSR)))
    {
        Logs::printLog(Logs::ERROR, 1," Error opening binary file: " + bin);
        return (cgi_infos);
    }
    cgi_infos.bin = bin;
    cgi_infos.correct = true;
    return (cgi_infos);
}

std::string getBin(const std::string &url)
{
    const char *binName;
    std::string command = "which ";

    std::string::size_type pos_slash = url.find_last_of('/');
    std::string::size_type pos_query = url.find_first_of('?');

    if (pos_slash != std::string::npos && pos_slash < url.length() - 1) {
        std::string            bin_tmp = url.substr(pos_slash + 1, pos_query - pos_slash - 1);
        std::string::size_type pos_dot = bin_tmp.find_last_of('.');
        std::string            bin     = bin_tmp.substr(pos_dot + 1, bin_tmp.length() - 1);
        if (bin == "py")
            bin = "python3";
        else
            bin = "";

        command += bin;
        binName      = command.c_str();

        FILE *stream = popen(binName, "r");
        if (!stream)
        {
            Logs::printLog(Logs::ERROR, 1,"Error getting binary name");
            return "";
        }
        char buffer[1024];
        while (fgets(buffer, 1024, stream) != NULL) {
            pclose(stream);
            std::string bin(buffer);
            bin.erase(std::remove(bin.begin(), bin.end(), '\n'), bin.end());
            return (bin);
        }
        Logs::printLog(Logs::ERROR, 1,"Error getting binary name" );
        return "";
    } else {
        Logs::printLog(Logs::ERROR, 1,"Error getting binary name");
        return "";
    }
}

std::string executeCGI(Request &request, cgi_infos infos)
{
    pid_t           pid;
    int             pipe_fd[2];
    std::string     body = "";
    unsigned int    timeout = 10000;
    struct timespec startTime;
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    std::vector<std::string> envs;
    std::string bin = infos.bin;
    std::string cgi_path = infos.cgi_path;
    int status;

    if (bin.empty())
        Logs::printLog(Logs::ERROR, 1,"There are not this program.");

    envs = assembleCGIEnv(assembleCGIHeaders(request, infos));

    char *cbin    = const_cast<char *>(bin.c_str());
    char *pathBin = const_cast<char *>(cgi_path.c_str());

    if (pipe(pipe_fd) == -1)
    {
        Logs::printLog(Logs::ERROR, 1,"Erro im pipe operaction");
        return ("");
    }

    pid = fork();
    
    std::vector<char*> envsCStyle;
    for (size_t i = 0; i < envs.size(); ++i) {
        envsCStyle.push_back(const_cast<char*>(envs[i].c_str()));
    }
    envsCStyle.push_back(NULL);

    if (pid == -1)
    {
        Logs::printLog(Logs::ERROR, 1,"Erro im fork operaction");
        return ("");
    }
    else if (!pid)
    {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);

        char *argv[] = {cbin, pathBin, NULL};
        execve(cbin, argv, &envsCStyle[0]);
        write(STDOUT_FILENO, "Status: 500 Internal Server Error\r\n\r\n", 38);
    }
    else
    {
        close(pipe_fd[1]);
        struct timespec currentTime;

        while (true)
        {
            if (waitpid(pid, &status, WNOHANG) == pid)
            {
                break;
            }
            clock_gettime(CLOCK_MONOTONIC, &currentTime);
            if (currentTime.tv_sec - startTime.tv_sec > timeout / 1000)
            {
                kill(pid, SIGTERM);
                Logs::printLog(Logs::ERROR, 504, "Scripty was aborted, took too long to execute");
                close(pipe_fd[0]);
                return ("erro504");
            }
            usleep(500);
        }
        if (WEXITSTATUS(status))
        {
            Logs::printLog(Logs::ERROR, 500, "The script had an execution error");
            close(pipe_fd[0]);
            return ("erro500");
        }
        char buffer[8192] = {0};
        int  n            = 0;
        while ((n = read(pipe_fd[0], buffer, 8191)) > 0) {
            body.append(buffer, n);
        }
        close(pipe_fd[0]);
    }
    return (body);
}