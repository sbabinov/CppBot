/*!
  @file
  @brief Main header of CppBot library.
  @author sbabinov92
  @version 1.0
  @date April 2025
  @warning The project is still in development

  This file contains Bot class and all it's methods.
*/

#ifndef CPPBOT_HPP
#define CPPBOT_HPP

#include <iostream>
#include <future>
#include <thread>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <utility>

#include <nlohmann/json.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>

#include "types.hpp"
#include "handlers.hpp"
#include "states.hpp"

namespace asio = boost::asio;
namespace http = boost::beast::http;

namespace cppbot
{
  /*!
    @brief Main class of library.
  */
  class Bot
  {
   public:
    using futureMessage = std::future< types::Message >;
    using futureFile = std::future< types::File >;
    using futureBool = std::future< bool >;

    /*!
      @param token Unique token for telegram bot
      @param mh Shared pointer to MessageHandler
      @param qh Shared pointer to CallbackQueryHandler
      @param storage Shared pointer to Storage
    */
    Bot(const std::string& token, std::shared_ptr< handlers::MessageHandler > mh,
      std::shared_ptr< handlers::CallbackQueryHandler > qh, std::shared_ptr< states::Storage > storage);

    /*!
      @brief Method for starting polling.
    */
    void startPolling();

    /*!
      @brief Method for stopping polling.
    */
    void stop();

    /*!
      @brief Async method for sending text messages.
      @param chatId Chat id
      @param text Text for sending
      @param replyMarkup Keyboard for sending with message
      @return std::future< types::Message >
    */
    futureMessage sendMessage           (size_t chatId, const std::string& text,
      const types::Keyboard& replyMarkup = {});

    /// @example message_sending.cpp

    /*!
      @brief Async method for deleting messages.
      @param chatId Chat id
      @param messageId Message id
      @return std::future< bool >
    */
    futureBool    deleteMessage        (size_t chatId, size_t messageId);

    /*!
      @brief Async method for answer callback queries.
      @param queryId Query id
      @param text Text for answer
      @param showAlert Allows to show alert with {text} to user
      @return std::future< bool >
    */
    futureBool    answerCallbackQuery  (const std::string& queryId, const std::string& text = "",
      bool showAlert = false, const std::string& url = "", size_t cacheTime = 0);

    /// @example queries.cpp

    /*!
      @brief Async method for sending photos.
      @param chatId Chat id
      @param photo Photo for sending
      @param caption Caption for photo
      @param replyMarkup Keyboard for sending with photo
      @param hasSpoiler Allows to send photo with spoiler
      @return std::future< types::Message >
    */
    futureMessage sendPhoto             (size_t chatId, const types::InputFile& photo, const std::string& caption = "",
      const types::InlineKeyboardMarkup& replyMarkup = {}, bool hasSpoiler = false);

    /*!
      @brief Async method for sending documents.
      @param chatId Chat id
      @param document Document for sending
      @param caption Caption for document
      @param replyMarkup Keyboard for sending with document
      @return std::future< types::Message >
    */
    futureMessage sendDocument          (size_t chatId, const types::InputFile& document, const std::string& caption = "",
      const types::InlineKeyboardMarkup& replyMarkup = {});

    /*!
      @brief Async method for sending audio.
      @param chatId Chat id
      @param photo Audio for sending
      @param caption Caption for audio
      @param replyMarkup Keyboard for sending with audio
      @return std::future< types::Message >
    */
    futureMessage sendAudio             (size_t chatId, const types::InputFile& audio, const std::string& caption = "",
      const types::InlineKeyboardMarkup& replyMarkup = {});

    /*!
      @brief Async method for sending videos.
      @param chatId Chat id
      @param photo Video for sending
      @param caption Caption for video
      @param replyMarkup Keyboard for sending with video
      @param hasSpoiler Allows to send video with spoiler
      @return std::future< types::Message >
    */
    futureMessage sendVideo             (size_t chatId, const types::InputFile& video, const std::string& caption = "",
      const types::InlineKeyboardMarkup& replyMarkup = {}, bool hasSpoiler = false);

    /*!
      @brief Async method for editing message text.
      @param chatId Chat id
      @param messageId Message id
      @param text New text
      @return std::future< types::Message >
    */
    futureMessage editMessageText       (size_t chatId, size_t messageId, const std::string& text,
      const types::InlineKeyboardMarkup& replyMarkup = {});

    /*!
      @brief Async method for editing message caption.
      @param chatId Chat id
      @param messageId Message id
      @param caption New caption
      @param replyMarkup Keyboard for this message
      @return std::future< types::Message >
    */
    futureMessage editMessageCaption    (size_t chatId, size_t messageId, const std::string& caption,
      const types::InlineKeyboardMarkup& replyMarkup = {});

    /*!
      @brief Async method for editing message media.
      @param chatId Chat id
      @param messageId Message id
      @param media New media
      @param replyMarkup Keyboard for this message
      @return std::future< types::Message >
    */
    futureMessage editMessageMedia      (size_t chatId, size_t messageId, const types::InputMedia& media,
      const types::InlineKeyboardMarkup& replyMarkup = {});

    /*!
      @brief Async method for editing message keyboard.
      @param chatId Chat id
      @param messageId Message id
      @param replyMarkup New keyboard
      @return std::future< types::Message >
    */
    futureMessage editMessageReplyMarkup(size_t chatId, size_t messageId,
      const types::InlineKeyboardMarkup& replyMarkup);

