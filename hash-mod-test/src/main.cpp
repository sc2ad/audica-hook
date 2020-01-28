// IMPORTANT!
// In order to properly build this library, you must provide an ndkpath.txt which points to your ndk-bundle directory
// This is used in the build and debug powershell scripts, which will build/debug for you.
// Author: Sc2ad

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "../../shared/utils/typedefs.h"
#include "../../shared/inline-hook/And64InlineHook.hpp"

#include "../../shared/utils/il2cpp-utils.hpp"
#include "../../shared/utils/utils.h"
#include "../../shared/utils/utils-functions.hpp"
#include "../../shared/utils/logging.h"

MAKE_HOOK_OFFSETLESS(SongList_AddSongSearchDir, void, Il2CppString* root, Il2CppString* dir) {
    log(DEBUG, "Adding song search dir for: root: %s dir: %s", to_utf8(csstrtostr(root)).data(), to_utf8(csstrtostr(dir)).data());
    SongList_AddSongSearchDir(root, dir);
}

static bool installed = false;
void InstallHooks() {
    if (installed) return;
    INSTALL_HOOK_OFFSETLESS(SongList_AddSongSearchDir, il2cpp_utils::GetMethod("", "SongList", "AddSongSearchDir", 2));
    log(INFO, "Installed TestMod!");
    installed = true;
}

extern "C" void load() {
    InstallHooks();
}
