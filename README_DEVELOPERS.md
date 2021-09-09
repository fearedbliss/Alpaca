## Developer's Readme

The goal of Alpaca has been to slowly refactor, re-architect, stabilize,
clean up, and improve the PlugY code base and bring it back to its core
and central goal of providing a stash extension for Diablo II (using
a modern compiler toolchain of course), and selectively using simple but
useful C++ features (where needed) rather than having everything written in
ASM/C. This core goal of Alpaca was completed many years ago and Alpaca is now
primarily in maintenance mode. Listed below are some tips that I've gathered for
people that want to continue Alpaca development in their own branches, or fork
Alpaca to use as a base for their mods.

## Supported Versions and Maintainability

Each supported Alpaca version has a completely separate code base. This was
an intentional design decision done to massively increase the maintainability
of the project, rather than keeping the previous "multiple version" support that
PlugY originally had. PlugY was never designed to support multiple versions,
and this is clearly reflected in its code base. Alpaca only supports versions
**`1.09b`** and **`1.10f`** since those are the two main versions I care about,
and were the two major and final versions of the game that were developed
by **`Blizzard North`**. Both source trees are mostly the same with just some
differences in offsets and some version specific assembly.

## Development

- Compiled with Visual Studio 2019.
- All development is done with **`Release`** mode configuration.
- Don't release **`Debug`** builds to the public since most people
  don't have the the debugging DLLs installed. Only use the **`Release`**
  builds since those DLLs will be provided by the Microsoft Redistributables.
- There have been instances where weird and different behaviors happen between
  Release and Debug Modes. Test your stuff properly.
- Don't use **`/GS`** (Security Check).
- Don't use compiler optimizations.
- Use the debugger and compile debugging symbols (.PDB).
- Use static classes and pointer to static functions in static classes
  if attempting to put naked functions in a class. C++ ISO standards
  does not support getting a function pointer to a instanced class function
  and calling it. When in doubt, just stick with regular C.

The reason for the above is because since the code uses a lot of
naked functions, there are times when the code directly maybe cause
an access violation, or something that could be perceived as an overflow.
Sometimes the code only works in Release mode but doesn't work in Debug mode.

Some of the above concerns/restrictions may be removed in the future if we can
continue to reduce the code base and test it properly.

## Tips for updating to a new version of Diablo II

- Look at each of the library header files and see what types of functions they declare.
  You can find all of the offsets currently being used by Alpaca in their corresponding
  implementation files.
  - You can also turn on some of the log printing messages in the Offset Retrieval functions,
    that way you can instantly see all of the offsets being used in the Alpaca.log.
- Search for all references of the Offset Retrieval Functions (Located in Library.h).
  That should further assist with offsets that need to be updated.
- Keep in mind that some offsets are constants and are not dynamically retrieved.
- There are still offsets scattered around the code that haven't yet been consolidated
  into a centralized location (Install Functions, etc, Look for the Memory Class)
- Even with all the offsets updated, there is probably going to be custom ASM code that
  you will need to write for the particular version you want to port for.

and here are a few more tips:

1. Upgrade all DLL offsets to new version (D2Client.cpp/.h, Etc)
1. Upgrade all **`[Patch]`** sections to new version
1. Upgrade all **`[Offset]`** sections to the new versions
1. Upgrade any remaining traces of independent offsets that may still exist in the code.

## Observations

- When using **`GlideWrapper`**, any message send operations that result in a visual change will look weird.
  - This affects the **`players 8`** message (The message displayed is weird,
    workaround is to display above the head of the character).
  - The color that we set for an indexed page in the stash might be rendered differently in Glide.
    (Workaround: Use a color that is rendered the same in both Direct3D and Glide: Green).
  - Overall, I recommend not using GlideWrapper and only using **`cnc-ddraw`** (Which is already included as part of Cactus).

Good Luck!
