#include <fstream>
#include "Json.hpp"


namespace http
{
    namespace server
    {
        namespace json
        {
            boost::json::value read_json(const std::string &file_name)
            {
                boost::json::stream_parser p;
                boost::json::error_code ec;
                std::ifstream f(file_name, std::ifstream::in);
                std::string line;
                do
                {
                    std::getline(f, line);
                    p.write(line.c_str(), line.size(), ec);
                }
                while(f.good());
                f.close();
                
                if (ec)
                {
                    return nullptr;
                }
                
                p.finish(ec);
                if (ec)
                {
                    return nullptr;
                }
                
                return p.release();
            }

            std::map<const std::string, std::string> read_json_as_map(const std::string &file_name)
            {
                boost::json::stream_parser p;
                boost::json::error_code ec;
                std::ifstream f(file_name, std::ifstream::in);
                std::string line;
                do
                {
                    std::getline(f, line);
                    p.write(line.c_str(), line.size(), ec);
                }
                while(f.good());
                f.close();
                
                if (ec)
                {
                    return std::map<const std::string, std::string>();
                }
                
                p.finish(ec);
                if (ec)
                {
                    return std::map<const std::string, std::string>();
                }

                boost::json::value v = p.release();
                if (v.if_object() == nullptr)
                {
                    return std::map<const std::string, std::string>();
                }

                std::map<const std::string, std::string> result;
                for (boost::json::object::iterator it = v.as_object().begin(),
                        end = v.as_object().end(); it != end; ++it)
                {
                    result[it->key_c_str()] = it->value().get_string().c_str();
                }
                return result;
            }
        }
    }
}