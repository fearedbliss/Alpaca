## Alpaca - 4.2.0
##### By: Jonathan Vasquez (fearedbliss)

## Description

Alpaca is a Simple Stash Extension for **`Diablo II: 1.09b & 1.10`**
that was based off Yohann Nicolas' **[PlugY](http://plugy.free.fr/en/index.html) 11.02**.
It has been massively refactored, stabilized, has full support for
**`Diablo II Classic`**, and is fully compatible with Necrolis' Native OpenGL Diablo II Driver:
**[D2GFEx](https://d2mods.info/forum/viewtopic.php?f=5&t=65312)**. All previous **`PlugY`** bugs
that were related to **`Multiplayer / LAN`** have also been fixed.

Alpaca's primary focus is to provide more storage. Nothing else. Therefore, for the
best experience, you should combine Alpaca with [Singling](https://github.com/fearedbliss/Cactus/blob/master/README-SINGLING.md).

As always, when using any type of mod, make sure to always keep backups!

## Status

This project is in maintenance mode and provided as is. I'll only be accepting
bug fixes from the community. Furthermore, I recommend not using Alpaca and
instead just using [Cactus' Character Isolation](https://www.youtube.com/watch?v=kAFfc-kjTxE)
feature in combination with Singling, to provide multiple instances and transfer
items via LAN. This allows you to still transfer items in game, reduces the number of
dependencies you need to play, and significantly increases the stability of the game
since you don't have all of this extra code running.

## Compatibility

- [Cactus](https://github.com/fearedbliss/Cactus) -
  A Modern Diablo II Version Switcher (Contains: Singling)
- [Singling](https://github.com/fearedbliss/Cactus/blob/master/README-SINGLING.md) -
  A collection of non-gameplay modifications and fixes in
  order to improve the Vanilla Diablo II Single Player Experience.
- [D2GFEx](https://d2mods.info/forum/viewtopic.php?f=5&t=65312) - Native OpenGL Driver For Diablo II

## Requirements

- [Microsoft Visual C++ 2019 Redistributable (x86)](https://aka.ms/vs/16/release/VC_redist.x86.exe)

## License

Released under the [GNU General Public License v3](LICENSE.md) or Later.

## Features

#### Core

- Increased Stash Size to **`(10x10)`**.
- **`1000`** Additional Stash Pages Per Character. No Shared Stash.
    - To transfer items between characters, use Singling's Multiple Instance
      Support to open multiple copies of the game, make a LAN game, and join it
      with any characters that want to trade. Singling is part of
      [Cactus](https://github.com/fearedbliss/Cactus).
- Diablo II **`Classic`** and **`Expansion`** Support.
- Full **`Multiplayer / LAN`** Support.
- **[D2GFEx](https://d2mods.info/forum/viewtopic.php?f=5&t=65312)** Support.

#### Stash Management Commands

- **`/rename <name>`** : Renames the current stash page. (If left blank, deletes the page name).
- **`/set`** : Marks the current stash page with an index.
- **`/unset`** : Removes the current stash page's index.
- **`/swap <page #>`** : Swaps the content of the current page with the content of the target page.
- **`/page <page #>`** : Takes you to the corresponding page.

## Installation Instructions ([Video](https://www.youtube.com/watch?v=w6cmsAOO1Qg))

### Installation (Standalone)

1. Copy all the files in the **`1. Files`** folder for the version you want to play to the root of your Diablo II folder.
1. Start up **`Alpaca.exe`**.

### Installation [(Cactus)](https://github.com/fearedbliss/Cactus)

1. Copy all the files in the **`1. Files`** folder for the version you want to play to the equivalent version in your **`Platforms`** folder.
1. Uncheck the **`Last Ran`** box for whatever entry is selected (if any).
1. Edit the entry for your target version and change the **`Path`** to point to **`Alpaca.exe`**.
1. Start up the target platform.

### Installation [(D2GFEx)](https://d2mods.info/forum/viewtopic.php?f=5&t=65312)

1. Copy all the files in the **`1. Files`** folder for the version you want to play to the root of your Diablo II folder.
2. Open **`D2GFEx.ini`** and add **`Alpaca.dll`** to one of the **`LoadDll`** lines.
3. Start up **`Game.exe`**.

### Installation (Other)

1. Copy all the files in the **`1. Files`** folder for the version you want to play to the root of your Diablo II folder.
1. Open your injector configuration and add **`Alpaca.dll`** to whatever dll loading lines it uses.
1. Start up the game via whatever method necessary.

Alpaca has only been tested for compatibility with the Alpaca Launcher and D2GFEx.
It should work with other systems but compatibility is not guaranteed.

## Troubleshooting

### Read Process Memory Failed

If you get a **`Read Process Memory Failed`** error when starting Diablo II
with **`Alpaca`**, you'll need to restart your machine until it works. This error
also occurs with **`PlugY`** as well, however **`Alpaca`** has a bit of extra
logic to try and reduce the chance of this occurring.

### MSVCP140.dll is Missing or Alpaca seems to not load

You may possibly be missing the **`Microsoft Visual C++ 2019 Redistributable (x86)`**
that was mentioned in the **`Requirements`** section above. Install that and try again.

## Screenshots

![Main Menu 09](https://i.imgur.com/Aypyp8i.jpg)

![Main Menu 10](https://i.imgur.com/4yVSEIB.jpg)

![Classic](https://i.imgur.com/lg039Rd.jpg)

![Expansion](https://i.imgur.com/PAjQ03J.jpg)

![Page Names / Indexed Pages](https://i.imgur.com/wniPrfA.jpg)