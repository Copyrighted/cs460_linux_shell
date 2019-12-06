#include "ucode.c"
int console;
int console_list[3];
char *ttys[3] = {"login /dev/tty0", "login /dev/ttys1", "login /dev/ttys2"};

int parent()
{
    int pid, status;
    while (1)
    {
        for (int i = 0; i < 3; i++)
        {
            printf("INIT: wait for ZOMBIE child\n");
            pid = wait(&status);
            if (pid == console_list[i])
            { //  if console login process died
                printf("INIT: forks a new console login\n");
                console_list[i] = 0;
                console_list[i] = fork(); //fork another one
                if (console_list[i])
                    continue;
                else
                    exec(ttys[i]); //new console login process
            }
            printf("INIT: I just buried an orphan child proc %d\n", pid);
        }
    }
}

main()
{
    int in, out;                       // file descriptors for terminal I/O
    in = open("/dev/tty0", O_RDONLY);  // file descriptor 0
    out = open("/dev/tty0", O_WRONLY); // for display to console
    printf("INIT: fork a login proc on console\n");
    console_list[0] = fork();
    if (console_list[0]) // parent
        console_list[1] = fork();
    else
        exec(ttys[0]);
    if (console_list[1])
        console_list[2] = fork();
    else
        exec(ttys[1]);
    if (console_list[2])
        parent();
    else
        exec(ttys[2]);
}
