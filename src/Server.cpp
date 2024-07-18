#include <signal.h>
#include <utility>
#include "Server.hpp"


namespace http
{
    namespace server
    {

        Server::Server(const std::string &address, const std::string &port,
                       const std::string &doc_root)
            : _io_context(1),
              _signals(_io_context),
              _acceptor(_io_context),
              _connection_manager(),
              _request_handler(doc_root)
        {
            // Register to handle the signals that indicate when the Server should exit.
            // It is safe to register for the same signal multiple times in a program,
            // provided all registration for the specified signal is made through Asio.
            _signals.add(SIGINT);
            _signals.add(SIGTERM);
        #if defined(SIGQUIT)
            _signals.add(SIGQUIT);
        #endif // defined(SIGQUIT)

            do_await_stop();

            // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
            boost::asio::ip::tcp::resolver resolver(_io_context);
            boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, port).begin();
            _acceptor.open(endpoint.protocol());
            _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            _acceptor.bind(endpoint);
            _acceptor.listen();

            do_accept();
        }

        void Server::run()
        {
            // The io_context::run() call will block until all asynchronous operations
            // have finished. While the Server is running, there is always at least one
            // asynchronous operation outstanding: the asynchronous accept call waiting
            // for new incoming connections.
            _io_context.run();
        }

        void Server::do_accept()
        {
            _acceptor.async_accept(
                [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
                {
                    // Check whether the Server was stopped by a signal before this
                    // completion handler had a chance to run.
                    if (!_acceptor.is_open())
                    {
                        return;
                    }

                    if (!ec)
                    {
                        _connection_manager.start(std::make_shared<Connection>(
                            std::move(socket), _connection_manager, _request_handler));
                    }

                    do_accept();
                });
        }

        void Server::do_await_stop()
        {
            _signals.async_wait(
                [this](boost::system::error_code /*ec*/, int /*signo*/)
                {
                    // The Server is stopped by cancelling all outstanding asynchronous
                    // operations. Once all operations have finished the io_context::run()
                    // call will exit.
                    _acceptor.close();
                    _connection_manager.stop_all();
                });
        }

    } // namespace Server
} // namespace http