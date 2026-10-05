#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

// This is optionnal, will default to "build"
// #define BUILD_FOLDER "path/to/build/folder"
// This is optionnal, will default to "Libraries/raylib"
// #define RAYLIB_PATH "./raylib"
// The file will include the implementation by default, if making another noblib_*
// that uses raylib(exemple: rlImGui) and needs it's includes and flags, do :
// #define RAYLIB_NOIMPLEMENTATION
#include "noblib_raylib.c"

#define LAB_NAME "serveur"
#define EXE_NAME "server"

#define LAB_NAME "serveur"
#define EXE_NAME "server"

#define RESEAU_LFLAGS "-lws2_32"
int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    // ^--- Only needed if you intend on changing the build file, use NOB_GO_REBUILD_URSELF otherwise
    if (!mkdir_if_not_exists("Deployment"))
    {
        return 1;
    }
    if (!mkdir_if_not_exists("build"))
    {
        return 1;
    }
    // ^---- UPDATE THE PATH IF LIBRARIES ARE SOMEWHERE ELSE
    nob_shift_args(&argc, &argv);
    char *name = nob_shift_args(&argc, &argv);
    printf("%s\n", name);
    File_Paths o_files = {0};

    Cmd cmd = {0};

    nob_cc(&cmd);
    cmd_append(&cmd, "-ggdb3");
    nob_cc_inputs(&cmd, "./src/main.cpp", temp_sprintf("./src/%s.cpp", name));
    // cmd_append(&cmd,RAYLIB_INCLUDES);
    for (int i = 0; i < o_files.count; ++i)
    {
        cmd_append(&cmd, o_files.items[i]);
    }
    nob_cc_output(&cmd, temp_sprintf("./Deployment/%s.exe", name));
    cmd_append(&cmd, RESEAU_LFLAGS);
    // cmd_append(&cmd,RAYLIB_LFLAGS);
    if (!cmd_run_sync_and_reset(&cmd))
        return 1;
}