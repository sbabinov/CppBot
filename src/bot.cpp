#include "bot.hpp"
#include <iostream>
#include <functional>
#include <future>
#include <utility>
#include "types.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

cppbot::Bot::Bot(const std::string& token, std::shared_ptr< handlers::MessageHandler > mh,
 std::shared_ptr< states::Storage > storage):
  token_(token),
  mh_(mh),
  sslContext_(asio::ssl::context::tlsv12_client),
  stateMachine_(storage)
{
  sslContext_.set_default_verify_paths();
}

void cppbot::Bot::start()
{
  isRunning_ = true;
  ioThread_ = std::thread(&cppbot::Bot::runIoContext, this);
  std::thread(std::bind(&cppbot::Bot::fetchUpdates, this)).detach();
  std::thread(std::bind(&cppbot::Bot::processCallbackQueries, this)).detach();
  processMessages();
}

types::Message cppbot::Bot::sendMessage(size_t chatId, const std::string& text, const types::InlineKeyboardMarkup replyMarkup)
{
  std::promise< types::Message > promise;
  std::future< types::Message > msg = promise.get_future();
  asio::post(ioContext_, [this, chatId, &replyMarkup, &text, &promise]
  {
    std::string host = "api.telegram.org";
    std::string path = "/bot" + token_ + "/sendMessage";
    nlohmann::json body = {
      {"chat_id", chatId},
      {"text", text}
    };
    if (!replyMarkup.keyboard.empty())
    {
      body["reply_markup"] = replyMarkup;
    }

    asio::ip::tcp::resolver resolver(ioContext_);
    asio::ssl::stream< asio::ip::tcp::socket > socket(ioContext_, sslContext_);

    try
    {
      if (!SSL_set_tlsext_host_name(socket.native_handle(), host.c_str()))
      {
        throw boost::system::system_error(::ERR_get_error(), asio::error::get_ssl_category());
      }

      auto const endpoints = resolver.resolve(host, "443");
      asio::connect(socket.next_layer(), endpoints.begin(), endpoints.end());

      socket.handshake(asio::ssl::stream_base::client);

      http::request< http::string_body > req{http::verb::post, path, 11};
      req.set(http::field::host, host);
      req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
      req.set(http::field::content_type, "application/json");
      req.body() = body.dump();
      req.prepare_payload();

      http::write(socket, req);

      beast::flat_buffer buffer;
      http::response< http::string_body > res;
      http::read(socket, buffer, res);
      promise.set_value(nlohmann::json::parse(res.body())["result"].template get< types::Message >());
    }
    catch (std::exception const& e)
    {
      std::cerr << "Error: " << e.what() << std::endl;
      promise.set_value(types::Message());
    }
  });
  return msg.get();
}

types::Message cppbot::Bot::editMessageText(size_t chatId, size_t messageId, const std::string& text)
{
  std::promise< types::Message > promise;
  std::future< types::Message > msg = promise.get_future();
  asio::post(ioContext_, [this, chatId, messageId, &text, &promise]
  {
    std::string host = "api.telegram.org";
    std::string path = "/bot" + token_ + "/editMessageText";
    nlohmann::json body = {
      {"chat_id", chatId},
      {"message_id", messageId},
      {"text", text}
    };
    asio::ip::tcp::resolver resolver(ioContext_);
    asio::ssl::stream< asio::ip::tcp::socket > socket(ioContext_, sslContext_);

    try
    {
      if (!SSL_set_tlsext_host_name(socket.native_handle(), host.c_str()))
      {
        throw boost::system::system_error(::ERR_get_error(), asio::error::get_ssl_category());
      }

      auto const endpoints = resolver.resolve(host, "443");
      asio::connect(socket.next_layer(), endpoints.begin(), endpoints.end());

      socket.handshake(asio::ssl::stream_base::client);

      http::request< http::string_body > req{http::verb::post, path, 11};
      req.set(http::field::host, host);
      req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
      req.set(http::field::content_type, "application/json");
      req.body() = body.dump();
      req.prepare_payload();

      http::write(socket, req);

      beast::flat_buffer buffer;
      http::response< http::string_body > res;
      http::read(socket, buffer, res);
      promise.set_value(nlohmann::json::parse(res.body())["result"].template get< types::Message >());
    }
    catch (std::exception const& e)
    {
      std::cerr << "Error: " << e.what() << std::endl;
      promise.set_value(types::Message());
    }
  });
  return msg.get();
}

void cppbot::Bot::stop()
{
    isRunning_ = false;
    ioContext_.stop();
    if (ioThread_.joinable())
    {
        ioThread_.join();
    }
}

void cppbot::Bot::runIoContext()
{
  asio::executor_work_guard< asio::io_context::executor_type > work = asio::make_work_guard(ioContext_);
  ioContext_.run();
}

void cppbot::Bot::fetchUpdates()
{
  std::size_t lastUpdateId = 0;
  while (isRunning_)
  {
    std::string host = "api.telegram.org";
    std::string path = "/bot" + token_ + "/getUpdates?offset=" + std::to_string(lastUpdateId + 1);

    asio::ip::tcp::resolver resolver(ioContext_);
    asio::ssl::stream< asio::ip::tcp::socket > socket(ioContext_, sslContext_);

    try
    {
      if (!SSL_set_tlsext_host_name(socket.native_handle(), host.c_str()))
      {
        throw boost::system::system_error(::ERR_get_error(), asio::error::get_ssl_category());
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
      for (const auto& update : updates["result"])
      {
        lastUpdateId = update["update_id"];
        if (update.contains("message"))
        {
          std::lock_guard< std::mutex > lock(messageQueueMutex_);
          messageQueue_.push(update["message"].template get< types::Message >());
          messageQueueCondition_.notify_one();
        }
        if (update.contains("callback_query"))
        {
          std::lock_guard< std::mutex > lock(queryQueueMutex_);
          queryQueue_.push(update["callback_query"].template get< types::CallbackQuery >());
          queryQueueCondition_.notify_one();
        }
      }
    }
    catch (std::exception const& e)
    {
      std::cerr << "Error: " << e.what() << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
}

void cppbot::Bot::processMessages()
{
  while (isRunning_)
  {
    std::unique_lock< std::mutex > lock(messageQueueMutex_);
    messageQueueCondition_.wait(lock, [this]
    {
      return !messageQueue_.empty();
    });
    types::Message msg = messageQueue_.front();
    messageQueue_.pop();
    lock.unlock();

    states::StateContext state(msg.chat.id, &stateMachine_);

    try
    {
      (*mh_).processMessage(msg, state);
    }
    catch (const std::exception& e)
    {
      std::cerr << e.what() << '\n';
    }
  }
}

void cppbot::Bot::processCallbackQueries()
{
  while (isRunning_)
  {
    std::unique_lock< std::mutex > lock(queryQueueMutex_);
    queryQueueCondition_.wait(lock, [this]
    {
      return !queryQueue_.empty();
    });
    types::CallbackQuery query = queryQueue_.front();
    queryQueue_.pop();
    lock.unlock();

    try
    {
      (*qh_).processCallbackQuery(query);
    }
    catch (const std::exception& e)
    {
      std::cerr << e.what() << '\n';
    }
  }
}
