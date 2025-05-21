#include "Settings.hpp"
#include <filesystem>
#include <iostream>
#include <glib.h>
#ifdef _WIN32
    #include <shlobj.h>
    #include <windows.h>
#endif


/**
 * Constructor for the Settings class.
 *
 * Initializes the settings with default values, sets the configuration path, and 
 * ensures the configuration directory exists. If the configuration directory does 
 * not exist, it attempts to create it, handling any filesystem errors that occur.
 * The default sort folder is set to the user's download directory, falling back to 
 * the home directory if not available. The sort folder is initialized to the default 
 * sort folder.
 */

Settings::Settings()
    : use_subcategories(true),
      categorize_files(true),
      categorize_directories(false),
      default_sort_folder(""),
      sort_folder("")
{
    const std::string app_name = "AIFileSorter";
    config_path = define_config_path();
    config_dir = std::filesystem::path(config_path).parent_path();

    try {
        if (!std::filesystem::exists(config_dir)) {
            std::filesystem::create_directories(config_dir);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to create configuration directory: " << e.what() << std::endl;
    }

    default_sort_folder = g_get_user_special_dir(G_USER_DIRECTORY_DOWNLOAD);
    if (!default_sort_folder) {
        default_sort_folder = g_get_home_dir();
    }
    sort_folder = default_sort_folder;
}

/**
 * Defines the path to the configuration file.
 *
 * The path is determined by the platform:
 * - On Windows, the file is stored in the user's AppData directory.
 * - On MacOS, the file is stored in the user's Application Support directory.
 * - On Linux, the file is stored in the user's .config directory.
 * If the directory does not exist, it is created.
 *
 * @return The full path to the configuration file.
 */
std::string Settings::define_config_path()
{
    std::string AppName = "AIFileSorter";
#ifdef _WIN32
    char appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        return std::string(appDataPath) + "\\" + AppName + "\\config.ini";
    }
#elif defined(__APPLE__)
    return std::string(getenv("HOME")) + "/Library/Application Support/" + AppName + "/config.ini";
#else
    return std::string(getenv("HOME")) + "/.config/" + AppName + "/config.ini";
#endif
    return "config.ini";
}

/**
 * Retrieves the path to the configuration directory.
 *
 * @return A string representing the configuration directory path.
 */
std::string Settings::get_config_dir()
{
    return config_dir.string();
}


/**
 * Loads the settings from the configuration file.
 *
 * If the file does not exist or cannot be loaded, it sets the sort folder to the default sort folder
 * and returns false. Otherwise, it loads the values from the configuration file and returns true.
 *
 * @return True if the settings were successfully loaded, false otherwise.
 */
bool Settings::load() {
    if (!config.load(config_path)) {
        sort_folder = default_sort_folder ? default_sort_folder : "/";
        return false;
    }

    use_subcategories = config.getValue("Settings", "UseSubcategories", "false") == "true";
    categorize_files = config.getValue("Settings", "CategorizeFiles", "true") == "true";
    categorize_directories = config.getValue("Settings", "CategorizeDirectories", "false") == "true";
    sort_folder = config.getValue("Settings", "SortFolder", default_sort_folder ? default_sort_folder : "/");
    skipped_version = config.getValue("Settings", "SkippedVersion", "0.0.0");

    return true;
}


/**
 * Saves the current settings to the configuration file.
 *
 * This function saves the settings to the configuration file specified by the
 * config_path member variable. It returns true if the save is successful, and
 * false otherwise.
 *
 * @return True if the settings were successfully saved, false otherwise.
 */
bool Settings::save() {
    config.setValue("Settings", "UseSubcategories", use_subcategories ? "true" : "false");
    config.setValue("Settings", "CategorizeFiles", categorize_files ? "true" : "false");
    config.setValue("Settings", "CategorizeDirectories", categorize_directories ? "true" : "false");
    config.setValue("Settings", "SortFolder", this->sort_folder);

    if (!skipped_version.empty()) {
        config.setValue("Settings", "SkippedVersion", skipped_version);
    }

    return config.save(config_path);
}


/**
 * Retrieves the current setting for whether to use subcategories.
 *
 * @return True if subcategories are currently in use, false otherwise.
 */
bool Settings::get_use_subcategories() const
{
    return use_subcategories;
}


/**
 * Sets the setting for whether to use subcategories.
 *
 * @param value True if subcategories should be used, false otherwise.
 */
void Settings::set_use_subcategories(bool value)
{
    use_subcategories = value;
}


/**
 * Retrieves the current setting for whether to categorize files.
 *
 * @return True if files should be categorized, false otherwise.
 */
bool Settings::get_categorize_files() const
{
    return categorize_files;
}


/**
 * Sets the setting for whether to categorize files.
 *
 * @param value True if files should be categorized, false otherwise.
 */
void Settings::set_categorize_files(bool value)
{
    categorize_files = value;
}


/**
 * Retrieves the current setting for whether to categorize directories.
 *
 * @return True if directories should be categorized, false otherwise.
 */

bool Settings::get_categorize_directories() const
{
    return categorize_directories;
}


/**
 * Sets the setting for whether to categorize directories.
 *
 * @param value True if directories should be categorized, false otherwise.
 */
void Settings::set_categorize_directories(bool value)
{
    categorize_directories = value;
}


/**
 * Retrieves the current sort folder path.
 *
 * @return A string representing the path to the sort folder.
 */

std::string Settings::get_sort_folder() const
{
    return sort_folder;
}


/**
 * Sets the current sort folder path.
 *
 * @param path A string representing the path to the new sort folder.
 */
void Settings::set_sort_folder(const std::string &path)
{
    this->sort_folder = path;
}


/**
 * Sets the skipped version setting.
 *
 * This setting is used to determine if the user has chosen to skip an update.
 * If the application version is less than or equal to the skipped version,
 * the update dialog will not be shown.
 *
 * @param version A string representation of the version to be skipped.
 */
void Settings::set_skipped_version(const std::string &version) {
    skipped_version = version;
}


/**
 * Retrieves the version that the user has chosen to skip.
 *
 * @return A string representation of the skipped version.
 */
std::string Settings::get_skipped_version()
{
    return skipped_version;
}