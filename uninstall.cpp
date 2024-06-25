#include <windows.h>
#include <iostream>

bool DeleteRegistryKey(HKEY hKeyParent, LPCSTR subKey) {
    LONG result = RegDeleteTreeA(hKeyParent, subKey);
    if (result != ERROR_SUCCESS) {
        std::cerr << "Error deleting registry key: " << result << std::endl;
        return false;
    }
    return true;
}

bool UninstallContextMenu() {
    std::string subKey = "Directory\\Background\\shell\\Open with Cursor";
    if (!DeleteRegistryKey(HKEY_CLASSES_ROOT, subKey.c_str())) {
        return false;
    }
    return true;
}

int main() {
    if (UninstallContextMenu()) {
        std::cout << "Context menu item uninstalled successfully." << std::endl;
    } else {
        std::cerr << "Failed to uninstall context menu item." << std::endl;
    }

    return 0;
}
