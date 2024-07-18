#pragma once

#include <boost/asio.hpp>
#include <string>
#include "Connection.hpp"
#include "ConnectionManager.hpp"
#include "RequestHandler.hpp"

namespace http
{
    namespace server
    {

        /// The top-level class of the HTTP Server.
        class Server
        {
        private:
            /// The io_context used to perform asynchronous operations.
            boost::asio::io_context _io_context;

            /// The signal_set is used to register for process termination notifications.
            boost::asio::signal_set _signals;

            /// Acceptor used to listen for incoming connections.
            boost::asio::ip::tcp::acceptor _acceptor;

            /// The connection manager which owns all live connections.
            ConnectionManager _connection_manager;

            /// The handler for all incoming requests.
            RequestHandler _request_handler;

        public:
            Server(const Server &) = delete;
            Server &operator=(const Server &) = delete;

            /// Construct the Server to listen on the specified TCP address and port, and
            /// serve up files from the given directory.
            explicit Server(const std::string &address, const std::string &port,
                            const std::string &doc_root);

            /// Run the Server's io_context loop.
            void run();

        private:
            /// Perform an asynchronous accept operation.
            void do_accept();

            /// Wait for a request to stop the Server.
            void do_await_stop();
        };

    } // namespace Server
} // namespace http