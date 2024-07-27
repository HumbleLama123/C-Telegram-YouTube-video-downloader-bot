#include <boost/process.hpp>

#include "nlohmann/json.hpp"
#include "yt_downloader.hpp"

nlohmann::json yt_json_info;

inline void parse_exts(std::vector<std::string> &exts)
{
    exts.clear();

    if (yt_json_info.contains("formats"))
    {
        for (const auto &i : yt_json_info["formats"])
            if (std::find(exts.begin(), exts.end(), i["ext"]) == exts.end()
                && i["ext"] != "mhtml" && i["ext"] != "webm")
                exts.push_back(i["ext"]);
    }
}

yt_downloader::downloader::downloader() {}

yt_downloader::downloader::downloader(const std::string &url) { set_url(url); }

void yt_downloader::downloader::set_url(const std::string &url)
{
    params.url = url;

    boost::process::ipstream pipe_stream;
    boost::process::child c(
        "yt-dlp --dump-json --no-playlist " + params.url,
        boost::process::std_out > pipe_stream
    );

    std::string temp, output_json;

    while (std::getline(pipe_stream, temp)) output_json += temp;

    c.wait();

    yt_json_info = nlohmann::json::parse(output_json);

    parse_exts(video_info.exts);

    video_info.title = yt_json_info["title"];
}

const std::vector<std::string> yt_downloader::downloader::get_exts() const
{
    return video_info.exts;
}

void yt_downloader::downloader::download(
    const yt_downloader::video_download_info &download_info
)
{
    std::string download_command;

    for (const auto &i : video_exts)
    {
        if (i.first == download_info.ext)
        {
            if (i.second == "audio")
            {
                download_command =
                    "yt-dlp --no-playlist -f bestaudio[ext=" + download_info.ext
                    + "] " + params.url + " -o FILE.%(ext)s";
            } else if (download_info.ext != "webm") {
                boost::process::child c(
                    "yt-dlp --no-playlist -f bestvideo[ext=webm]+bestaudio "
                    + params.url + " -o FILE.%(ext)s"
                );

                c.wait();

                download_command =
                    "ffmpeg -i FILE.webm -c:v copy -c:a copy FILE." + i.first;
            }
        }
    }

    boost::process::child c(download_command);

    c.wait();

    c = boost::process::child("rm -rf FILE.webm");

    c.wait();
}

const std::string yt_downloader::downloader::get_title() const
{
    return video_info.title;
}