#pragma once

#include <string>
#include <boost/json.hpp>
#include <map>


namespace http
{
    namespace server
    {
        namespace json
        {
            boost::json::value read_json(const std::string &file_name);

            std::map<const std::string, std::string> read_json_as_map(const std::string &file_name);
        }
    }
}