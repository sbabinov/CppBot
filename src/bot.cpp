#include "bot.hpp"
#include <iostream>
#include "types.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

bot::Bot::Bot(const std::string& token, handlers::Handler mh):
  token_(token),
  mh_(&mh),
  sslContext_(asio::ssl::context::tlsv12_client)
{
  sslContext_.set_default_verify_paths();
}

void bot::Bot::runIoContext()
{
  asio::executor_work_guard< asio::io_context::executor_type > work = asio::make_work_guard(ioContext_);
  ioContext_.run();
}

void bot::Bot::fetchUpdates()
{
  std::size_t lastUpdateId = 0;
  while (isRunning_)
  {
    std::string host = "api.telegram.org";
    std::string path = "/bot" + token_ + "/getUpdates?offset=" + std::to_string(lastUpdateId + 1);

    asio::ip::tcp::resolver resolver(ioContext_);
    asio::ssl::stream<asio::ip::tcp::socket> socket(ioContext_, sslContext_);

    try
    {
      if (!SSL_set_tlsext_host_name(socket.native_handle(), host.c_str()))
      {
        throw boost::system::system_error(
          ::ERR_get_error(),
          asio::error::get_ssl_category()
        );
      }

      auto const results = resolver.resolve(host, "443");
      asio::connect(socket.next_layer(), results.begin(), results.end());

      socket.handshake(asio::ssl::stream_base::client);

      http::request< http::string_body > req{http::verb::get, path, 11};
      req.set(http::field::host, host);
      req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
      http::write(socket, req);

      beast::flat_buffer buffer;
      http::response< http::string_body > res;
      http::read(socket, buffer, res);
      auto updates = nlohmann::json::parse(res.body());
      for (const auto& update : updates["result"]) {
        lastUpdateId = update["update_id"];
        if (update.contains("message"))
        {
          std::lock_guard< std::mutex > lock(queueMutex_);
          messageQueue_.push(types::Message(update["message"]));
          queueCondition_.notify_one();
        }
      }
    }
    catch (std::exception const& e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
