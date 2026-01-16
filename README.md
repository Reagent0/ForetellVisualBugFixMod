# ForetellVisualBugFixMod
UE4SS Mod for Clair Obscur: Expedition 33 that fixes a (mostly but not only visual) bug that triggers when trying to apply Foretell to a target that already has more Foretell than the currently allowed cap. Check [this document](BUG_INFO.md) for more info on the bug.

## Installation  
This mod is based on the [UE4SS Scripting System](https://github.com/UE4SS-RE/RE-UE4SS), as such, you need to install that first.

#### Installing UE4SS  
Step 1: Download UE4SS ([UE4SS_v3.0.1-848-g91b70e5.zip](https://github.com/UE4SS-RE/RE-UE4SS/releases/download/experimental/UE4SS_v3.0.1-848-g91b70e5.zip)).  
Step 2: Locate the directory containing the E33 **actual** executable, aka the install directory. **IMPORTANT:** The executable you're looking for should be called `SandFall-Win64-Shipping.exe`. If you've installed the game from Steam, the directory should be `C:\Program Files (x86)\Steam\steamapps\common\Expedition 33\Sandfall\Binaries\Win64`.  
Step 3: Once you have located the install directory, extract the contents of the zip you downloaded in Step 1 in there. The install directory should now have a new ue4ss folder and a dwmapi.dll file. If you did everything correctly, good! UE4SS is installed.

#### Installing ForetellVisualBugFixMod  
For clarity purposes, I'll refer to the path to the directory you located in the previous phase as `InstallDirectory`.  
Step 1: Download the [latest release of ForetellVisualBugFixMod](https://github.com/Reagent0/ForetellVisualBugFixMod/releases/latest/download/ForetellVisualBugFixMod.zip).  
Step 2: Open the zip folder you just downloaded and extract its content (a ForetellVisualBugFixMod folder) to `InstallDirectory\ue4ss\Mods`.  
Step 3: Open the file `InstallDirectory\ue4ss\Mods\mods.txt` and add in a new line `ForetellVisualBugFixMod : 1`. This will tell UE4SS to load the mod.

##### All done. If you did everything correctly and UE4SS is injected, a log file should generate in path `InstallDirectory\ue4ss\UE4SS.log` after launching the game. Look for something like `ForetellVisualBugFixMod loaded!` to ensure the mod is loaded.

### For those who come after.