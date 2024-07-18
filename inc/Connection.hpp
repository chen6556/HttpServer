#pragma once

#include <array>
#include <memory>
#include <boost/asio.hpp>
#include "Reply.hpp"
#include "Request.hpp"
#include "RequestHandler.hpp"
#include "RequestParser.hpp"


namespace http
{
    namespace server
    {

        class ConnectionManager;

        /// Represents a single Connection from a client.
        class Connection : public std::enable_shared_from_this<Connection>
        {
        private:
            /// Socket for the Connection.
            boost::asio::ip::tcp::socket _socket;

            /// The manager for this Connection.
            ConnectionManager &_connection_manager;

            /// The handler used to process the incoming request.
            RequestHandler &_request_handler;

            /// Buffer for incoming data.
            std::array<char, 8192> _buffer;

            /// The incoming request.
            Request _request;

            /// The parser for the incoming request.
            RequestParser _request_parser;

            /// The reply to be sent back to the client.
            Reply _reply;

        public:
            Connection(const Connection &) = delete;
            Connection &operator=(const Connection &) = delete;

            /// Construct a Connection with the given socket.
            explicit Connection(boost::asio::ip::tcp::socket socket,
                                ConnectionManager &manager, RequestHandler &handler);

            /// Start the first asynchronous operation for the Connection.
            void start();

            /// Stop all asynchronous operations associated with the Connection.
            void stop();

        private:
            /// Perform an asynchronous read operation.
            void do_read();

            /// Perform an asynchronous write operation.
            void do_write();
        };

        typedef std::shared_ptr<Connection> Connection_ptr;

    } // namespace server
} // namespace http