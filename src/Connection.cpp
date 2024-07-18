//
// Connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "Connection.hpp"
#include <utility>
#include <vector>
#include "ConnectionManager.hpp"
#include "RequestHandler.hpp"


namespace http
{
    namespace server
    {

        Connection::Connection(boost::asio::ip::tcp::socket socket,
                               ConnectionManager &manager, RequestHandler &handler)
            : _socket(std::move(socket)),
              _connection_manager(manager),
              _request_handler(handler)
        {
        }

        void Connection::start()
        {
            do_read();
        }

        void Connection::stop()
        {
            _socket.close();
        }

        void Connection::do_read()
        {
            std::shared_ptr<Connection> self(shared_from_this());
            _socket.async_read_some(boost::asio::buffer(_buffer),
                [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
                {
                    if (!ec)
                    {
                        RequestParser::ResultType result;
                        std::tie(result, std::ignore) = _request_parser.parse(
                            _request, _buffer.data(), _buffer.data() + bytes_transferred);

                        switch (result)
                        {
                        case RequestParser::good:
                            _request_handler.handle_request(_request, _reply);
                            do_write();
                            break;
                        case RequestParser::bad:
                            _reply = Reply::stock_reply(Reply::bad_request);
                            do_write();
                            break;
                        default:
                            do_read();
                        }
                    }
                    else if (ec != boost::asio::error::operation_aborted)
                    {
                        _connection_manager.stop(shared_from_this());
                    }
                });
        }

        void Connection::do_write()
        {
            std::shared_ptr<Connection> self(shared_from_this());
            boost::asio::async_write(_socket, _reply.to_buffers(),
                [this, self](boost::system::error_code ec, std::size_t)
                {
                    if (!ec)
                    {
                        // Initiate graceful Connection closure.
                        boost::system::error_code ignored_ec;
                        _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                                        ignored_ec);
                    }

                    if (ec != boost::asio::error::operation_aborted)
                    {
                        _connection_manager.stop(shared_from_this());
                    }
                });
        }

    } // namespace server
} // namespace http