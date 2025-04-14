#include "types.hpp"

using json = nlohmann::json;

// User
void types::to_json(json& j, const types::User& user)
{
  j = json{
    {"id", user.id},
    {"first_name", user.firstName},
    {"last_name", user.lastName},
    {"username", user.username}
  };
}

void types::from_json(const json& j, types::User& user)
{
  j.at("id").get_to(user.id);
  j.at("first_name").get_to(user.firstName);
  if (j.contains("last_name"))
  {
    j.at("last_name").get_to(user.lastName);
  }
  if (j.contains("username"))
  {
    j.at("username").get_to(user.username);
  }
}

// Chat
const std::unordered_map< types::Chat::Type, std::string > types::Chat::typeAsString = {
    {types::Chat::PRIVATE, "private"},
    {types::Chat::GROUP, "group"},
    {types::Chat::SUPERGROUP, "supergroup"},
    {types::Chat::CHANNEL, "channel"}
  };

  const std::unordered_map< std::string, types::Chat::Type > types::Chat::typeFromString = {
    {"private", types::Chat::PRIVATE},
    {"group", types::Chat::GROUP},
    {"supergroup", types::Chat::SUPERGROUP},
    {"channel", types::Chat::CHANNEL}
  };

void types::to_json(json& j, const types::Chat& chat)
{
  j = json{
    {"id", chat.id},
    {"type", Chat::typeAsString.at(chat.type)}
  };
}

void types::from_json(const json& j, types::Chat& chat)
{
  j.at("id").get_to(chat.id);
  chat.type = Chat::typeFromString.at(j.at("type"));
}

// Message
void types::to_json(json& j, const types::Message& msg)
{
  json from;
  json chat;
  to_json(from, msg.from);
  to_json(chat, msg.chat);
  j = json{
    {"message_id", msg.id},
    {"from", from},
    {"chat", chat},
    {"text", msg.text}
  };
}

void types::from_json(const json& j, types::Message& msg)
{
  j.at("message_id").get_to(msg.id);
  if (j.contains("from"))
  {
    from_json(j.at("from"), msg.from);
  }
  from_json(j.at("chat"), msg.chat);
  if (j.contains("text"))
  {
    j.at("text").get_to(msg.text);
  }
}
