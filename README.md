# Cursor Context Menu Installer

This project provides a simple way to add and remove a context menu item in Windows Explorer for opening directories with the Cursor application.

[Follow my X!](https://x.com/hexcreator) ![x icon](https://upload.wikimedia.org/wikipedia/commons/thumb/c/cc/X_icon.svg/12px-X_icon.svg.png)

## Installation

To install the context menu item, run the `install` executable. By default, it will use the path `C:\Users\%USERNAME%\AppData\Local\Programs\cursor\Cursor.exe` for the Cursor application. You can also provide a custom path to the Cursor executable as a command-line argument.

## Compilation
I used MinGW to compile this project, but you can use any other compiler that supports C++. Example commands:

```bash
g++ -o install install.cpp -static
g++ -o uninstall uninstall.cpp -static
```

## Usage Instructions

To use the context menu item, follow these steps:

1. **Download the Executables:**
   - Visit the [releases page](https://github.com/hexcreator/open-with-cursor/releases) of this project.
   - Download the `install.exe` and `uninstall.exe` files from the latest release.

2. **Install the Context Menu Item:**
   - Run the `install.exe` file.
   - By default, it will use the path `C:\Users\%USERNAME%\AppData\Local\Programs\cursor\Cursor.exe` for the Cursor application.
   - If you want to use a custom path for the Cursor executable, provide the path as a command-line argument when running `install.exe`.
   - You can also provide the command-line argument `-a` to install the context menu item for all files, not just directories.

3. **Uninstall the Context Menu Item:**
   - Run the `uninstall.exe` file to remove the context menu item.

