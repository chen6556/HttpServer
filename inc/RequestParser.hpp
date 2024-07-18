#pragma once

#include <tuple>
#include <string>


namespace http
{
    namespace server
    {

        struct Request;

        /// Parser for incoming requests.
        class RequestParser
        {
        private:
            /// The current state of the parser.
            enum State
            {
                method_start,
                method,
                url,
                http_version_h,
                http_version_t_1,
                http_version_t_2,
                http_version_p,
                http_version_slash,
                http_version_major_start,
                http_version_major,
                http_version_minor_start,
                http_version_minor,
                expecting_newline_1,
                header_line_start,
                header_lws,
                header_name,
                space_before_header_value,
                header_value,
                expecting_newline_2,
                expecting_newline_3,
                content
            } _state;

            size_t _content_length = 0;

        public:
            /// Construct ready to parse the request method.
            RequestParser();

            /// Reset to initial parser state.
            void reset();

            /// Result of parse.
            enum ResultType
            {
                good,
                bad,
                indeterminate
            };

            /// Parse some data. The enum return value is good when a complete request has
            /// been parsed, bad if the data is invalid, indeterminate when more data is
            /// required. The InputIterator return value indicates how much of the input
            /// has been consumed.
            template <typename InputIterator>
            std::tuple<ResultType, InputIterator> parse(Request &req, InputIterator begin, InputIterator end)
            {
                while (begin != end)
                {
                    ResultType result = consume(req, *begin++);
                    if (result == good || result == bad)
                    {
                        return std::make_tuple(result, begin);
                    }
                }
                return std::make_tuple(indeterminate, begin);
            }

        private:
            /// Handle the next character of input.
            ResultType consume(Request &req, char input);

            /// Check if a byte is an HTTP character.
            static bool is_char(int c);

            /// Check if a byte is an HTTP control character.
            static bool is_ctl(int c);

            /// Check if a byte is defined as an HTTP tspecial character.
            static bool is_tspecial(int c);

            /// Check if a byte is a digit.
            static bool is_digit(int c);

            static size_t get_content_length(const Request &req);
        };

    } // namespace server
} // namespace http