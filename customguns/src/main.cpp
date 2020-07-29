// IMPORTANT!
// In order to properly build this library, you must provide an ndkpath.txt which points to your ndk-bundle directory
// This is used in the build and debug powershell scripts, which will build/debug for you.
// Author: Sc2ad

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

#include "../../shared/utils/typedefs.h"
#include "../../shared/inline-hook/And64InlineHook.hpp"

#include "../../shared/utils/il2cpp-utils.hpp"
#include "../../shared/utils/utils.h"
#include "../../shared/utils/utils-functions.hpp"
#include "../../shared/utils/logging.h"

#define GAME_PATH "/sdcard/Audica/"
#define CUSTOMIZATION_PATH "customization"
#define CUSTOM_GUNS_PATH "guns"
#define CUSTOM_DRUMS_PATH "target_drums"

bool loadedOnce = false;

MAKE_HOOK_OFFSETLESS(CustomModelLoader_GetCustomModelDir, Il2CppString*, Il2CppObject* self, int type) {
    auto ret = CustomModelLoader_GetCustomModelDir(self, type);
    if (type == 0 || type == 1) {
        // Remap customization to /sdcard
        log(DEBUG, "Redirecting %s to %s", to_utf8(csstrtostr(ret)).data(), GAME_PATH CUSTOMIZATION_PATH "/" CUSTOM_GUNS_PATH);
        auto folder = std::string(GAME_PATH);
        folder += CUSTOMIZATION_PATH;
        if (!fileexists(folder.data())) {
            log(DEBUG, "Folder: %s does not exist, making it!", folder.data());
            mkdir(folder.data(), 0700);
        }
        folder += "/" CUSTOM_GUNS_PATH;
        if (!fileexists(folder.data())) {
            log(DEBUG, "Folder: %s does not exist, making it!", folder.data());
            mkdir(folder.data(), 0700);
        }
        return il2cpp_utils::createcsstr(folder.data());
    }
    return ret;
}

// MAKE_HOOK_OFFSETLESS(AudioDriver_LoadSingleTargetDrums, bool, Il2CppString* targetDrums, Il2CppString* searchRoot, Il2CppString* zipPath) {
//     auto ret = AudioDriver_LoadSingleTargetDrums(targetDrums, searchRoot, zipPath);
//     log(DEBUG, "Redirecting (%s, %s, %s) to %s", to_utf8(csstrtostr(targetDrums)).data(), to_utf8(csstrtostr(searchRoot)).data(), to_utf8(csstrtostr(zipPath)).data(), GAME_PATH CUSTOMIZATION_PATH "/" CUSTOM_DRUMS_PATH);
//     auto folder = std::string(GAME_PATH);
//     folder += CUSTOMIZATION_PATH;
//     if (!fileexists(folder.data())) {
//         log(DEBUG, "Folder: %s does not exist, making it!", folder.data());
//         mkdir(folder.data(), 0700);
//     }
//     folder += "/" CUSTOM_DRUMS_PATH;
//     if (!fileexists(folder.data())) {
//         log(DEBUG, "Folder: %s does not exist, making it!", folder.data());
//         mkdir(folder.data(), 0700);
//     }
//     log(DEBUG, "Original ret: %s", ret ? "true" : "false");
//     return ret;
// }

void InstallHooks() {
    INSTALL_HOOK_OFFSETLESS(CustomModelLoader_GetCustomModelDir, il2cpp_utils::GetMethod("", "CustomModelLoader", "GetCustomModelDir", 1));
    // INSTALL_HOOK_OFFSETLESS(AudioDriver_LoadSingleTargetDrums, il2cpp_utils::GetMethod("", "AudioDriver", "LoadSingleTargetDrums", 3));
    log(INFO, "Installed Custom Guns!");
}

extern "C" void load() {
    InstallHooks();
}