    /*!
      @brief Async method for getting file info.
      @param fileId File id
      @return std::future< types::Message >
    */
    futureFile    getFile               (const std::string& fileId);

    /*!
      @brief Method for getting .
      @param fileId File id
      @return std::future< types::Message >
    */
    states::StateContext getStateContext(size_t chatId);
   private:
    std::string token_;
    std::shared_ptr< handlers::MessageHandler > mh_;
    std::shared_ptr< handlers::CallbackQueryHandler > qh_;
    asio::io_context ioContext_;
    asio::ssl::context sslContext_;
    std::thread ioThread_;
    std::queue< types::Message > messageQueue_;
    std::queue< types::CallbackQuery > queryQueue_;
    std::mutex updateMutex_;
    std::condition_variable updateCondition_;
    states::StateMachine stateMachine_;
    bool isRunning_;

    void runIoContext();
    void fetchUpdates();
    void processUpdates();

    futureMessage sendFile(const types::InputFile& file, const std::string& fileType,
      const nlohmann::json& fields);
    futureMessage updateFile(const types::InputMedia& media, const nlohmann::json& fields);

    void printError(const std::string& errorMessage) const;

    struct RequestData
    {
      std::string body;
      std::string endpoint;
      std::vector< std::pair< http::field, std::string > > additionalHeaders;
      std::string contentType;
      std::shared_ptr< asio::ip::tcp::resolver > resolver;
      std::shared_ptr < asio::ssl::stream<asio::ip::tcp::socket> > socket;
      std::shared_ptr< http::request< http::string_body > > req;
      std::shared_ptr< http::response<http::string_body> > res;
      std::shared_ptr< boost::beast::flat_buffer > buffer;
    };

    template< typename T >
    std::future< T > sendRequest(const std::string& body, const std::string& endpoint,
      const std::vector< std::pair< http::field, std::string > >& additionalHeaders = {},
      const std::string& contentType = "application/json")
    {
      auto data = std::make_shared< RequestData >();

      data->body = body;
      data->endpoint = endpoint;
      data->additionalHeaders = additionalHeaders;
      data->contentType = contentType;
      data->resolver = std::make_shared< asio::ip::tcp::resolver >(ioContext_);
      data->socket = std::make_shared< asio::ssl::stream< asio::ip::tcp::socket > >(ioContext_, sslContext_);

      auto promise = std::make_shared< std::promise< T > >();
      std::future< T > future = promise->get_future();

      if (!SSL_set_tlsext_host_name(data->socket->native_handle(), "api.telegram.org"))
      {
        printError("Problems with SSL");
        promise->set_exception(std::make_exception_ptr(std::runtime_error("Correct message wasn't received")));
      }
      data->resolver->async_resolve("api.telegram.org", "443",
        [this, data, promise](auto ec, auto endpoints)
        {
          if (ec)
          {
            printError(ec.message());
            promise->set_exception(std::make_exception_ptr(std::runtime_error("Correct message wasn't received")));
            return;
          }
          async_connect(data->socket->next_layer(), endpoints, [this, data, promise](auto ec, auto)
          {
            if (ec)
            {
              printError(ec.message());
              promise->set_exception(std::make_exception_ptr(std::runtime_error("Correct message wasn't received")));
              return;
            }
            data->socket->async_handshake(asio::ssl::stream_base::client, [this, data, promise](auto ec)
            {
              if (ec)
              {
                printError(ec.message());
                promise->set_exception(std::make_exception_ptr(std::runtime_error("Correct message wasn't received")));
                return;
              }
              std::string host = "api.telegram.org";
              std::string path = "/bot" + token_ + data->endpoint;
              auto req = std::make_shared< http::request< http::string_body > >(http::verb::post, path, 11);
              req->set(http::field::host, host);
              req->set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
              req->set(http::field::content_type, data->contentType);
              for (const auto& header : data->additionalHeaders)
              {
                req->set(header.first, header.second);
              }
              req->body() = data->body;
              req->prepare_payload();
              data->req = req;
              http::async_write(*(data->socket), *(data->req), [this, data, promise](auto ec, auto)
              {
                if (ec)
                {
                  printError(ec.message());
                  promise->set_exception(std::make_exception_ptr(std::runtime_error("Correct message wasn't received")));
                  return;
                }
                auto buffer = std::make_shared< boost::beast::flat_buffer >();
                auto res = std::make_shared< http::response< http::string_body > >();
                data->buffer = buffer;
                data->res = res;
                http::async_read(*(data->socket), *(data->buffer), *(data->res), [this, promise, data](auto ec, auto)
                {
                  if (ec)
                  {
                    printError(ec.message());
                    promise->set_exception(std::make_exception_ptr(std::runtime_error("Correct message wasn't received")));
                    return;
                  }
                  nlohmann::json response = nlohmann::json::parse(data->res->body());
                  if (!response["ok"])
                  {
                    printError(response["description"].dump());
                    promise->set_exception(std::make_exception_ptr(std::runtime_error("Correct message wasn't received")));
                    return;
                  }
                  promise->set_value(response["result"].template get< T >());
                });
              });
            });
          });
        });
      return future;
    }
  };
}

#endif
