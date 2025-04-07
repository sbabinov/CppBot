#include "handlers.hpp"

handlers::MessageHandler::MessageHandler():
  cmds_()
{}

void handlers::MessageHandler::addHandler(const std::string& cmd, handler_t handler)
{
  cmds_[cmd] = handler;
}
