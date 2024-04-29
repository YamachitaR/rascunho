


#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "./Libs.hpp"

class Request;

class Location {
    private:
        std::string _indexPage;
        std::string _uriExtension;
        Request     _req;
 
        void            _setIndexPage(void);
        responseData    _getFileContent(void);
        responseData    _getIndexContent(void);

    public:
        Location(void);
        Location(Request);
        ~Location(void);

        void         setup(void);
        responseData getLocationContent(void);
};


#endif