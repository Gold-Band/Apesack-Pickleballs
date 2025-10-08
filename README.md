# Unreal Engine

A Fool's Reign uses **Unreal Engine 5.4.4**

## Building

This project is a Unreal C++ project, meaning you will have to build source before you work on the project.

The following guide is for Windows systems. If you're on Linux, you already know what to do.

1. Install [Visual Studio](https://visualstudio.microsoft.com) (NOT Visual Studio Code). [Rider](https://www.jetbrains.com/rider/) is also an option
2. In the installer, make sure you check C++ features for Unreal Engine before installing. If you already downloaded VS at this point,
you can search for the "Visual Studio Installer". If you're using Rider and don't have VS, download the [build tools](https://visualstudio.microsoft.com/downloads/?q=build+tools) 
(scroll to the bottom > expand "Tools for Visual Studio" > Build Tools for Visual Studio)
3. Follow the instructions for plugins below.
4. In the src/ directory of this project, right click on the `uproject` and click "Generate Visual Studio project files". If you don't see it,
click "Show more options at the bottom of the context menu"
5. Open the `.sln` file. This should open VS (or Rider, if you prefer)
6. At the configuration bar at the top, Make sure that Development Editor and Apesack_Pickleballs (name will change) is selected
7. Build the game! Click the green play button (**not the fully green one!!**) This will take a bit of time if it's your first time

If you're iterating and run into compilation errors, repeat steps 4-7.

# Plugins

This project uses external plugins which you **must install** or
the project **will not run**.

After downloading, extract all the folders (nothing else) into `src/Plugins` (make the folder if it doesn't exist).

Plugin list:

### [FMOD for Unreal](https://www.fmod.com/download#fmodforunreal)

Version 2.03.09 is in use. Be sure to install the correct integration version (**5.4**)
