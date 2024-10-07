#include <windows.h>
#include <string>
#include <iostream>
#include <shellapi.h>
#include <shlobj.h>

// Function to check if the current process is running with elevated privileges
bool IsElevated()
{
    BOOL isElevated = FALSE;
    HANDLE token = NULL;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
    {
        TOKEN_ELEVATION elevation;
        DWORD size;
        if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size))
        {
            isElevated = elevation.TokenIsElevated;
        }
        CloseHandle(token);
    }
    return isElevated;
}

// Function to run a process as an administrator
void RunAsAdmin(const char* path, const char* params)
{
    SHELLEXECUTEINFOA sei = { sizeof(sei) };
    sei.lpVerb = "runas";
    sei.lpFile = path;
    sei.lpParameters = params;
    sei.hwnd = NULL;
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExA(&sei))
    {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_CANCELLED)
        {
            std::cerr << "The operation was cancelled by the user." << std::endl;
        }
        else
        {
            std::cerr << "Error running as admin: " << dwError << std::endl;
        }
        exit(1);
    }
}

// Function to create a registry key and set its value
bool CreateRegistryKey(HKEY hKeyParent, LPCSTR subKey, LPCSTR valueName, LPCSTR data)
{
    HKEY hKey;
    LONG result = RegCreateKeyExA(hKeyParent, subKey, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    if (result != ERROR_SUCCESS)
    {
        std::cerr << "Error creating registry key: " << result << std::endl;
        return false;
    }

    result = RegSetValueExA(hKey, valueName, 0, REG_SZ, (const BYTE*)data, strlen(data) + 1);
    if (result != ERROR_SUCCESS)
    {
        std::cerr << "Error setting registry value: " << result << std::endl;
        RegCloseKey(hKey);
        return false;
    }

    RegCloseKey(hKey);
    return true;
}

std::string GetCursorPath()
{
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path)))
    {
        std::string fullPath = std::string(path) + "\\Programs\\cursor\\Cursor.exe";
        return fullPath;
    }
    return "";
}

bool FileExists(const std::string& path)
{
    DWORD attrib = GetFileAttributesA(path.c_str());
    return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool InstallContextMenu(const std::string& exePath, bool installForAllFiles)
{
    if (!FileExists(exePath))
    {
        std::cerr << "Error: File not found: " << exePath << std::endl;
        return false;
    }

    // Add context menu for right-clicking inside a folder (existing behavior)
    std::string insideFolderSubKey = "Directory\\Background\\shell\\Open with Cursor";
    std::string insideFolderCommandKey = insideFolderSubKey + "\\command";

    if (!CreateRegistryKey(HKEY_CLASSES_ROOT, insideFolderSubKey.c_str(), NULL, "Open with Cursor"))
    {
        return false;
    }
    if (!CreateRegistryKey(HKEY_CLASSES_ROOT, insideFolderSubKey.c_str(), "Icon", exePath.c_str()))
    {
        return false;
    }
    std::string insideFolderCommandValue = "\"" + exePath + "\" \"%V\"";
    if (!CreateRegistryKey(HKEY_CLASSES_ROOT, insideFolderCommandKey.c_str(), NULL, insideFolderCommandValue.c_str()))
    {
        return false;
    }

    // Add context menu for right-clicking on a folder
    std::string onFolderSubKey = "Directory\\shell\\Open with Cursor";
    std::string onFolderCommandKey = onFolderSubKey + "\\command";

    if (!CreateRegistryKey(HKEY_CLASSES_ROOT, onFolderSubKey.c_str(), NULL, "Open with Cursor"))
    {
        return false;
    }
    if (!CreateRegistryKey(HKEY_CLASSES_ROOT, onFolderSubKey.c_str(), "Icon", exePath.c_str()))
    {
        return false;
    }
    std::string onFolderCommandValue = "\"" + exePath + "\" \"%1\"";
    if (!CreateRegistryKey(HKEY_CLASSES_ROOT, onFolderCommandKey.c_str(), NULL, onFolderCommandValue.c_str()))
    {
        return false;
    }

    // Add context menu for all files if requested
    if (installForAllFiles)
    {
        std::string allFilesSubKey = "*\\shell\\Open with Cursor";
        std::string allFilesCommandKey = allFilesSubKey + "\\command";

        if (!CreateRegistryKey(HKEY_CLASSES_ROOT, allFilesSubKey.c_str(), NULL, "Open with Cursor"))
        {
            return false;
        }
        if (!CreateRegistryKey(HKEY_CLASSES_ROOT, allFilesSubKey.c_str(), "Icon", exePath.c_str()))
        {
            return false;
        }
        std::string allFilesCommandValue = "\"" + exePath + "\" \"%1\"";
        if (!CreateRegistryKey(HKEY_CLASSES_ROOT, allFilesCommandKey.c_str(), NULL, allFilesCommandValue.c_str()))
        {
            return false;
        }
    }

    return true;
}

int main(int argc, char* argv[])
{
    bool installForAllFiles = false;
    if (!IsElevated())
    {
        RunAsAdmin(argv[0], (argc > 1) ? argv[1] : "");
        return 0;
    }

    std::string exePath = GetCursorPath();
    for (int i = 1; i < argc; i++)
    {
        const std::string arg = std::string(argv[i]);
        if (arg == "-a" || arg == "--all")
        {
            installForAllFiles = true;
        }
        else {
            exePath = arg;
        }
    }

    if (exePath.empty())
    {
        MessageBoxA(NULL, "Failed to determine Cursor.exe path.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    if (InstallContextMenu(exePath, installForAllFiles))
    {
        std::string successMsg = "Context menu item installed successfully.\nPath: " + exePath;
        MessageBoxA(NULL, successMsg.c_str(), "Success", MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        std::string errorMsg = "Failed to install context menu item.\nAttempted path: " + exePath;
        MessageBoxA(NULL, errorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
    }

    return 0;
}
