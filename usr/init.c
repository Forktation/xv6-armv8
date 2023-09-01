// init: The initial user-level program

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#ifndef LOOP(X)
#define LOOP(X) for(;;) { X }
#endif

char* argv[] = { "sh", 0 };

int main(void) {
    int pid, wpid;

    if (open("console", O_RDWR) < 0) {
        mknod("console", 1, 1);
        open("console", O_RDWR);
    }
    dup(0);  // stdout
    dup(0);  // stderr

    printf(1, "\n");
    printf(1, "**************************************************************************\n");
    printf(1, "**                                                                      **\n");
    printf(1, "**                                                                      **\n");
    printf(1, "**                  xv6 on ARMv8-A (64-bit) Architecture                **\n");
    printf(1, "**                                                                      **\n");
    printf(1, "**                                                                      **\n");
    printf(1, "**************************************************************************\n");
    printf(1, "\n");

    for (;;) {
        printf(1, "init: Starting Shell\n");
        pid = fork();
        if (pid < 0) {
            printf(1, "init: fork failed\n");
            exit();
        }
        if (pid == 0) {
            exec("sh", argv);
            printf(1, "init: exec sh failed\n");
            exit();
        }
        while ((wpid = wait()) >= 0 && wpid != pid)
            printf(1, "zombie!\n");
    }
}
