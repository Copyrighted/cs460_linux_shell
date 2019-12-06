#include "ucode.c"
int console;
int console_list[3];

int parent(int console) //P1's code
{
    int pid, status;
    while(1){
        printf("INIT: wait for ZOMBIE child\n");
        pid = wait(&status);
        if (pid == console){  //  if console login process died
            printf("INIT: forks a new console login\n");
            console = fork();  //fork another one
            if (console)
                continue;
            else
                exec("login /dev/tty0");  //new console login process
        }
        printf("INIT: I just buried an orphan child proc %d\n", pid);
    }
}

main()
{
    int in, out;  // file descriptors for terminal I/O
    in = open("/dev/tty0", O_RDONLY);  // file descriptor 0
    out = open("/dev/tty0", O_WRONLY);  // for display to console
    printf("INIT: fork a login proc on console\n");
    for (int i = 0; i < 3; i++){
        console = fork();
        console_list[i] = console;
    }
    if (console)  // parent
        for (int i = 0; i < 3; i++){
            parent(console_list[i]);
        }
    else //child: exec to login on tty0
        exec("login /dev/tty0");
}