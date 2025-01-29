#include "proxy.h"
#include <nghttp2/asio_http2_server.h>

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

class ProxyListener {
public:
  ProxyListener(boost::asio::io_service& io, const std::string& address, uint16_t port)
    : io_(io), acceptor_(io) {
    
    boost::asio::ip::tcp::endpoint ep(
      boost::asio::ip::make_address(address), port);
    
    acceptor_.open(ep.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(ep);
    acceptor_.listen();
    
    start_accept();
  }

private:
  void start_accept() {
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_);
    
    acceptor_.async_accept(*socket, [this, socket](const boost::system::error_code& ec) {
      if (!ec) {
        handle_connection(socket);
      }
      start_accept();
    });
  }

  void handle_connection(std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
    auto session = std::make_shared<Http2Session>(io_, config_);
    
    boost::system::error_code ec;
    session->handle(*socket, ec);
    
    if (ec) {
      std::cerr << "HTTP/2 session error: " << ec.message() << std::endl;
    }
  }

  boost::asio::io_service& io_;
  boost::asio::ip::tcp::acceptor acceptor_;
  ProxyConfig config_;
};
