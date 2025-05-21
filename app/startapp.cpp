#include <iostream>
#include <cstdlib>
#include <shlobj.h>
#include <string>
#include <windows.h>


/**
 * Retrieves the directory path of the executable file.
 *
 * @return A string representing the directory path of the executable.
 */
std::string getExecutableDirectory() {
    WCHAR buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring wpath(buffer);
    size_t pos = wpath.find_last_of(L"\\/");
    std::wstring dir = wpath.substr(0, pos);
    return std::string(dir.begin(), dir.end());
}


/**
 * Adds the specified directory to the PATH environment variable.
 *
 * This function retrieves the current PATH environment variable, appends the given
 * directory to it, and updates the PATH variable in the current process. If any
 * errors occur while retrieving or updating the PATH, an error message is printed
 * to the standard error stream.
 *
 * @param directory The directory to be added to the PATH environment variable.
 */

void addToPath(const std::string& directory) {
    char* pathEnv = nullptr;
    size_t requiredSize = 0;

    getenv_s(&requiredSize, nullptr, 0, "PATH");
    if (requiredSize == 0) {
        std::cerr << "Failed to retrieve PATH environment variable." << std::endl;
        return;
    }

    pathEnv = new char[requiredSize];
    getenv_s(&requiredSize, pathEnv, requiredSize, "PATH");

    std::string newPath = std::string(pathEnv) + ";" + directory;
    delete[] pathEnv;

    // Update PATH in the current process
    if (_putenv_s("PATH", newPath.c_str()) != 0) {
        std::cerr << "Failed to set PATH environment variable." << std::endl;
    } else {
        std::cout << "Updated PATH: " << newPath << std::endl;
    }
}


/**
 * Launches the main application executable.
 *
 * This function attempts to launch the main application executable (bin/AI File Sorter.exe)
 * using the WinExec function. If the launch fails, an error message is printed to the standard
 * error stream.
 */
void launchMainApp() {
    std::string exePath = "bin\\AI File Sorter.exe";
    if (WinExec(exePath.c_str(), SW_SHOW) < 32) {
        std::cerr << "Failed to launch the application." << std::endl;
    }
}


/**
 * The entry point for the startapp executable.
 *
 * This function is responsible for setting the current directory to the executable's
 * directory, adding the "lib" subdirectory to the PATH environment variable, and
 * launching the main application executable.
 *
 * @param hInstance The instance handle passed to the executable.
 * @param hPrevInstance The previous instance handle passed to the executable.
 * @param lpCmdLine The command line arguments passed to the executable.
 * @param nCmdShow The show window command passed to the executable.
 *
 * @return An exit status indicating whether the executable succeeded or failed.
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    std::string exeDir = getExecutableDirectory();

    if (!SetCurrentDirectoryA(exeDir.c_str())) {
        std::cerr << "Failed to set current directory: " << exeDir << std::endl;
        return EXIT_FAILURE;
    }

    std::string dllPath = exeDir + "\\lib";
    addToPath(dllPath);
    launchMainApp();
    return EXIT_SUCCESS;
}