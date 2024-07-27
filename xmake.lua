set_project("C++ Telegram YouTube video downloader")

target("main")
    add_files("*.cpp")
    add_links("TgBot",
              "boost_system",
              "ssl",
              "crypto",
              "pthread")
    add_languages("c++23")