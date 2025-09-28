# Gothic 2 Night of The Raven: MultiProcess

This plugin allows to run the game in more than one instance.   
It can be really useful for debugging and testing multiplayer modifications.

**NOTE!** This plugin works only with `Gothic 2 Night of the Raven` with `2.6 fix`.

## How to use

**_NOTE:_** At this moment the plugins doesn't work with Union.  

You need an ASI Loader to use this mod:

1. Rename `game/system/binkw32.dll` to `game/system/binkw32hooked.dll`. 
2. Download [Ultimate ASI Loader for binkw32.dll](https://github.com/user-attachments/files/22582498/binkw32.zip) and unpack it in `game/system` directory. 

To install the plugin, you have to:

1. [Download](https://github.com/Patrix9999/MultiProcess/releases) a release.
2. Create `game/system/plugins` subfolder and unpack the file into it.
3. Start the game. The plugin will load automatically.

To uninstall the plugin remove `MultiProcess.asi` from your `plugins` directory. 

# Contribute

The official repository of this project is available at: https://github.com/Patrix9999/MultiProcess

Compile it yourself
Download Visual Studio 2017 or newer with the C++ workload. The project is configured to build it with the Windows 10 compatible v142 toolchain. However, you should be able to change the toolchain to whatever you like. No additional libraries are required so it should compile out of the box.
