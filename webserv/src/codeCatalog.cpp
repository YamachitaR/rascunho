/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codeCatalog.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esilva-s <esilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/20 10:32:03 by esilva-s          #+#    #+#             */
/*   Updated: 2024/04/21 20:49:55 by esilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codeCatalog.hpp"

std::string getStatus(std::string status, map_ss codes)
{
    std::map<std::string, std::string>::const_iterator it = codes.find(status);

    if (it != codes.end())
        return (status + " " + it->second);
    return ("");
}

std::string getTypes(std::string extension, map_ss types)
{
    std::map<std::string, std::string>::const_iterator it = types.find(extension);

    if (it != types.end())
        return (it->second);
    else
        return ("application/octet-stream");
}

map_ss setTypes(void)
{
    map_ss types;

    types[".aac"]  = "audio/aac";
    types[".abw"]  = "application/x-abiword";
    types[".arc"]  = "application/x-freearc";
    types[".avif"] = "image/avif";
    types[".avi"]  = "video/x-msvideo";
    types[".azw"]  = "application/vnd.amazon.ebook";
    types[".bin"]  = "application/octet-stream";
    types[".bmp"]  = "image/bmp";
    types[".bz"]   = "application/x-bzip";
    types[".bz2"]  = "application/x-bzip2";
    types[".cda"]  = "application/x-cdf";
    types[".csh"]  = "application/x-csh";
    types[".css"]  = "text/css";
    types[".csv"]  = "text/csv";
    types[".doc"]  = "application/msword";
    types[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    types[".eot"]  = "application/vnd.ms-fontobject";
    types[".epub"] = "application/epub+zip";
    types[".gz"]   = "application/gzip";
    types[".gif"]  = "image/gif";
    types[".htm"]  = "text/html";
    types[".html"] = "text/html";
    types[".ico"]  = "image/vnd.microsoft.icon";
    types[".ics"]  = "text/calendar";
    types[".jar"]  = "application/java-archive";
    types[".jpeg"] = "image/jpeg";
    types[".jpg"]  = "image/jpeg";
    types[".js"]   = "text/javascript";
    types[".json"] = "application/json";
    types[".jsonld"] = "application/ld+json";
    types[".mid"]    = "audio/midi, audio/x-midi";
    types[".midi"]   = "audio/midi, audio/x-midi";
    types[".mjs"]    = "text/javascript";
    types[".mp3"]    = "audio/mpeg";
    types[".mp4"]    = "video/mp4";
    types[".mpeg"]   = "video/mpeg";
    types[".mpkg"]   = "application/vnd.apple.installer+xml";
    types[".odp"]    = "application/vnd.oasis.opendocument.presentation";
    types[".ods"]    = "application/vnd.oasis.opendocument.spreadsheet";
    types[".odt"]    = "application/vnd.oasis.opendocument.text";
    types[".oga"]    = "audio/ogg";
    types[".ogv"]    = "video/ogg";
    types[".ogx"]    = "application/ogg";
    types[".opus"]   = "audio/opus";
    types[".otf"]    = "font/otf";
    types[".png"]    = "image/png";
    types[".pdf"]    = "application/pdf";
    types[".php"]    = "application/x-httpd-php";
    types[".ppt"]    = "application/vnd.ms-powerpoint";
    types[".pptx"]
        = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    types[".rar"]   = "application/vnd.rar";
    types[".rtf"]   = "application/rtf";
    types[".sh"]    = "application/x-sh";
    types[".svg"]   = "image/svg+xml";
    types[".tar"]   = "application/x-tar";
    types[".tif"]   = "image/tiff";
    types[".tiff"]  = "image/tiff";
    types[".ts"]    = "video/mp2t";
    types[".ttf"]   = "font/ttf";
    types[".txt"]   = "text/plain";
    types[".vsd"]   = "application/vnd.visio";
    types[".wav"]   = "audio/wav";
    types[".weba"]  = "audio/webm";
    types[".webm"]  = "video/webm";
    types[".webp"]  = "image/webp";
    types[".woff"]  = "font/woff";
    types[".woff2"] = "font/woff2";
    types[".xhtml"] = "application/xhtml+xml";
    types[".xls"]   = "application/vnd.ms-excel";
    types[".xlsx"]  = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    types[".xml"]   = "application/xml";
    types[".xul"]   = "application/vnd.mozilla.xul+xml";
    types[".zip"]   = "application/zip";
    types[".3gp"]   = "video/3gpp; audio/3gpp ";
    types[".3g2"]   = "video/3gpp2; audio/3gpp2";
    types[".7z"]    = "application/x-7z-compressed";
    
    return (types);
}

map_ss setStatus(void)
{
    map_ss statusCodes;

    statusCodes["100"] = "Continue";
    statusCodes["101"] = "Switching Protocols";
    statusCodes["102"] = "Processing";
    statusCodes["103"] = "Early Hints";
    statusCodes["200"] = "OK";
    statusCodes["201"] = "Created";
    statusCodes["202"] = "Accepted";
    statusCodes["203"] = "Non-Authoritative Information";
    statusCodes["204"] = "No Content";
    statusCodes["205"] = "Reset Content";
    statusCodes["206"] = "Partial Content";
    statusCodes["207"] = "Multi-Status";
    statusCodes["208"] = "Already Reported";
    statusCodes["226"] = "IM Used";
    statusCodes["300"] = "Multiple Choices";
    statusCodes["301"] = "Moved Permanently";
    statusCodes["302"] = "Found";
    statusCodes["303"] = "See Other";
    statusCodes["304"] = "Not Modified";
    statusCodes["307"] = "Temporary Redirect";
    statusCodes["308"] = "Permanent Redirect";
    statusCodes["400"] = "Bad Request";
    statusCodes["401"] = "Unauthorized";
    statusCodes["402"] = "Payment Required";
    statusCodes["403"] = "Forbidden";
    statusCodes["404"] = "Not Found";
    statusCodes["405"] = "Method Not Allowed";
    statusCodes["406"] = "Not Acceptable";
    statusCodes["407"] = "Proxy Authentication Required";
    statusCodes["408"] = "Request Timeout";
    statusCodes["409"] = "Conflict";
    statusCodes["410"] = "Gone";
    statusCodes["411"] = "Length Required";
    statusCodes["412"] = "Precondition Failed";
    statusCodes["413"] = "Payload Too Large";
    statusCodes["414"] = "URI Too Long";
    statusCodes["415"] = "Unsupported Media Type";
    statusCodes["416"] = "Range Not Satisfiable";
    statusCodes["417"] = "Expectation Failed";
    statusCodes["418"] = "I'm a teapot";
    statusCodes["421"] = "Misdirected Request";
    statusCodes["422"] = "Unprocessable Content";
    statusCodes["423"] = "Locked";
    statusCodes["424"] = "Failed Dependency";
    statusCodes["425"] = "Too Early";
    statusCodes["426"] = "Upgrade Required";
    statusCodes["428"] = "Precondition Required";
    statusCodes["429"] = "Too Many Requests";
    statusCodes["431"] = "Request Header Fields Too Large";
    statusCodes["451"] = "Unavailable For Legal Reasons";
    statusCodes["500"] = "Internal Server Error";
    statusCodes["501"] = "Not Implemented";
    statusCodes["502"] = "Bad Gateway";
    statusCodes["503"] = "Service Unavailable";
    statusCodes["504"] = "Gateway Timeout";
    statusCodes["505"] = "HTTP Version Not Supported";
    statusCodes["506"] = "Variant Also Negotiates";
    statusCodes["507"] = "Insufficient Storage";
    statusCodes["508"] = "Loop Detected";
    statusCodes["510"] = "Not Extended";
    statusCodes["511"] = "Network Authentication Required";
    
    return (statusCodes);
}
