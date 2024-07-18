#include <sstream>
#include "RequestParser.hpp"
#include "Request.hpp"


namespace http
{
    namespace server
    {

        RequestParser::RequestParser()
            : _state(method_start)
        {
        }

        void RequestParser::reset()
        {
            _state = method_start;
        }

        RequestParser::ResultType RequestParser::consume(Request &req, char input)
        {
            switch (_state)
            {
            case method_start:
                if (!is_char(input) || is_ctl(input) || is_tspecial(input))
                {
                    return bad;
                }
                else
                {
                    _state = method;
                    req.method.push_back(input);
                    return indeterminate;
                }
            case method:
                if (input == ' ')
                {
                    switch (req.method.front())
                    {
                    case 'G':
                        req.method_code = Request::GET_method;
                        break;
                    case 'H':
                        req.method_code = Request::HEAD_method;
                        break;
                    case 'D':
                        req.method_code = Request::DELETE_method;
                        break;
                    case 'C':
                        req.method_code = Request::CONNECT_method;
                        break;
                    case 'O':
                        req.method_code = Request::OPTIONS_method;
                        break;
                    case 'T':
                        req.method_code = Request::TRACE_method;
                        break;
                    default:
                        if (req.method == "POST")
                        {
                            req.method_code = Request::POST_method;
                        }
                        else if (req.method == "PUT")
                        {
                            req.method_code = Request::PUT_method;
                        }
                        break;
                    }
                    _state = url;
                    return indeterminate;
                }
                else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
                {
                    return bad;
                }
                else
                {
                    req.method.push_back(input);
                    return indeterminate;
                }
            case url:
                if (input == ' ')
                {
                    _state = http_version_h;
                    return indeterminate;
                }
                else if (is_ctl(input))
                {
                    return bad;
                }
                else
                {
                    req.uri.push_back(input);
                    return indeterminate;
                }
            case http_version_h:
                if (input == 'H')
                {
                    _state = http_version_t_1;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case http_version_t_1:
                if (input == 'T')
                {
                    _state = http_version_t_2;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case http_version_t_2:
                if (input == 'T')
                {
                    _state = http_version_p;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case http_version_p:
                if (input == 'P')
                {
                    _state = http_version_slash;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case http_version_slash:
                if (input == '/')
                {
                    req.http_version_major = 0;
                    req.http_version_minor = 0;
                    _state = http_version_major_start;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case http_version_major_start:
                if (is_digit(input))
                {
                    req.http_version_major = req.http_version_major * 10 + input - '0';
                    _state = http_version_major;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case http_version_major:
                if (input == '.')
                {
                    _state = http_version_minor_start;
                    return indeterminate;
                }
                else if (is_digit(input))
                {
                    req.http_version_major = req.http_version_major * 10 + input - '0';
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case http_version_minor_start:
                if (is_digit(input))
                {
                    req.http_version_minor = req.http_version_minor * 10 + input - '0';
                    _state = http_version_minor;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case http_version_minor:
                if (input == '\r')
                {
                    _state = expecting_newline_1;
                    return indeterminate;
                }
                else if (is_digit(input))
                {
                    req.http_version_minor = req.http_version_minor * 10 + input - '0';
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case expecting_newline_1:
                if (input == '\n')
                {
                    _state = header_line_start;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case header_line_start:
                if (input == '\r')
                {
                    _state = expecting_newline_3;
                    return indeterminate;
                }
                else if (!req.headers.empty() && (input == ' ' || input == '\t'))
                {
                    _state = header_lws;
                    return indeterminate;
                }
                else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
                {
                    return bad;
                }
                else
                {
                    req.headers.emplace_back(Header());
                    req.headers.back().name.push_back(input);
                    _state = header_name;
                    return indeterminate;
                }
            case header_lws:
                if (input == '\r')
                {
                    _state = expecting_newline_2;
                    return indeterminate;
                }
                else if (input == ' ' || input == '\t')
                {
                    return indeterminate;
                }
                else if (is_ctl(input))
                {
                    return bad;
                }
                else
                {
                    _state = header_value;
                    req.headers.back().value.push_back(input);
                    return indeterminate;
                }
            case header_name:
                if (input == ':')
                {
                    _state = space_before_header_value;
                    return indeterminate;
                }
                else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
                {
                    return bad;
                }
                else
                {
                    req.headers.back().name.push_back(input);
                    return indeterminate;
                }
            case space_before_header_value:
                if (input == ' ')
                {
                    _state = header_value;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case header_value:
                if (input == '\r')
                {
                    _state = expecting_newline_2;
                    return indeterminate;
                }
                else if (is_ctl(input))
                {
                    return bad;
                }
                else
                {
                    req.headers.back().value.push_back(input);
                    return indeterminate;
                }
            case expecting_newline_2:
                if (input == '\n')
                {
                    _state = header_line_start;
                    return indeterminate;
                }
                else
                {
                    return bad;
                }
            case expecting_newline_3:
                _state = content;
                _content_length = get_content_length(req);
                if (_content_length > 0)
                {
                    return indeterminate;
                }
                else
                {
                    return (input == '\n') ? good : bad;
                }
            case content:
                req.content.push_back(input);
                return --_content_length > 0 ? indeterminate : good;
            default:
                return bad;
            }
        }

        bool RequestParser::is_char(int c)
        {
            return c >= 0 && c <= 127;
        }

        bool RequestParser::is_ctl(int c)
        {
            return (c >= 0 && c <= 31) || (c == 127);
        }

        bool RequestParser::is_tspecial(int c)
        {
            switch (c)
            {
            case '(':
            case ')':
            case '<':
            case '>':
            case '@':
            case ',':
            case ';':
            case ':':
            case '\\':
            case '"':
            case '/':
            case '[':
            case ']':
            case '?':
            case '=':
            case '{':
            case '}':
            case ' ':
            case '\t':
                return true;
            default:
                return false;
            }
        }

        bool RequestParser::is_digit(int c)
        {
            return c >= '0' && c <= '9';
        }

        size_t RequestParser::get_content_length(const Request &req)
        {
            for (const Header &header : req.headers)
            {
                if (header.name == "Content-Length")
                {
                    return std::stoull(header.value);
                }
            }
            return 0;
        }

    } // namespace server
} // namespace http