#include "MovableCategorizedFile.hpp"
#include "Utils.hpp"
#include <filesystem>
#include <gtk/gtk.h>


/**
 * @brief Constructs a MovableCategorizedFile.
 *
 * @param dir_path The root directory in which the file is located.
 * @param cat The category name.
 * @param subcat The subcategory name.
 * @param file_name The name of the file.
 * @param file_type The type of the file, either "F" for file or "D" for directory.
 *
 * If any of the path components are empty, it throws a std::runtime_error.
 *
 * This function sets the category_path, subcategory_path, and destination_path
 * fields of the object.
 */
MovableCategorizedFile::MovableCategorizedFile(
    const std::string& dir_path, const std::string& cat, const std::string& subcat,
    const std::string& file_name, const std::string& file_type)
    : file_name(file_name),
      file_type(file_type),
      dir_path(dir_path),
      category(cat),
      subcategory(subcat)
{
    if (dir_path.empty() || category.empty() || subcategory.empty() || file_name.empty()) {
        g_print("Error: One or more path components are empty.\n");
        throw std::runtime_error("Invalid path component in CategorizedFile constructor.");
    }

    category_path = std::filesystem::path(dir_path) / category;
    subcategory_path = category_path / subcategory;
    destination_path = subcategory_path / file_name;
}



/**
 * @brief Creates the directories for the category and subcategory.
 *
 * If @a use_subcategory is true, it will attempt to create the subcategory directory
 * as well. If the directories already exist, this function does nothing.
 *
 * If the directories cannot be created, this function will throw a
 * std::filesystem::filesystem_error exception.
 */
void MovableCategorizedFile::create_cat_dirs(bool use_subcategory)
{
    try {
        if (!std::filesystem::exists(category_path)) {
            std::filesystem::create_directory(category_path);
        }
        if (use_subcategory && !std::filesystem::exists(subcategory_path)) {
            std::filesystem::create_directory(subcategory_path);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        g_print("Error creating directories: %s\n", e.what());
        throw;
    }
}


/**
 * Moves the file to the categorized directory based on the specified category and subcategory.
 *
 * @param use_subcategory If true, includes the subcategory in the destination path.
 *         or if the file already exists in the destination path.
 *
 * The function first checks if the source file exists and then attempts to move it to the
 * designated category (and optionally subcategory) directory. If the destination file already
 * exists, the move is aborted. Errors encountered during the move operation are reported.
 */


bool MovableCategorizedFile::move_file(bool use_subcategory)
{
    std::filesystem::path categorized_path;
    if (use_subcategory) {
        categorized_path = std::filesystem::path(dir_path) / category / subcategory;
    } else {
        categorized_path = std::filesystem::path(dir_path) / category;
    }
    std::filesystem::path destination_path = categorized_path / file_name;
    std::filesystem::path source_path = std::filesystem::path(dir_path) / file_name;

    if (!std::filesystem::exists(source_path)) {
        g_print("Error: Source file does not exist: %s\n", Utils::to_cstr(source_path.u8string()));
        return false;
    }

    if (!std::filesystem::exists(destination_path)) {
        try {
            std::filesystem::rename(source_path, destination_path);
            g_print("File %s moved to %s\n", Utils::to_cstr(source_path.u8string()),
                                             Utils::to_cstr(destination_path.u8string()));
            return true;
        } catch (const std::filesystem::filesystem_error& e) {
            g_print("Error moving file: %s\n", e.what());
            return false;
        }
    } else {
        g_print("File %s already exists in %s\n", Utils::to_cstr(source_path.u8string()),
                                                  Utils::to_cstr(destination_path.u8string()));
        return false;
    }
}


/**
 * Retrieves the full path of the subcategory directory as a string.
 *
 * @return A string representation of the subcategory path.
 */

std::string MovableCategorizedFile::get_subcategory_path() const
{
    return subcategory_path.string();
}


/**
 * Retrieves the full path of the category directory as a string.
 *
 * @return A string representation of the category path.
 */
std::string MovableCategorizedFile::get_category_path() const
{
    return category_path.string();
}


/**
 * Retrieves the full path of the file as it would be named in the category
 * directory (and optionally subcategory directory).
 *
 * @return A string representation of the destination path.
 */

std::string MovableCategorizedFile::get_destination_path() const
{
    return destination_path.string();
}


/**
 * Retrieves the original file name of the file.
 *
 * @return A string representing the file name.
 */
std::string MovableCategorizedFile::get_file_name() const
{
    return file_name;
}

/**
 * Retrieves the directory path where the categorized file is located.
 *
 * @return A string representing the directory path.
 */

std::string MovableCategorizedFile::get_dir_path() const
{
    return dir_path;
}


/**
 * Retrieves the category name of the categorized file.
 *
 * @return A string representing the category name.
 */
std::string MovableCategorizedFile::get_category() const
{
    return category;
}
/**
 * Retrieves the subcategory name of the categorized file.
 *
 * @return A string representing the subcategory name.
 */

std::string MovableCategorizedFile::get_subcategory() const
{
    return subcategory;
}

/**
 * Sets the category name of the categorized file.
 *
 * @param category A reference to a string representing the new category name.
 */

void MovableCategorizedFile::set_category(std::string& category)
{
    this->category = category;
}


/**
 * Sets the subcategory name of the categorized file.
 *
 * @param subcategory A reference to a string representing the new subcategory name.
 */
void MovableCategorizedFile::set_subcategory(std::string& subcategory)
{
    this->subcategory = subcategory;
}

MovableCategorizedFile::~MovableCategorizedFile() {}