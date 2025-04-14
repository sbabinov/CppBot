#ifndef TYPES_HPP
#define TYPES_HPP
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace types
{
  struct InlineKeyboardButton
  {
    std::string text;
    std::string callbackData;
    std::string url;

    InlineKeyboardButton() = default;
    InlineKeyboardButton(const std::string& text, const std::string& callbackData = "", const std::string& url = "");
  };
  void to_json(json& j, const InlineKeyboardButton& button);
  void from_json(const json& j, InlineKeyboardButton& button);

  struct InlineKeyboardMarkup
  {
    using keyboard_t = std::vector< std::vector< InlineKeyboardButton > >;
    keyboard_t keyboard;
    InlineKeyboardMarkup() = default;
    InlineKeyboardMarkup(const keyboard_t& keyboard);
  };
  void to_json(json& j, const InlineKeyboardMarkup& keyboard);
  void from_json(const json& j, InlineKeyboardMarkup& keyboard);

  struct User
  {
    size_t id;
    std::string firstName;
    std::string lastName;
    std::string username;
  };
  void to_json(json& j, const User& user);
  void from_json(const json& j, User& user);

  struct Chat
  {
    enum Type {PRIVATE, GROUP, SUPERGROUP, CHANNEL};
    size_t id;
    Type type;

    static const std::unordered_map< Type, std::string > typeAsString;
    static const std::unordered_map< std::string, Type > typeFromString;
  };
  void to_json(json& j, const Chat& chat);
  void from_json(const json& j, Chat& chat);

  struct Message
  {
    size_t id;
    User from;
    Chat chat;
    std::string text;
    InlineKeyboardMarkup replyMarkup;
  };
  void to_json(json& j, const Message& msg);
  void from_json(const json& j, Message& msg);

  struct CallbackQuery
  {
    size_t id;
    User from;
    Message message;
    std::string data;
  };
  void to_json(json& j, const CallbackQuery& query);
  void from_json(const json& j, CallbackQuery& query);
}

#endif
