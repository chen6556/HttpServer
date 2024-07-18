#include <fstream>
#include <sstream>
#include <string>
#include <boost/json.hpp>
#include "RequestHandler.hpp"
#include "MIMETypes.hpp"
#include "Reply.hpp"
#include "Request.hpp"


namespace http
{
    namespace server
    {

        RequestHandler::RequestHandler(const std::string &doc_root)
            : _doc_root(doc_root)
        {
        }

        void RequestHandler::handle_request(const Request &req, Reply &rep)
        {
            // Decode url to path.
            std::string request_path;
            std::map<const std::string, std::string> request_data;
            if (!url_decode(req.uri, request_path, request_data))
            {
                rep = Reply::stock_reply(Reply::bad_request);
                return;
            }

            // Request path must be absolute and not contain "..".
            if (request_path.empty() || request_path[0] != '/' || request_path.find("..") != std::string::npos)
            {
                rep = Reply::stock_reply(Reply::bad_request);
                return;
            }


            // If path ends in slash (i.e. is a directory) then add "index.html".
            if (request_path[request_path.size() - 1] == '/')
            {
                request_path.append("index.html");
            }

            // Determine the file extension.
            std::size_t last_slash_pos = request_path.find_last_of("/");
            std::size_t last_dot_pos = request_path.find_last_of(".");
            std::string extension;
            if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
            {
                extension = request_path.substr(last_dot_pos + 1);
            }

            // Open the file to send back.
            std::string full_path = _doc_root + request_path;
            std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
            if (!is)
            {
                rep = Reply::stock_reply(Reply::not_found);
                return;
            }

            // Fill out the reply to be sent to the client.
            rep.status = Reply::ok;

            if (req.content.find("content") == std::string::npos)
            {
                char buf[512];
                while (is.read(buf, sizeof(buf)).gcount() > 0)
                {
                    rep.content.append(buf, is.gcount());
                }
                rep.headers.resize(2);
                rep.headers[0].name = "Content-Length";
                rep.headers[0].value = std::to_string(rep.content.size());
                rep.headers[1].name = "Content-Type";
                rep.headers[1].value = mime_types::extension_to_type(extension);
            }
            else
            {
                boost::json::value value = boost::json::parse(req.content);
                rep.headers.resize(2);
                rep.headers[0].name = "Content-Length";
                rep.headers[0].value = std::to_string(rep.content.size());
                rep.headers[1].name = "Content-Type";
                rep.headers[1].value = mime_types::extension_to_type(extension);
            }
        }

        bool RequestHandler::url_decode(const std::string &in, std::string &out, std::map<const std::string, std::string> &request_data)
        {
            out.clear();
            out.reserve(in.size());
            for (std::size_t i = 0, count = in.size(); i < count; ++i)
            {
                if (in[i] == '%')
                {
                    if (i + 3 <= in.size())
                    {
                        int value = 0;
                        std::istringstream is(in.substr(i + 1, 2));
                        if (is >> std::hex >> value)
                        {
                            out += static_cast<char>(value);
                            i += 2;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
                else if (in[i] == '+')
                {
                    out += ' ';
                }
                else
                {
                    out += in[i];
                }
            }

            const size_t pos = out.find_first_of('?');
            if (pos == std::string::npos)
            {
                return true;
            }

            bool is_key = true;
            std::string key, value;
            for (size_t i = pos + 1, count = out.size(); i < count; ++i)
            {
                switch (out.at(i))
                {
                case '=':
                    is_key = false;
                    break;
                case '&':
                    is_key = true;
                    request_data[key] = value.empty() ? key : value;
                    key.clear();
                    value.clear();
                    break;
                default:
                    if (is_key)
                    {
                        key.push_back(out.at(i));
                    }
                    else
                    {
                        value.push_back(out.at(i));
                    }
                }
            }
            if (!key.empty())
            {
                request_data[key] = key;
            }
            out.erase(out.cbegin() + pos, out.cend());

            return true;
        }

    } // namespace server
} // namespace http