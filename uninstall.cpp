#include <windows.h>
#include <iostream>
#include <string>

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

bool DeleteRegistryKey(HKEY hKeyParent, LPCSTR subKey) {
    LONG result = RegDeleteTreeA(hKeyParent, subKey);
    if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND) {
        std::cerr << "Error deleting registry key: " << result << std::endl;
        return false;
    }
    return true;
}

bool UninstallContextMenu() {
    bool success = true;

    // Remove context menu for right-clicking inside a folder
    std::string insideFolderSubKey = "Directory\\Background\\shell\\Open with Cursor";
    success &= DeleteRegistryKey(HKEY_CLASSES_ROOT, insideFolderSubKey.c_str());

    // Remove context menu for right-clicking on a folder
    std::string onFolderSubKey = "Directory\\shell\\Open with Cursor";
    success &= DeleteRegistryKey(HKEY_CLASSES_ROOT, onFolderSubKey.c_str());

    // Remove context menu for all files
    std::string allFilesSubKey = "*\\shell\\Open with Cursor";
    success &= DeleteRegistryKey(HKEY_CLASSES_ROOT, allFilesSubKey.c_str());

    return success;
}

int main(int argc, char* argv[]) {
    if (!IsElevated())
    {
        RunAsAdmin(argv[0], (argc > 1) ? argv[1] : "");
        return 0;
    }

    if (UninstallContextMenu()) {
        std::string successMsg = "Context menu items uninstalled successfully.";
        MessageBoxA(NULL, successMsg.c_str(), "Success", MB_OK | MB_ICONINFORMATION);
    } else {
        std::string errorMsg = "Failed to uninstall some or all context menu items.";
        MessageBoxA(NULL, errorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
    }

    return 0;
}
