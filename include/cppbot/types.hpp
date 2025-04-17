#ifndef TYPES_HPP
#define TYPES_HPP
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace types
{
  struct File
  {
    std::string fileId;
    std::string fileUniqueId;
    std::string filePath;
  };
  void from_json(const json& j, File& photo);

  struct PhotoSize: public File
  {
    size_t width;
    size_t height;
  };
  void from_json(const json& j, PhotoSize& photo);

  struct Document: public File
  {};
  void from_json(const json& j, Document& document);

  struct Audio: public File
  {
    size_t duration;
  };
  void from_json(const json& j, Audio& audio);

  struct Video: public File
  {
    size_t width;
    size_t height;
    size_t duration;
  };
  void from_json(const json& j, Video& video);

  struct Keyboard
  {
    Keyboard() = default;
    virtual json toJson() const;
  };

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

  struct InlineKeyboardMarkup: public Keyboard
  {
    using keyboard_t = std::vector< std::vector< InlineKeyboardButton > >;
    keyboard_t keyboard;
    InlineKeyboardMarkup() = default;
    InlineKeyboardMarkup(const keyboard_t& keyboard);
    virtual json toJson() const override;
  };
  void to_json(json& j, const InlineKeyboardMarkup& keyboard);
  void from_json(const json& j, InlineKeyboardMarkup& keyboard);

  struct KeyboardButton
  {
    std::string text;
    KeyboardButton() = default;
    KeyboardButton(const std::string& text);
  };
  void to_json(json& j, const KeyboardButton& button);

  struct ReplyKeyboardMarkup: public Keyboard
  {
    using keyboard_t = std::vector< std::vector< KeyboardButton > >;
    keyboard_t keyboard;
    bool isPersistent;
    bool resizeKeyboard;
    bool oneTimeKeyboard;
    std::string inputFieldPlaceholder;
    ReplyKeyboardMarkup() = default;
    ReplyKeyboardMarkup(const keyboard_t& keyboard, bool isPersistent = false, bool resizeKeyboard = false,
      bool oneTimeKeyboard = false, const std::string& placeholder = "");
    virtual json toJson() const override;
  };
  void to_json(json& j, const types::ReplyKeyboardMarkup& keyboard);

  struct ReplyKeyboardRemove: public Keyboard
  {
    ReplyKeyboardRemove() = default;
    json toJson() const override;
  };
  void to_json(json& j, const types::ReplyKeyboardRemove& keyboard);

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
    size_t date;
    std::vector< PhotoSize > photo;
    Document document;
    Audio audio;
    Video video;
    InlineKeyboardMarkup replyMarkup;
  };
  void to_json(json& j, const Message& msg);
  void from_json(const json& j, Message& msg);

  struct CallbackQuery
  {
    std::string id;
    User from;
    Message message;
    std::string data;
  };
  void to_json(json& j, const CallbackQuery& query);
  void from_json(const json& j, CallbackQuery& query);

  class InputFile
  {
   public:
    InputFile() = default;
    InputFile(const std::string& path);
    std::string name() const;
    std::string asStringBytes() const;
   private:
    std::string name_;
    std::string strBytes_;
  };

  enum MediaType {
    PHOTO,
    DOCUMENT,
    AUDIO,
    VIDEO
  };

  class InputMedia
  {
   public:
    std::string caption;
    InputMedia(MediaType mediaType, const std::string& path, const std::string& caption = "");
    std::string type() const;
    InputFile file() const;

    friend void to_json(json& j, const InputMedia& query);
   protected:
    InputFile file_;
    std::string type_;
    bool hasSpoiler_;
  };
  void to_json(json& j, const InputMedia& query);

  class InputMediaPhoto: public InputMedia
  {
   public:
    InputMediaPhoto(const std::string& path);
    InputMediaPhoto(const std::string& path, bool hasSpoiler);
  };

  class InputMediaDocument: public InputMedia
  {
   public:
    InputMediaDocument(const std::string& path);
  };

  class InputMediaAudio: public InputMedia
  {
   public:
    InputMediaAudio(const std::string& path);
  };

  class InputMediaVideo: public InputMedia
  {
   public:
    InputMediaVideo(const std::string& path);
    InputMediaVideo(const std::string& path, bool hasSpoiler);
  };
}

#endif
