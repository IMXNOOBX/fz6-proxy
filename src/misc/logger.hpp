#pragma once

#include <Windows.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

#include <filesystem>
#include <fstream>

#define LOG_INIT() logger::instance.allocate_console()
#define LOG_OUT(path) logger::instance.open_file(path);

#define LOG(type, text) logger::type(text)
#define LOGF(type, msg)                 \
    do {                                \
        std::ostringstream stream;      \
        stream << msg;                  \
        logger::type(stream.str());     \
    } while (0)
#define LOG_IF(condition, type, text)   \
    do {                                \
        if (condition) LOG(type, text); \
    } while (0)

namespace logger {
    enum class debug_type {
        info,
        debug,
        success,
        warning,
        fatal
    };

    class logger_t {
    public:
        void allocate_console() {
            std::lock_guard lock(mutex_);

            if (console_allocated_) {
                return;
            }

            AllocConsole();

            FILE* file_ptr = nullptr;

            freopen_s(&file_ptr, "CONIN$", "r", stdin);
            freopen_s(&file_ptr, "CONOUT$", "w", stdout);
            freopen_s(&file_ptr, "CONOUT$", "w", stderr);

            console_input_ = GetStdHandle(STD_INPUT_HANDLE);
            console_output_ = GetStdHandle(STD_OUTPUT_HANDLE);

            SetConsoleTitleA("Debug Console");

            DWORD mode = 0;

            if (GetConsoleMode(console_input_, &mode)) {
                mode &= ~ENABLE_PROCESSED_INPUT;
                mode &= ~ENABLE_MOUSE_INPUT;

                SetConsoleMode(console_input_, mode);
            }

            console_allocated_ = true;
        }

        bool open_file(
            const std::filesystem::path& path,
            bool append = true
        ) {
            std::lock_guard lock(mutex_);

            if (log_file_.is_open()) {
                log_file_.close();
            }

            const auto open_mode =
                std::ios::out |
                (append ? std::ios::app : std::ios::trunc);

            log_file_.open(path, open_mode);

            file_logging_enabled_ = log_file_.is_open();

            return file_logging_enabled_;
        }

        void close_file() {
            std::lock_guard lock(mutex_);

            if (log_file_.is_open()) {
                log_file_.flush();
                log_file_.close();
            }

            file_logging_enabled_ = false;
        }

        void write(debug_type type, const std::string& message) {
            std::lock_guard lock(mutex_);

            if (message == last_message_) {
                ++repeat_count_;
                return;
            }

            flush_repeated_message();

            const auto [prefix, color] = get_style(type);

            if (console_allocated_) {
                if (current_color_ != color) {
                    SetConsoleTextAttribute(console_output_, color);
                    current_color_ = color;
                }

                std::cout
                    << '[' << current_timestamp() << "] "
                    << prefix << ' '
                    << message;
            }

            if (file_logging_enabled_) {
                log_file_
                    << '[' << current_timestamp() << "] "
                    << prefix << ' '
                    << message;
            }

            last_message_ = message;
            repeat_count_ = 1;
        }

        ~logger_t() {
            flush_repeated_message();
            close_file();
        }

    private:
        struct style_t {
            const char* prefix;
            WORD color;
        };

        static style_t get_style(debug_type type) {
            switch (type) {
            case debug_type::success:
                return {
                    "[success]",
                    FOREGROUND_GREEN | FOREGROUND_INTENSITY
                };

            case debug_type::warning:
                return {
                    "[warning]",
                    FOREGROUND_RED |
                    FOREGROUND_GREEN |
                    FOREGROUND_INTENSITY
                };

            case debug_type::fatal:
                return {
                    "[error]",
                    FOREGROUND_RED | FOREGROUND_INTENSITY
                };

            case debug_type::debug:
                return {
                    "[debug]",
                    FOREGROUND_INTENSITY
                };

            default:
                return {
                    "[info]",
                    FOREGROUND_BLUE | FOREGROUND_INTENSITY
                };
            }
        }

        static std::string current_timestamp() {
            using namespace std::chrono;

            static std::string cached_timestamp;
            static auto next_update = system_clock::now();

            const auto now = system_clock::now();

            if (now >= next_update) {
                next_update = now + seconds(1);

                const auto time = system_clock::to_time_t(now);

                std::tm local_tm{};
                localtime_s(&local_tm, &time);

                std::ostringstream stream;

                stream << std::put_time(
                    &local_tm,
                    "%Y-%m-%d|%H:%M:%S"
                );

                cached_timestamp = stream.str();
            }

            return cached_timestamp;
        }

        void flush_repeated_message() {
            if (last_message_.empty()) {
                return;
            }

            if (repeat_count_ > 1) {
                if (console_allocated_) {
                    std::cout << " (" << repeat_count_ << "x)";
                }

                if (file_logging_enabled_) {
                    log_file_ << " (" << repeat_count_ << "x)";
                }
            }

            if (console_allocated_) {
                std::cout << std::endl;
            }

            if (file_logging_enabled_) {
                log_file_ << std::endl;
                log_file_.flush();
            }
        }

    private:
        HANDLE console_input_ = nullptr;
        HANDLE console_output_ = nullptr;

        std::ofstream log_file_;

        WORD current_color_ = 0;

        int repeat_count_ = 0;

        bool console_allocated_ = false;
        bool file_logging_enabled_ = false;

        std::string last_message_;

        std::mutex mutex_;
    };

    inline logger_t instance;

    inline void SUCCESS(const std::string& text) {
        instance.write(debug_type::success, text);
    }

    inline void WARN(const std::string& text) {
        instance.write(debug_type::warning, text);
    }

    inline void FATAL(const std::string& text) {
        instance.write(debug_type::fatal, text);
    }

    inline void INFO(const std::string& text) {
        instance.write(debug_type::info, text);
    }

    inline void DEBUG(const std::string& text) {
        instance.write(debug_type::debug, text);
    }
}