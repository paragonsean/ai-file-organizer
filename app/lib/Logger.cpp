#include "constants.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include <cstdlib>
#include <string>
#include <filesystem>


/**
 * Returns the log directory path for the application.
 *
 * On Windows, this will return "%APPDATA%\AIFileSorter\logs".
 * On Linux, this will return "$XDG_CACHE_HOME/AIFileSorter/logs".
 *
 * @return The log directory path.
 */
std::string Logger::get_log_directory()
{
#ifdef _WIN32
    return get_windows_log_directory();
#else
    return get_xdg_cache_home();
#endif
}


/**
 * Returns the value of the XDG_CACHE_HOME environment variable, or a default
 * value if this variable is not set. The default value is $HOME/.cache/AIFileSorter/logs.
 *
 * This function will throw a std::runtime_error if neither XDG_CACHE_HOME nor
 * HOME are set.
 *
 * @return The value of the XDG_CACHE_HOME environment variable, or a default
 * value if this variable is not set.
 */
std::string Logger::get_xdg_cache_home()
{
    const char* xdg_cache_home = std::getenv("XDG_CACHE_HOME");
    if (xdg_cache_home && *xdg_cache_home) {
        return std::string(xdg_cache_home) + "/" + APP_NAME_DIR + "/my_app/logs";
    }

    const char* home = std::getenv("HOME");
    if (home && *home) {
        return std::string(home) + "/.cache/" + APP_NAME_DIR + "/logs";
    }
    throw std::runtime_error("Failed to determine XDG_CACHE_HOME or HOME environment variable.");
}


/**
 * Returns the path to the log directory for the application on Windows.
 *
 * On Windows, this will return "%APPDATA%\AIFileSorter\logs".
 *
 * This function will throw a std::runtime_error if APPDATA environment variable
 * is not set.
 *
 * @return The path to the log directory for the application on Windows.
 */
std::string Logger::get_windows_log_directory() {
    const char* appdata = std::getenv("APPDATA");
    if (appdata && *appdata) {
        return std::string(appdata) + "\\" + APP_NAME_DIR + "\\logs";
    }
    throw std::runtime_error("Failed to determine APPDATA environment variable.");
}


/**
 * Configures and sets up the loggers for the application.
 *
 * This function creates a log directory if it doesn't already exist and sets up
 * three separate loggers: core_logger, db_logger, and ui_logger. Each logger
 * has both a console sink and a rotating file sink. The file sinks write log
 * entries to core.log, db.log, and ui.log files, respectively, with a maximum
 * file size of approximately 5 MB and at most 3 rotated files. All loggers are
 * registered with the spdlog library and are set to log messages at the warning
 * level or higher. The function logs an informational message once the loggers
 * are initialized.
 */

void Logger::setup_loggers()
{
    std::string log_dir = get_log_directory();
    Utils::ensure_directory_exists(log_dir);

    auto core_log_path = log_dir + "/core.log";
    auto db_log_path = log_dir + "/db.log";
    auto ui_log_path = log_dir + "/ui.log";
    
    auto core_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto core_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(db_log_path, 1048576 * 5, 3);

    auto db_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto db_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(db_log_path, 1048576 * 5, 3);

    auto ui_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto ui_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(ui_log_path, 1048576 * 5, 3);

    auto core_logger = std::make_shared<spdlog::logger>("core_logger", spdlog::sinks_init_list{core_console_sink, core_file_sink});
    auto db_logger = std::make_shared<spdlog::logger>("db_logger", spdlog::sinks_init_list{db_console_sink, db_file_sink});
    auto ui_logger = std::make_shared<spdlog::logger>("ui_logger", spdlog::sinks_init_list{ui_console_sink, ui_file_sink});

    spdlog::register_logger(core_logger);
    spdlog::register_logger(db_logger);
    spdlog::register_logger(ui_logger);

    spdlog::set_level(spdlog::level::warn);
    spdlog::info("Loggers initialized.");
}


std::shared_ptr<spdlog::logger> Logger::get_logger(const std::string &name) {
    return spdlog::get(name);
}