#include "bot.hpp"
#include <iostream>
#include <functional>
#include <future>
#include <utility>
#include "types.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

std::string generateBoundary()
{
  return "----CppbotBoundary" + std::to_string(rand());
}

std::string extractFileName(const std::string& path)
{
  return path.substr(path.find_last_of("/\\") + 1);
}

std::string createMultipartBody(const std::string& boundary, nlohmann::json fields,
  const std::string& fileName, const std::string& fileType, const types::InputFile& file)
{
  std::string body = "";

  for (const auto& [field, value] : fields.items())
  {
    body += "--" + boundary + "\r\n";
    body += "Content-Disposition: form-data; name=\"" + field + "\"\r\n\r\n";
    body += value.dump() + "\r\n";
  }

  body += "--" + boundary + "\r\n";
  body += "Content-Disposition: form-data; name=\"" + fileType + "\"; filename=\"" + fileName + "\"\r\n";
  body += "Content-type: application/octet-stream\r\n\r\n";
  body += file.asStringBytes() + "\r\n";

  body += "--" + boundary + "--\r\n";
  return body;
}

cppbot::Bot::Bot(const std::string& token, std::shared_ptr< handlers::MessageHandler > mh,
 std::shared_ptr< handlers::CallbackQueryHandler > qh, std::shared_ptr< states::Storage > storage):
  token_(token),
  mh_(mh),
  qh_(qh),
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
  nlohmann::json body = {
    {"chat_id", chatId},
    {"text", text}
  };
  if (!replyMarkup.keyboard.empty())
  {
    body["reply_markup"] = replyMarkup;
  }
  http::response< http::string_body > response = sendRequest(body.dump(), "/sendMessage");
  return nlohmann::json::parse(response.body())["result"].template get< types::Message >();
}

types::Message cppbot::Bot::editMessageText(size_t chatId, size_t messageId, const std::string& text)
{
  nlohmann::json body = {
    {"chat_id", chatId},
    {"message_id", messageId},
    {"text", text}
  };
  http::response< http::string_body > response = sendRequest(body.dump(), "/editMessageText");
  return nlohmann::json::parse(response.body())["result"].template get< types::Message >();
}

bool cppbot::Bot::deleteMessage(size_t chatId, size_t messageId)
{
  nlohmann::json body = {
    {"chat_id", chatId},
    {"message_id", messageId}
  };
  http::response< http::string_body > response = sendRequest(body.dump(), "/deleteMessage");
  return nlohmann::json::parse(response.body())["result"].template get< bool >();
}

bool cppbot::Bot::answerCallbackQuery(size_t queryId, const std::string& text, bool showAlert,
  const std::string& url, size_t cacheTime)
{
  nlohmann::json body = {
    {"callback_query_id", std::to_string(queryId)},
    {"show_alert", showAlert},
    {"cache_time", cacheTime}
  };
  if (text != "")
  {
    body["text"] = text;
  }
  if (url != "")
  {
    body["url"] = url;
  }
  http::response< http::string_body > response = sendRequest(body.dump(), "/answerCallbackQuery");
  return nlohmann::json::parse(response.body())["result"].template get< bool >();
}

types::Message cppbot::Bot::sendPhoto(size_t chatId, const types::InputFile& photo, const std::string& caption,
  const types::InlineKeyboardMarkup& replyMarkup, bool hasSpoiler)
{
  std::string boundary = generateBoundary();
  nlohmann::json fields;
  fields["chat_id"] = chatId;
  if (caption != "")
  {
    fields["caption"] = caption;
  }
  if (!replyMarkup.keyboard.empty())
  {
    fields["reply_markup"] = replyMarkup;
  }
  return sendFile(chatId, photo, "/sendPhoto", fields);
}

types::Message cppbot::Bot::sendDocument(size_t chatId, const types::InputFile& document,
  const std::string& caption, const types::InlineKeyboardMarkup& replyMarkup)
{
  std::string boundary = generateBoundary();
  nlohmann::json fields;
  fields["chat_id"] = chatId;
  if (caption != "")
  {
    fields["caption"] = caption;
  }
  if (!replyMarkup.keyboard.empty())
  {
    fields["reply_markup"] = replyMarkup;
  }
  return sendFile(chatId, document, "/sendDocument", fields);
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

http::response< http::string_body > cppbot::Bot::sendRequest(const std::string& body, const std::string& endpoint,
  const std::vector< std::pair< http::field, std::string > >& additionalHeaders, const std::string& contentType)
{
  std::promise< http::response< http::string_body > > promise;
  std::future< http::response< http::string_body > > response = promise.get_future();
  asio::post(ioContext_, [this, &body, &endpoint, &additionalHeaders, &contentType, &promise]
  {
    std::string host = "api.telegram.org";
    std::string path = "/bot" + token_ + endpoint;
    asio::ip::tcp::resolver resolver(ioContext_);
    asio::ssl::stream< asio::ip::tcp::socket > socket(ioContext_, sslContext_);

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
    req.set(http::field::content_type, contentType);
    for (const auto& header : additionalHeaders)
    {
      req.set(header.first, header.second);
    }
    req.body() = body;
    req.prepare_payload();

    http::write(socket, req);

    beast::flat_buffer buffer;
    http::response< http::string_body > res;
    http::read(socket, buffer, res);
    promise.set_value(res);
  });
  return response.get();
}

types::Message cppbot::Bot::sendFile(size_t chatId, const types::InputFile& file,
  const std::string& endpoint, const nlohmann::json& fields)
{
  std::string fileType = "";
  if (endpoint == "/sendPhoto")
  {
    fileType = "photo";
  }
  else if (endpoint == "/sendDocument")
  {
    fileType = "document";
  }

  std::string boundary = generateBoundary();

  std::string body = createMultipartBody(boundary, fields, "image.jpg", fileType, file);
  http::response< http::string_body > response = sendRequest(
    body,
    endpoint,
    {{http::field::content_length, std::to_string(body.size())}, {http::field::connection, "close"}},
    "multipart/form-data; boundary=" + boundary
  );
  nlohmann::json jsonResponse = nlohmann::json::parse(response.body());
  if (!jsonResponse["ok"])
  {
    throw std::runtime_error(jsonResponse["description"]);
  }
  return jsonResponse["result"].template get< types::Message >();
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
