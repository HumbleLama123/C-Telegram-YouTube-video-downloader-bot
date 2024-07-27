#include <boost/process.hpp>
#include <iostream>
#include <tgbot/tgbot.h>

#include "yt_downloader.hpp"

using namespace std;

const string BOT_TOKEN = "";

yt_downloader::downloader _yt_downloader;

inline void bot_on_command_start(TgBot::Bot &bot)
{
    bot.getEvents().onCommand(
        "start", [&bot](TgBot::Message::Ptr message)
        { bot.getApi().sendMessage(message->chat->id, "Enter YouTube URL: "); }
    );
}

inline void bot_init_inline_exts_keyboard(
    TgBot::Bot &bot, TgBot::InlineKeyboardMarkup::Ptr &keyboard
)
{
    vector<TgBot::InlineKeyboardButton::Ptr> btns;

    for (const auto &i : _yt_downloader.get_exts())
    {
        TgBot::InlineKeyboardButton::Ptr extbtn(new TgBot::InlineKeyboardButton
        );

        extbtn->text = i;
        extbtn->callbackData = i;

        btns.push_back(extbtn);
    }

    keyboard->inlineKeyboard.push_back(btns);
}

inline void bot_on_any_message(TgBot::Bot &bot)
{
    bot.getEvents().onAnyMessage(
        [&bot](TgBot::Message::Ptr message)
        {
            if (message->text != "/start")
            {
                _yt_downloader.set_url(message->text);

                TgBot::InlineKeyboardMarkup::Ptr keyboard(
                    new TgBot::InlineKeyboardMarkup
                );

                bot_init_inline_exts_keyboard(bot, keyboard);

                bot.getApi().sendMessage(
                    message->chat->id, "Choose ext: ", nullptr, nullptr,
                    keyboard
                );
            }
        }
    );
}

inline void bot_download_selected_ext(TgBot::Bot &bot)
{
    bot.getEvents().onCallbackQuery(
        [&bot](TgBot::CallbackQuery::Ptr query)
        {
            for (const auto &i : _yt_downloader.get_exts())
                if (query->data == i)
                {
                    _yt_downloader.download({i});

                    for (const auto &jj : yt_downloader::video_exts)
                    {
                        if (i == jj.first)
                        {
                            if (jj.second == "audio")
                                bot.getApi().sendAudio(
                                    query->message->chat->id,
                                    TgBot::InputFile::fromFile(
                                        "FILE." + i, "audio/" + i
                                    )
                                );
                            else if (jj.second == "video")
                                bot.getApi().sendVideo(
                                    query->message->chat->id,
                                    TgBot::InputFile::fromFile(
                                        "FILE." + i, "video/" + i
                                    )
                                );
                            else if (jj.second == "other")
                            {
                                bot.getApi().sendDocument(
                                    query->message->chat->id,
                                    TgBot::InputFile::fromFile("FILE." + i, "")
                                );
                            }
                        }
                    }

                    boost::process::child c("rm -rf FILE." + i);

                    c.wait();
                }
        }
    );
}

int main()
{
    TgBot::Bot bot(BOT_TOKEN);

    bot_on_command_start(bot);

    bot_on_any_message(bot);

    bot_download_selected_ext(bot);

    try
    {
        TgBot::TgLongPoll longPoll(bot);

        while (true) longPoll.start();
    } catch (TgBot::TgException &e)
    {
        cout << "TgBot error: " << e.what() << endl;
    }

    return EXIT_SUCCESS;
}