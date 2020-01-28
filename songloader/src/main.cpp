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

#define CUSTOM_SONGS_PATH "/storage/emulated/0/Android/data/com.harmonixmusic.kata/"
#define CUSTOM_SONGS_FOLDER "CustomSongs"

bool loadedOnce = false;

MAKE_HOOK_OFFSETLESS(SongList_AddSongSearchDir, void, Il2CppString* root, Il2CppString* dir) {
    if (!loadedOnce) {
        loadedOnce = true;
        log(DEBUG, "Adding %s - %s to search dir!", CUSTOM_SONGS_PATH, CUSTOM_SONGS_FOLDER);
        auto folder = std::string(CUSTOM_SONGS_PATH);
        folder += CUSTOM_SONGS_FOLDER;
        if (!fileexists(folder.data())) {
            log(DEBUG, "Folder: %s does not exist, making it!", folder.data());
            mkdir(folder.data(), 0700);
        }
        struct dirent *dp;
        DIR *dir = opendir(folder.data());
        if (dir == NULL) {
            log(ERROR, "Loading songs(%s): null dir! errno: %i, msg: %s", folder.data(), errno, strerror(errno));
            return;
        }

        while ((dp = readdir(dir)) != NULL)
        {
            if (strlen(dp->d_name) > 7 && !strcmp(dp->d_name + strlen(dp->d_name) - 7, ".audica"))
            {
                std::string full_path(folder.data());
                full_path += "/";
                full_path.append(dp->d_name);
                log(DEBUG, "Adding song: %s to search dir!", full_path.data());
                SongList_AddSongSearchDir(il2cpp_utils::createcsstr(full_path), il2cpp_utils::createcsstr(""));
            }
        }
        closedir(dir);
    }
    log(DEBUG, "Adding song search dir for: root: %s dir: %s", to_utf8(csstrtostr(root)).data(), to_utf8(csstrtostr(dir)).data());
    SongList_AddSongSearchDir(root, dir);
}

MAKE_HOOK_OFFSETLESS(SongList_ProcessSingleSong, bool, Il2CppObject* self, Il2CppObject* root, Il2CppString* found, Il2CppObject* songHash) {
    log(DEBUG, "Processing a single song. Found: %s", to_utf8(csstrtostr(found)).data());
    return SongList_ProcessSingleSong(self, root, found, songHash);
}

void InstallHooks() {
    INSTALL_HOOK_OFFSETLESS(SongList_AddSongSearchDir, il2cpp_utils::GetMethod("", "SongList", "AddSongSearchDir", 2));
    INSTALL_HOOK_OFFSETLESS(SongList_ProcessSingleSong, il2cpp_utils::GetMethod("", "SongList", "ProcessSingleSong", 3));
    log(INFO, "Installed SongLoader!");
}

extern "C" void load() {
    InstallHooks();
}
