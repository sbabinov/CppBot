#ifndef BOT_HPP
#define BOT_HPP

#include <thread>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

#include <nlohmann/json.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>

#include "types.hpp"
#include "handlers.hpp"

namespace asio = boost::asio;

namespace bot
{
  class Bot
  {
   public:
    Bot(const std::string& token, handlers::Handler mh);
   private:
    std::string token_;
    handlers::Handler* mh_;
    asio::io_context ioContext_;
    std::thread ioThread_;
    std::queue< types::Message > messageQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCondition_;
    bool isRunning_ = false;
    asio::ssl::context sslContext_;

    void runIoContext();
    void fetchUpdates();
    // void processMessagesAsync();
    // void sendMessageAsync(const std::string& chatId, const std::string& text);
  };
}

#endif
