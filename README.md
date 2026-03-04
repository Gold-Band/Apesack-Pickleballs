# Plugins

This project uses external plugins which you **must install** or
the project **will not run**.

After downloading, extract all the folders (nothing else) into `src/Plugins` (make the folder if it doesn't exist).

Plugin list:

### [FMOD for Unreal](https://www.fmod.com/download#fmodforunreal)

Version 2.03.09 is in use.

# Using the terminal 😎
To make sh life easier, [just](https://just.systems/man/en/) is a simple command runner. Don't worry, it's a basic 1.75MB CLI tool
> [!NOTE]
> A video if you wanna be entertained lol: [I'm never writing another Makefile ever again](https://youtu.be/_aQ8xJ5DuHY)

Install it from your package manager:

```sh
# arch
yay just

# mint
apt install just

# macos
brew install just

# windows
winget install --id casey.just
```

Simply execute `just` to view available commands:
```sh
$ just
Available recipes:
    clean # Deletes Binaries directory
    open  # Opens the project
```

Executing a command:
```sh
$ just open
Opening project
```

**If `just open` failed:**
It's because it could not find where the `UnrealEditor` is. Simply create a **`.env` at the project root** and specify the UE_PATH variable:
```sh
# PROJECT_ROOT/.env
UE_PATH="~/UE/Engine/Binaries/Linux/UnrealEditor"

# windows example:
UE_PATH="C:/Program Files/Epic Games/UE_5.4/Engine/Binaries/Win64/UnrealEditor.exe"
```

> [!NOTE]
> The `.env` file is ignored by git. Don't expect it to be replicated
