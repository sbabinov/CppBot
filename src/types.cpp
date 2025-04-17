#include "types.hpp"
#include <fstream>
#include <exception>

using json = nlohmann::json;

// InlineKeyboard
json types::Keyboard::toJson() const
{
  return json();
}

types::InlineKeyboardButton::InlineKeyboardButton(const std::string& text, const std::string& callbackData,
 const std::string& url)
{
  this->text = text;
  this->callbackData = callbackData;
  this->url = url;
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

types::InlineKeyboardMarkup::InlineKeyboardMarkup(const keyboard_t& keyboard)
{
  this->keyboard = keyboard;
}

json types::InlineKeyboardMarkup::toJson() const
{
  return *this;
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

// ReplyKeyboard
types::KeyboardButton::KeyboardButton(const std::string& text)
{
  this->text = text;
}

void types::to_json(json& j, const types::KeyboardButton& button)
{
  j["text"] = button.text;
}

types::ReplyKeyboardMarkup::ReplyKeyboardMarkup(const keyboard_t& keyboard, bool isPersistent,
  bool resizeKeyboard, bool oneTimeKeyboard, const std::string& placeholder)
{
  this->keyboard = keyboard;
  this->isPersistent = isPersistent;
  this->resizeKeyboard = resizeKeyboard;
  this->oneTimeKeyboard = oneTimeKeyboard;
  this->inputFieldPlaceholder = placeholder;
}

json types::ReplyKeyboardMarkup::toJson() const
{
  return *this;
}

void types::to_json(json& j, const types::ReplyKeyboardMarkup& keyboard)
{
  j["keyboard"] = keyboard.keyboard;
  j["is_persistent"] = keyboard.isPersistent;
  j["resize_keyboard"] = keyboard.resizeKeyboard;
  j["one_time_keyboard"] = keyboard.oneTimeKeyboard;
  j["input_field_placeholder"] = keyboard.inputFieldPlaceholder;
}

json types::ReplyKeyboardRemove::toJson() const
{
  return *this;
}

void types::to_json(json& j, const types::ReplyKeyboardRemove& keyboard)
{
  j["remove_keyboard"] = true;
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
  j.at("date").get_to(msg.date);
  if (j.contains("photo"))
  {
    for (const auto& p : j["photo"])
    {
      PhotoSize photo;
      from_json(p, photo);
      msg.photo.push_back(photo);
    }
  }
  if (j.contains("document"))
  {
    from_json(j, msg.document);
  }
  if (j.contains("audio"))
  {
    j.at("audio").get_to(msg.audio);
  }
  if (j.contains("video"))
  {
    j.at("video").get_to(msg.video);
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
std::string extractFileName(const std::string& path)
{
  return path.substr(path.find_last_of("/\\") + 1);
}

types::InputFile::InputFile(const std::string& path)
{
  std::ifstream file(path, std::ios::binary);
  if (!file)
  {
    throw std::runtime_error("Cannot open file: " + path);
  }
  name_ = extractFileName(path);
  strBytes_ = std::string(std::istreambuf_iterator< char >(file), std::istreambuf_iterator< char >());
}

std::string types::InputFile::name() const
{
  return name_;
}

std::string types::InputFile::asStringBytes() const
{
  return strBytes_;
}

types::InputMedia::InputMedia(types::MediaType mediaType, const std::string& path, const std::string& caption)
{
  std::ifstream file(path, std::ios::binary);
  if (!file)
  {
    throw std::runtime_error("Cannot open file: " + path);
  }

  switch (mediaType)
  {
  case PHOTO:
    type_ = "photo";
    break;
  case DOCUMENT:
    type_ = "document";
    break;
  case AUDIO:
    type_ = "audio";
    break;
  case VIDEO:
    type_ = "video";
    break;
  }
  file_ = types::InputFile(path);
  this->caption = caption;
  hasSpoiler_ = false;
}

std::string types::InputMedia::type() const
{
  return type_;
}

types::InputFile types::InputMedia::file() const
{
  return file_;
}

void types::to_json(json& j, const types::InputMedia& media)
{
  j = json{
    {"type", media.type_},
    {"media", "attach://" + media.file_.name()}
  };
  if (media.caption != "")
  {
    j["caption"] = media.caption;
  }
  if (media.hasSpoiler_)
  {
    j["has_spoiler"] = true;
  }
}

types::InputMediaPhoto::InputMediaPhoto(const std::string& path):
  types::InputMedia(types::MediaType::PHOTO, path)
{}

types::InputMediaPhoto::InputMediaPhoto(const std::string& path, bool hasSpoiler):
  types::InputMediaPhoto(path)
{
  hasSpoiler_ = hasSpoiler;
}

types::InputMediaDocument::InputMediaDocument(const std::string& path):
  types::InputMedia(types::MediaType::DOCUMENT, path)
{}

types::InputMediaAudio::InputMediaAudio(const std::string& path):
  types::InputMedia(types::MediaType::AUDIO, path)
{}

types::InputMediaVideo::InputMediaVideo(const std::string& path):
  types::InputMedia(types::MediaType::VIDEO, path)
{}

types::InputMediaVideo::InputMediaVideo(const std::string& path, bool hasSpoiler):
  types::InputMediaVideo(path)
{
  hasSpoiler_ = hasSpoiler;
}

// File types
void types::from_json(const json& j, types::File& file)
{
  j.at("file_id").get_to(file.fileId);
  j.at("file_unique_id").get_to(file.fileUniqueId);
  if (j.contains("file_path"))
  {
    j.at("file_path").get_to(file.filePath);
  }
}

void types::from_json(const json& j, types::PhotoSize& photo)
{
  j.at("file_id").get_to(photo.fileId);
  j.at("file_unique_id").get_to(photo.fileUniqueId);
  j.at("width").get_to(photo.width);
  j.at("height").get_to(photo.height);
}

void types::from_json(const json& j, types::Document& document)
{
  j.at("file_id").get_to(document.fileId);
  j.at("file_unique_id").get_to(document.fileUniqueId);
}

void types::from_json(const json& j, types::Audio& audio)
{
  j.at("file_id").get_to(audio.fileId);
  j.at("file_unique_id").get_to(audio.fileUniqueId);
  j.at("duration").get_to(audio.duration);
}

void types::from_json(const json& j, types::Video& video)
{
  j.at("file_id").get_to(video.fileId);
  j.at("file_unique_id").get_to(video.fileUniqueId);
  j.at("width").get_to(video.width);
  j.at("height").get_to(video.height);
  j.at("duration").get_to(video.duration);
}
