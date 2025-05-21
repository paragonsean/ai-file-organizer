#include "EmbeddedEnv.hpp"
#include "Logger.hpp"
#include "MainApp.hpp"
#include "Utils.hpp"
#include <gio/gio.h>
#include <locale.h>
#include <libintl.h>
#include <iostream>
// #include <X11/Xlib.h>
extern GResource *resources_get_resource();


/**
 * Initializes the logging system for the application.
 *
 * This function attempts to set up the loggers by calling Logger::setup_loggers.
 * If the setup is successful, it returns true. If an exception is thrown during
 * the setup process, it catches the exception, logs an error message to the standard
 * error stream, and returns false.
 *
 * @return true if the loggers are successfully initialized, false otherwise.
 */

bool initialize_loggers()
{
    try {
        Logger::setup_loggers();
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Failed to initialize loggers: " << e.what() << std::endl;
        return false;
    }
}


/**
 * The entry point for the application.
 *
 * Initializes logging and environment settings, registers resources, sets locale,
 * and starts the main application. If initialization or execution fails, it logs
 * the error and exits with a failure status.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line argument strings.
 * @return EXIT_SUCCESS on successful execution, or EXIT_FAILURE on error.
 */

int main(int argc, char **argv)
{
    // XInitThreads();
    if (!initialize_loggers()) {
        return EXIT_FAILURE;
    }

    #ifdef _WIN32
        _putenv("GSETTINGS_SCHEMA_DIR=schemas");
    #endif

    MainApp* app;

    try {
        g_resources_register(resources_get_resource());
        EmbeddedEnv env_loader("/net/quicknode/AIFileSorter/.env");
        env_loader.load_env();
        setlocale(LC_ALL, "");
        std::string locale_path = Utils::get_executable_path() + "/locale";
        bindtextdomain("net.quicknode.AIFileSorter", locale_path.c_str());
        app = new MainApp(argc, argv);
        app->run();
        app->shutdown();
        delete app;
    } catch (const std::exception& ex) {
        delete app;
        g_critical("Error: %s", ex.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}