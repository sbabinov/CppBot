# Info
This is a simple C++ async library makes it easier to create Telegram bots.
> [!NOTE]
> More detailed documentation you can find on [GitHub Pages](https://sbabinov.github.io/CppBot/).

# Build
The project can be built using CMakeLists.txt. There are some options you can change, see comments in this file.  

> [!TIP]
> To connect this library to your project, you can use following commands in <b>your</b> CMakeLists.txt:
> ```cmake
> find_package(cppbot REQUIRED)
> target_link_libraries(your-target PRIVATE cppbot)
> ```

# Supported features
These are features that the current version of the bot supports:
- [sending text messages](#sending-text-messages)
- [sending files (photo, documents, audio, video)](#sending-files)
- [sending messages with Telegram keyboards](#sending-messages-with-telegram-keyboards)
- [deleting messages](#deleting-messages)
- [editing text messages](#editing-text-messages)
- [editing messages with media](#editing-messages-with-media)
- [editing messages keyboards](#editing-messages-keyboards)
- [getting files info to download them](#getting-files-info-to-download-them)
- [using and processing callback queries](#using-and-processing-callback-queries)
- [using states for processing only certain messages](#using-states-for-processing-only-certain-messages)


# How the bot works
The ```bot``` uses three threads in its work which are initialized when the ```bot``` is starting. Starting method looks something like this:
```c++
void cppbot::Bot::startPolling()
{
  isRunning_ = true;
  ioThread_ = std::thread(&cppbot::Bot::runIoContext, this);
  std::thread(std::bind(&cppbot::Bot::fetchUpdates, this)).detach();
  processUpdates();
}
```
- The first thread ```ioThread_``` is used for processing async operations.  
- The second thread is responsible for fetching updates from telegram.org.  
- Main thread is processing all updates.

# Using some bot's methods
> [!IMPORTANT]
> All bot's methods are async, so they return ```std::future``` as a result.

Let the ```bot``` object be defined in some header in namespace ```app```.
```c++
#include <memory>
#include "cppbot/cppbot.hpp"
#include "cppbot/states.hpp"
#include "cppbot/handlers.hpp"

namespace app
{
  std::shared_ptr< handlers::MessageHandler > messageHandler = std::make_shared< handlers::MessageHandler >();
  std::shared_ptr< handlers::CallbackQueryHandler > queryHandler = std::make_shared< handlers::CallbackQueryHandler>();
  std::shared_ptr< states::Storage > storage = std::make_shared< states::Storage >();
  cppbot::Bot bot("YOUR_TOKEN_HERE", messageHandler, queryHandler, storage);
}
```
You need to create shared pointers to ```MessageHander```, ```CallbackQueryHandler``` and ```Storage``` objects and pass them to the bot constructor. Also the bot requires unique Telegram ```token```.
To start the bot, use method ```startPolling()```.

## Sending text messages
```types::Message``` class represents Telegram message.

This is the example of function for sending text message:
```c++
void echo(const types::Message& msg)
{  
  app::bot.sendMessage(msg.chat.id, "Hello there!"); 
}
```

To allow the ```bot``` to process this handler, you need to perform the following steps before starting the ```bot```:
```c++
app::bot.messageHandler->addHandler("your command", echo);
```
You will need to do this for each handler you have added for messages.

## Sending files
```types::InputFile``` class is used for sending your files.
```c++
types::InputFile photo("path to your file");
app::bot.sendPhoto(chatId, photo, "caption");

types::InputFile document("path to your file");
app::bot.sendDocument(chatId, document, "caption");
```

## Sending messages with Telegram keyboards
```types::ReplyKeyboard``` and ```types::InlineKeyboardMarkup``` are used for creating keyboards to send with messages.
```c++
types::ReplyKeyboard menu({
  {
    types::KeyboardButton("first button text"),
    types::KeyboardButton("second button text")
  }
});
app::bot.sendMessage(chatId, "Message with keyboard", menu);
```
To remove this type of keyboard (```types::RepyKeyboardMarkup```) from the user's chat, send message with ```ReplyKeyboardRemove``` object.

## Deleting messages
```c++
app::bot.deleteMessage(chatId, messageId);
```

## Editing text messages
```c++
app::bot.editMessageText(chatId, messageId, "new text");
```

## Editing messages with media
```types::InputMedia[Photo/Document/Audio/Video]``` is used for creating a file to send as a replacement.
```c++
types::InputMediaPhoto newPhoto("path to a new photo");
app::bot.editMessageMedia(chatId, messageId, newPhoto);
```

## Editing messages keyboards
```c++
types::InlineKeyboardMarkup newMenu(...);
app::bot.editMessageReplyMarkup(chatId, messageId, newMenu);
```

## Getting files info to download them
You can fetch files from the ```Message``` object using certain fields (```photo```, ```document```, ```audio```, ```video```).

> [!WARNING]
> To get file info, you need to call ```std::future.get()``` method, so this is the thread-blocking operation.

```c++
void getFileInfo(const types::Message& msg)
{
  types::Document document = msg.document;
  types::File file = app::bot.getFile(document.fileId).get();
  // now you can use file.filePath to download this file from https://api.telegram.org/file/bot<token>/<file_path>
}
```

## Using and processing callback queries
Firstly, you need to create ```types::InlineKeyboardMarkup``` object and send it to the user.
```c++
types::InlineKeyboardMarkup menu({
  {
    types::InlineKeyboardButton("first button", "first button callback data"),
    types::InlineKeyboardButton("second button", "second button callback data")
  }
})
app::bot.sendMessage(chatId, "some text", menu);
```

Now you can handle clicks on these buttons. Query handlers are used for this purpose:
```c++
void handleFirstButtonQuery(const types::CallbackQuery& query)
{
  app::bot.sendMessage(query.from.id, "you have pressed the first button");
}
```

Also, don't forget to add this handler to ```CallbackQueryHandler``` before starting the ```bot```:
```c++
app::queryHandler->addHandler("first button callback data", handleFirstButtonQuery);
```

## Using states for processing only certain messages
The user in a certain state can trigger handlers only for this state.

Firstly, you need to create your ```StatesForm```:
```c++
namespace forms
{
  struct RegistrationForm: public states::StatesForm
  {
    states::State username;
    states::State age;
  } registrationForm;
}
```

Then you can use this states to set them to the user.
```c++
// Called when entering command /start
void startRegistration(const types::Message& msg)
{
  auto state = app::bot.getStateContext(msg.chat.id); // get current state context
  app::bot.sendMessage(msg.chat.id, "Hello! Let's start registration. Enter username:");
  state.setState(forms::registrationForm.username); // set new state
}

void processUsername(const types::Message& msg, states::StateContext& state)
{
  state.data(state.current()) = msg.text; // set state context data for current state (registration.username)
  app::bot.sendMessage(msg.chat.id, "Great! Now enter your age:");
  state.setState(forms::registrationForm.age); // set next state
}

void processAge(const types::Message& msg, states::StateContext& state)
{
  auto data = state.data(); // get all current data fields
  std::string text = "Successfull registration! Please check all fields are right:\n\n";

  // Now we can read fields that write before
  text += "Username: " + boost::any_cast< std::string >(data[forms::registrationForm.username]) + '\n';
  text += "Age: " + msg.text + '\n';

  state.resetState(); // Don't forget to reset the state to default
  app::bot.sendMessage(msg.chat.id, text);
}
```

Adding handlers looks something like this:
```c++
app::messageHandler->addHandler("/start", startRegistration);

// This handlers will be called only in appropriate states
app::messageHandler->addHandler(forms::registrationForm.username, processUsername);
app::messageHandler->addHandler(forms::registrationForm.age, processAge);
```





