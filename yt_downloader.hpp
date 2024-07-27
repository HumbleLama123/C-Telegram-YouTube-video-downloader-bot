#pragma once

#ifndef DOWNLOADER
#define DOWNLOADER

#include <string>
#include <vector>

#define YT_DOWNLOADER_DEFAULT_OUTPUT_FILENAME "%(title)s.%(ext)s"

namespace yt_downloader
{
    const std::vector<std::pair<std::string, std::string>> video_exts = {
        std::make_pair("m4a", "audio"), std::make_pair("mp4", "video")
    };

    struct video_download_info
    {
        std::string ext;
    };

    class downloader
    {
      public:
        downloader();

        downloader(const std::string &url);

        void set_url(const std::string &url);

        const std::vector<std::string> get_exts() const;

        void download(const video_download_info &download_info);

        const std::string get_title() const;

      private:
        struct params
        {
            std::string url;
        } params;

        struct video_info
        {
            std::vector<std::string> exts;
            std::string title;
        } video_info;
    };
} // namespace yt_downloader

#endif // YT_DOWNLOADER_HPP