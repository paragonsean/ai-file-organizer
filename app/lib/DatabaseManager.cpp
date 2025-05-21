#include "DatabaseManager.hpp"
#include "Settings.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <sqlite3.h>
#include <unistd.h>
#include <glib.h>
#include <Types.hpp>


/**
 * Constructs a DatabaseManager object and initializes the SQLite database.
 * 
 * @param config_dir The directory path where the database file is located.
 * 
 * Initializes the database connection using the provided configuration directory.
 * If the environment variable "CATEGORIZATION_CACHE_FILE" is set, it uses its value
 * as the database file name; otherwise, defaults to "categorization_results.db".
 * If the database file path is empty or the database cannot be opened, an error
 * message is printed. Ensures that the 'file_categorization' table exists in the
 * database, creating it if necessary, with columns for file name, type, directory path,
 * category, subcategory, and a timestamp, with a unique constraint on file name, type,
 * and directory path.
 */

DatabaseManager::DatabaseManager(std::string config_dir) :
    config_dir(config_dir),
    db_file(config_dir + "/" + 
            (std::getenv("CATEGORIZATION_CACHE_FILE") 
             ? std::getenv("CATEGORIZATION_CACHE_FILE") 
             : "categorization_results.db"))
{
    if (db_file.empty()) {
        std::cerr << "Error: Database path is empty" << std::endl;
        return;
    }
    
    if (sqlite3_open(db_file.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    const char* create_table_sql = R"(
        CREATE TABLE IF NOT EXISTS file_categorization (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            file_name TEXT NOT NULL,
            file_type TEXT NOT NULL,
            dir_path TEXT NOT NULL,
            category TEXT NOT NULL,
            subcategory TEXT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            UNIQUE(file_name, file_type, dir_path)
        );
    )";

    char* error_msg = nullptr;
    if (sqlite3_exec(db, create_table_sql, nullptr, nullptr, &error_msg) != SQLITE_OK) {
        std::cerr << "Failed to create table: " << error_msg << std::endl;
        sqlite3_free(error_msg);
    }
}


/**
 * Destructor for the DatabaseManager class.
 *
 * Closes the SQLite database connection if it is open to ensure
 * proper resource management and to prevent memory leaks.
 */

DatabaseManager::~DatabaseManager() {
    if (db) {
        sqlite3_close(db);
    }
}


/**
 * Inserts a new entry into the database or updates an existing entry if it already exists.
 *
 * @param file_name The name of the file or directory to be categorized.
 * @param file_type The type of file, either FileType::File or FileType::Directory.
 * @param dir_path The directory path where the file or directory is located.
 * @param category The top-level category assigned to the file or directory.
 * @param subcategory The subcategory assigned to the file or directory.
 *
 * @return true if the operation was successful, false otherwise.
 */
bool DatabaseManager::insert_or_update_file_with_categorization(const std::string& file_name,
                                                                const std::string& file_type,
                                                                const std::string& dir_path, 
                                                                const std::string& category, 
                                                                const std::string& subcategory) {
    const char *sql = R"(
        INSERT INTO file_categorization (file_name, file_type, dir_path, category, subcategory)
        VALUES (?, ?, ?, ?, ?)
        ON CONFLICT(file_name, file_type, dir_path)
        DO UPDATE SET category = excluded.category, subcategory = excluded.subcategory;
    )";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        g_print("SQL error: %s\n", sqlite3_errmsg(db));
        return false;
    }

    sqlite3_bind_text(stmt, 1, file_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, file_type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, dir_path.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, category.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, subcategory.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_print("SQL error during insert or update: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}


/**
 * Retrieves a list of categorized files from the database for a given directory path.
 *
 * @param directory_path The directory path to filter categorized files by.
 * 
 * @return A vector of CategorizedFile objects, each containing metadata such as directory path,
 *         file name, file type, category, and subcategory.
 *
 * This function executes an SQL query to select categorized files from the 'file_categorization'
 * table where the directory path matches the specified parameter. If the directory path cannot
 * be bound or the query preparation fails, an empty vector is returned. The query results
 * are transformed into CategorizedFile objects which are then collected into a vector.
 */

std::vector<CategorizedFile>
DatabaseManager::get_categorized_files(const std::string& directory_path)
{
    std::vector<CategorizedFile> categorized_files;
    const char *sql = "SELECT dir_path, file_name, file_type, category, subcategory FROM file_categorization WHERE dir_path=?;";
    sqlite3_stmt *stmtcat;

    if (sqlite3_prepare_v2(db, sql, -1, &stmtcat, nullptr) != SQLITE_OK) {
        // g_print("SQL error: %s\n", sqlite3_errmsg(db));
        return categorized_files;
    }

    if (sqlite3_bind_text(stmtcat, 1, directory_path.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Failed to bind directory_path: " << sqlite3_errmsg(db) << std::endl;
        return categorized_files;
    }

    int rc;
    while ((rc = sqlite3_step(stmtcat)) == SQLITE_ROW) {
        const char* file_dir_path = reinterpret_cast<const char*>(sqlite3_column_text(stmtcat, 0));
        const char* file_name = reinterpret_cast<const char*>(sqlite3_column_text(stmtcat, 1));
        const char* file_type = reinterpret_cast<const char*>(sqlite3_column_text(stmtcat, 2));
        const char* category = reinterpret_cast<const char*>(sqlite3_column_text(stmtcat, 3));
        const char* subcategory = reinterpret_cast<const char*>(sqlite3_column_text(stmtcat, 4));

        // Ensure null safety for strings
        std::string dir_path = file_dir_path ? file_dir_path : "";
        std::string name = file_name ? file_name : "";
        std::string type_str = file_type ? file_type : "";
        std::string cat = category ? category : "";
        std::string subcat = subcategory ? subcategory : "";

        FileType file_type_enum = (type_str == "F") ? FileType::File : FileType::Directory;

        categorized_files.push_back({dir_path, name, file_type_enum, cat, subcat});
    }

    sqlite3_finalize(stmtcat);

    return categorized_files;
}


/**
 * Retrieves the categorization of a file from the database.
 *
 * @param file_name The name of the file to query.
 * @param file_type The type of the file to query (file or directory).
 *
 * @return A vector of two strings, where the first element is the category and the second element is the subcategory.
 *         If the query fails or the file does not exist in the database, an empty vector is returned.
 */
std::vector<std::string>
DatabaseManager::get_categorization_from_db(const std::string& file_name, const FileType file_type)
{
    std::vector<std::string> categorization;
    const char *sql = "SELECT category, subcategory FROM file_categorization WHERE file_name = ? AND file_type = ?;";
    sqlite3_stmt *stmtcat;

    if (sqlite3_prepare_v2(db, sql, -1, &stmtcat, nullptr) != SQLITE_OK) {
        g_print("SQL error: %s\n", sqlite3_errmsg(db));
        return categorization;
    }

    if (sqlite3_bind_text(stmtcat, 1, file_name.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Failed to bind file_name: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmtcat);
        return categorization;
    }

    std::string file_type_str = (file_type == FileType::File) ? "F" : "D";

    if (sqlite3_bind_text(stmtcat, 2, file_type_str.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Failed to bind file_type: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmtcat);
        return categorization;
    }

    int rc;
    if ((rc = sqlite3_step(stmtcat)) == SQLITE_ROW) {
        const char* category = reinterpret_cast<const char*>(sqlite3_column_text(stmtcat, 0));
        const char* subcategory = reinterpret_cast<const char*>(sqlite3_column_text(stmtcat, 1));

        // std::cout << "Got categorization from DB: " << category << " " << subcategory << std::endl;

        categorization.push_back(category ? category : "");
        categorization.push_back(subcategory ? subcategory : "");
    }

    sqlite3_finalize(stmtcat);

    return categorization;
}