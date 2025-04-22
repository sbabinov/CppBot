/*!
  @file
  @brief Header contains types represents different Telegram objects.
  @author sbabinov92
  @version 1.0
  @date April 2025
  @warning The project is still in development
*/

#ifndef CPPBOT_TYPES_HPP
#define CPPBOT_TYPES_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace types
{
  /// Struct contains telegram file info.
  struct File
  {
    std::string fileId;
    std::string fileUniqueId;
    std::string filePath;
  };
  void from_json(const json& j, File& photo);

  /// Struct represents telegram photo object.
  struct PhotoSize: public File
  {
    size_t width;
    size_t height;
  };
  void from_json(const json& j, PhotoSize& photo);

  /// Struct represents telegram document object.
  struct Document: public File
  {};
  void from_json(const json& j, Document& document);

  /// Struct represents telegram audio object.
  struct Audio: public File
  {
    size_t duration;
  };
  void from_json(const json& j, Audio& audio);

  /// Struct represents telegram video object.
  struct Video: public File
  {
    size_t width;
    size_t height;
    size_t duration;
  };
  void from_json(const json& j, Video& video);

  /// Base struct for telegram keyboards.
  struct Keyboard
  {
    Keyboard() = default;
    virtual json toJson() const;
  };

  /// Struct represents button for inline keyboard.
  struct InlineKeyboardButton
  {
    std::string text;
    std::string callbackData;
    std::string url;

    InlineKeyboardButton() = default;

    /*!
      @param text Text that will be shown at button
      @param callbackData String data allows to identify this button
      @param url Url that will be opened when button was pressed
    */
    InlineKeyboardButton(const std::string& text, const std::string& callbackData = "", const std::string& url = "");
  };
  void to_json(json& j, const InlineKeyboardButton& button);
  void from_json(const json& j, InlineKeyboardButton& button);

  /// Struct represents telegram inline keyboard.
  struct InlineKeyboardMarkup: public Keyboard
  {
    using keyboard_t = std::vector< std::vector< InlineKeyboardButton > >;
    keyboard_t keyboard;
    InlineKeyboardMarkup() = default;

    /*!
      @param keyboard Vector of vectors of InlineKeyboardButton objects
    */
    InlineKeyboardMarkup(const keyboard_t& keyboard);
    virtual json toJson() const override;
  };
  void to_json(json& j, const InlineKeyboardMarkup& keyboard);
  void from_json(const json& j, InlineKeyboardMarkup& keyboard);

  /// Struct represents button for reply keyboard.
  struct KeyboardButton
  {
    std::string text;
    KeyboardButton() = default;

    /*!
      @param text Text that will be shown at button
    */
    KeyboardButton(const std::string& text);
  };
  void to_json(json& j, const KeyboardButton& button);

  /// Struct represents telegram reply keyboard.
  struct ReplyKeyboardMarkup: public Keyboard
  {
    using keyboard_t = std::vector< std::vector< KeyboardButton > >;
    keyboard_t keyboard;
    bool isPersistent;
    bool resizeKeyboard;
    bool oneTimeKeyboard;
    std::string inputFieldPlaceholder;
    ReplyKeyboardMarkup() = default;

    /*!
      @param keyboard Vector of vectors of KeyboardButton objects
      @param isPersistent Requests client to always show the keyboard when the regular keyboard is hidden
      @param resizeKeyboard Requests client to resize the keyboard vertically for optimal fit
      @param oneTimeKeyboard Requests client to hide the keyboard as soon as it's been used
      @param placeholder The placeholder to be shown in the input field when the keyboard is active
    */
    ReplyKeyboardMarkup(const keyboard_t& keyboard, bool isPersistent = false, bool resizeKeyboard = false,
      bool oneTimeKeyboard = false, const std::string& placeholder = "");
    virtual json toJson() const override;
  };
  void to_json(json& j, const types::ReplyKeyboardMarkup& keyboard);

  /// Object of this struct is using for remove current reply keyboard.
  struct ReplyKeyboardRemove: public Keyboard
  {
    ReplyKeyboardRemove() = default;
    json toJson() const override;
  };
  void to_json(json& j, const types::ReplyKeyboardRemove& keyboard);

  /// Struct represents a Telegram user.
  struct User
  {
    size_t id;
    std::string firstName;
    std::string lastName;
    std::string username;
  };
  void to_json(json& j, const User& user);
  void from_json(const json& j, User& user);

  /// Struct represents a chat.
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

  /// Struct represents a Telegram message.
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

  /// Struct represents an incoming callback query from a callback button in an InlineKeyboard.
  struct CallbackQuery
  {
    std::string id;
    User from;
    Message message;
    std::string data;
  };
  void to_json(json& j, const CallbackQuery& query);
  void from_json(const json& j, CallbackQuery& query);

  /// Class represents the contents of a file to be uploaded.
  class InputFile
  {
   public:
    InputFile() = default;

    /*!
      @param path Path to a file to be uploaded
    */
    InputFile(const std::string& path);

    /// Method allows to get file name (without full path).
    std::string name() const;

    /// Method allows to get a file as a string of bytes.
    std::string bytes() const;
   private:
    std::string path_;
  };

  enum MediaType
  {
    PHOTO,
    DOCUMENT,
    AUDIO,
    VIDEO
  };

  /// Class represents the content of a media message to be sent.
  class InputMedia
  {
   public:
    std::string caption;

    /*!
      @param mediaType Type of media
      @param path Path to a file
      @param caption Caption for a media
    */
    InputMedia(MediaType mediaType, const std::string& path, const std::string& caption = "");

    /// Method allows to get a media type.
    std::string type() const;

    /// Method allows to get media file as InputFile object.
    InputFile file() const;

    friend void to_json(json& j, const InputMedia& query);
   protected:
    InputFile file_;
    std::string type_;
    bool hasSpoiler_;
  };
  void to_json(json& j, const InputMedia& query);

  /// Class represents a photo to be sent.
  class InputMediaPhoto: public InputMedia
  {
   public:

    /*!
      @param path Path to a file
    */
    InputMediaPhoto(const std::string& path);

    /*!
      @param path Path to a file
      @param hasSpoiler If true, photo will be covered with a spoiler animation
    */
    InputMediaPhoto(const std::string& path, bool hasSpoiler);
  };

  /// Class represents a document to be sent.
  class InputMediaDocument: public InputMedia
  {
   public:
    /*!
      @param path Path to a file
    */
    InputMediaDocument(const std::string& path);
  };

  /// Class represents an audio to be sent.
  class InputMediaAudio: public InputMedia
  {
   public:
    /*!
      @param path Path to a file
    */
    InputMediaAudio(const std::string& path);
  };

  /// Class represents a video to be sent.
  class InputMediaVideo: public InputMedia
  {
   public:
    /*!
      @param path Path to a file
    */
    InputMediaVideo(const std::string& path);

    /*!
      @param path Path to a file
      @param hasSpoiler If true, video will be covered with a spoiler animation
    */
    InputMediaVideo(const std::string& path, bool hasSpoiler);
  };
}

#endif
