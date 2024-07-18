#include "ConnectionManager.hpp"


namespace http
{
    namespace server
    {

        ConnectionManager::ConnectionManager()
        {
        }

        void ConnectionManager::start(Connection_ptr c)
        {
            _connections.insert(c);
            c->start();
        }

        void ConnectionManager::stop(Connection_ptr c)
        {
            _connections.erase(c);
            c->stop();
        }

        void ConnectionManager::stop_all()
        {
            for (Connection_ptr c : _connections)
            {
                c->stop();
            }
            _connections.clear();
        }

    } // namespace server
} // namespace http