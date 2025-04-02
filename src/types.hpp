#ifndef TYPES_HPP
#define TYPES_HPP
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace types
{
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
  };
  void to_json(json& j, const Message& msg);
  void from_json(const json& j, Message& msg);
}

#endif
