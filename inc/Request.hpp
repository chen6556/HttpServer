#pragma once

#include <string>
#include <vector>
#include <map>
#include "Header.hpp"


namespace http
{
    namespace server
    {
        /// A request received from a client.
        struct Request
        {
            enum RequestMethod
            {
                GET_method,
                HEAD_method,
                POST_method,
                PUT_method,
                DELETE_method,
                CONNECT_method,
                OPTIONS_method,
                TRACE_method
            } method_code;
            
            std::string method;
            std::string uri;
            int http_version_major;
            int http_version_minor;            
            std::vector<Header> headers;
            std::string content;
        };

    } // namespace server
} // namespace http