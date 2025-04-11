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

void handlers::MessageHandler::addHandler(const std::string& cmd, handler_t handler)
{
  cmdHandlers_[cmd] = handler;
}

void handlers::MessageHandler::addHandler(const states::State& state, state_handler_t handler)
{
  stateHandlers_[state] = handler;
}

void handlers::MessageHandler::addHandler(const std::string& cmd, const states::State& state, state_handler_t handler)
{
  stateCmdHandlers_[{cmd, state}] = handler;
}

void handlers::MessageHandler::processMessage(const types::Message& msg) const
{
  try
  {
    cmdHandlers_.at(fetchCommand(msg))(msg);
  }
  catch(const std::exception&)
  {}
}
