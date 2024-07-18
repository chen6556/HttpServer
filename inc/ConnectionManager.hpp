#pragma once

#include <set>
#include "Connection.hpp"


namespace http
{
    namespace server
    {

        /// Manages open connections so that they may be cleanly stopped when the server
        /// needs to shut down.
        class ConnectionManager
        {
        private:
            /// The managed connections.
            std::set<Connection_ptr> _connections;

        public:
            ConnectionManager(const ConnectionManager &) = delete;
            ConnectionManager &operator=(const ConnectionManager &) = delete;

            /// Construct a connection manager.
            ConnectionManager();

            /// Add the specified connection to the manager and start it.
            void start(Connection_ptr c);

            /// Stop the specified connection.
            void stop(Connection_ptr c);

            /// Stop all connections.
            void stop_all();
        };

    } // namespace server
} // namespace http