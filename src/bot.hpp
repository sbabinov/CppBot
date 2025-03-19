#ifndef BOT_HPP
#define BOT_HPP

#include <string>
#include "handlers.hpp"

class Bot
{
 public:
  Bot(const std::string& token, handlers::MessageHandler mh):
    token_(token),
    mh_(&mh)
  {}
 private:
  std::string token_;
  handlers::MessageHandler* mh_;
};

#endif
