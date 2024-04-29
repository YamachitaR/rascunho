/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 21:29:29 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/28 12:12:53 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "./Libs.hpp"

class Request
{
    private:
        std::string                         _uri;//
        std::string                         _path;
        std::string                         _body;
        std::string                         _port;
        std::string                         _host;
        std::string                         _method;
        std::string                         _boundary;
        std::string                         _httpVersion;
        std::vector<std::string>            _allowMethods;
        std::vector<std::string>            _paramQuery;
        map_ss                              _header;
        std::string                         _root;
        std::vector<std::string>           _errorPageConfig;
        std::vector<std::string>           _limitExcept;
        int                                 _serverIndex;
        int                                 _locationIndex;
        int                                 _locationSize;
        size_t                              _maxBodySize;
        size_t                              _contentLength;
        //
        bool                                _has_body;
        bool                                _has_form;
        bool                                _has_multipart;
        bool                                _autoIndexServer;
        bool                                _autoIndexLoc;
        std::string                         _content;

        void        _getHost(void);
        bool        _getMultipartData(std::string request);
        bool        _getBody(std::string request);
        void        _getServerParam(void);
        void        _getMaxBody(void);
        void        _init();
        bool        _parseFirstLine(std::string &requestLine);
        void        _parseQuery(void);
        void        _parseHeaders(const std::string &request);
        void        _findHeaders(std::string key, std::string value);
        std::string _catchPathURI(void);
        void        _setRoot(void);
        void        _setPath(void);
        void        _setLimitExcept(void);
        void        _setErrorPage(void);
        void        _setAutoIndex(void);
// os get que estão com get com retonro void mudar de nome para catch, take ou algo do genero

    public:
        Request();
        ~Request();
        int         statusCode;
        bool        requestStart(std::string request);
        void        printInfos(void);
        std::vector<std::string>    getErrorPageConfig(void);
        std::vector<std::string>    getLimitExcept(void);

        std::string getUri(void);
        std::string getRoot(void);
        std::string getMethod(void);
        std::string getHost(void);
        std::string getPort(void);
        std::string getPath(void);
        std::string getBody(void);
        std::string getBoundary(void);
        bool        getHasBody(void);
        bool        getHasForm(void);
        bool        getHasMultipart(void);
        bool        getAutoIndexServer(void);
        bool        getAutoIndexLoc(void);
        std::string getContent(void);
        std::vector<std::string> getQuery(void);
        int getQtLocationsInServer(void);
        int getServerIndex(void);
        int getLocationIndex(void);
        size_t getMaxBodySize(void);
        size_t getContentLength(void);
};


# endif
