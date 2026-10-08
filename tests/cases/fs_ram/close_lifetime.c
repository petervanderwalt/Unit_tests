#include "hal.h"
#include "fs_ram.h"
#include "vfs.h"
#include "check.h"
int main(void)
{
    fs_ram_mount();
    vfs_file_t *file = vfs_open("/ram/close", "w");
    CHECK(file != NULL);
    CHECK(vfs_write("hello", 1, 5, file) == 5);
    vfs_close(file);
    return EXIT_SUCCESS;
}
