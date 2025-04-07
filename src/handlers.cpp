#include "handlers.hpp"
#include <string>

std::string fetchCommand(const types::Message& msg)
{
  std::string cmd;
  for (size_t i = 0; (i < msg.text.size()) && (msg.text[i] != ' '); ++i)
  {
    cmd += msg.text[i];
  }
  return cmd;
}

handlers::MessageHandler::MessageHandler():
  cmds_()
{}

void handlers::MessageHandler::addHandler(const std::string& cmd, handler_t handler)
{
  cmds_[cmd] = handler;
}

void handlers::MessageHandler::processMessage(const types::Message& msg) const
{
  try
  {
    cmds_.at(fetchCommand(msg))(msg);
  }
  catch(const std::exception&)
  {}
}
