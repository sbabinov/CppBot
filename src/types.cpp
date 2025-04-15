#include "types.hpp"
#include <fstream>
#include <exception>

using json = nlohmann::json;


// InlineKeyboard
types::InlineKeyboardButton::InlineKeyboardButton(const std::string& text, const std::string& callbackData,
 const std::string& url)
{
  this->text = text;
  this->callbackData = callbackData;
  this->url = url;
}

types::InlineKeyboardMarkup::InlineKeyboardMarkup(const keyboard_t& keyboard)
{
  this->keyboard = keyboard;
}

void types::to_json(json& j, const types::InlineKeyboardButton& button)
{
  j["text"] = button.text;
  if (button.callbackData != "")
  {
    j["callback_data"] = button.callbackData;
  }
  if (button.url != "")
  {
    j["url"] = button.url;
  }
}

void types::from_json(const json& j, types::InlineKeyboardButton& button)
{
  j.at("text").get_to(button.text);
  if (j.contains("callback_data"))
  {
    j.at("callback_data").get_to(button.callbackData);
  }
  if (j.contains("url"))
  {
    j.at("url").get_to(button.callbackData);
  }
}

void types::to_json(json& j, const types::InlineKeyboardMarkup& keyboard)
{
  j["inline_keyboard"] = keyboard.keyboard;
}

void types::from_json(const json& j, types::InlineKeyboardMarkup& keyboard)
{
  for (const auto& row : j.at("inline_keyboard"))
  {
    keyboard.keyboard.push_back(std::vector< types::InlineKeyboardButton >());
    for (const auto& button : row)
    {
      keyboard.keyboard[keyboard.keyboard.size() - 1].push_back(types::InlineKeyboardButton{});
      size_t buttonsLen = keyboard.keyboard[keyboard.keyboard.size() - 1].size();
      from_json(button, keyboard.keyboard[keyboard.keyboard.size() - 1][buttonsLen - 1]);
    }
  }
}

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
  if (!msg.replyMarkup.keyboard.empty())
  {
    j["reply_markup"] = msg.replyMarkup;
  }
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
  if (j.contains("reply_markup"))
  {
    from_json(j.at("reply_markup"), msg.replyMarkup);
  }
}

// CallbackQuery
void types::to_json(json& j, const types::CallbackQuery& query)
{
  json from;
  json message;
  to_json(from, query.from);
  to_json(message, query.message);
  j = json{
    {"id", std::to_string(query.id)},
    {"from", from},
    {"message", message},
    {"data", query.data}
  };
}

void types::from_json(const json& j, types::CallbackQuery& query)
{
  std::string stringId;
  j.at("id").get_to(stringId);
  query.id = std::stoull(stringId);
  if (j.contains("from"))
  {
    from_json(j.at("from"), query.from);
  }
  if (j.contains("message"))
  {
    from_json(j.at("message"), query.message);
  }
  if (j.contains("data"))
  {
    j.at("data").get_to(query.data);
  }
}

// Files
types::InputFile::InputFile(const std::string& path)
{
  std::ifstream file(path, std::ios::binary);
  if (!file)
  {
    throw std::runtime_error("Cannot open file: " + path);
  }
  strBytes_ = std::string(std::istreambuf_iterator< char >(file), std::istreambuf_iterator< char >());
}
