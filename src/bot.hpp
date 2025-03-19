#ifndef BOT_HPP
#define BOT_HPP

#include <string>
#include "handlers.hpp"

class Bot
{
 public:
  Bot(const std::string& token, handlers::Handler mh):
    token_(token),
    mh_(&mh)
  {}
 private:
  std::string token_;
  handlers::Handler* mh_;
};

#endif
