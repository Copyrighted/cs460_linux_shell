// #include "ucode.c"
// int console_list[3];

// int parent()
// {
//     int pid, status;
//     while (1)
//     {
//         for (int i = 0; i < 3; i++)
//         {
//             printf("INIT: wait for ZOMBIE child\n");
//             pid = wait(&status);
//             if (pid == console_list[i])
//             { //  if console login process died
//                 printf("INIT: forks a new console login\n");
//                 console_list[i] = 0;
//                 console_list[i] = fork(); //fork another one
//                 if (console_list[i])
//                     continue;
//                 else
//                     exec(ttys[i]); //new console login process
//             }
//             printf("INIT: I just buried an orphan child proc %d\n", pid);
//         }
//     }
// }

// main()
// {
//     int in, out;                       // file descriptors for terminal I/O
//     in = open("/dev/tty0", O_RDONLY);  // file descriptor 0
//     out = open("/dev/tty0", O_WRONLY); // for display to console
//     printf("INIT: fork a login proc on console\n");
//     console_list[0] = fork();
//     if (console_list[0]) // parent
//             console_list[1] = fork();
//         else
//             exec(ttys[0]);
//             if (console_list[1])
//                 console_list[2] = fork();
//             else
//                 exec(ttys[1]);
//                  if (console_list[2])
//                     parent();
//                 else
//                     exec(ttys[2]);
// }
#include "ucode.c"
//char *ttys[3] = {"login /dev/tty0", "login /dev/ttys0", "login /dev/ttys1"};

int console, s0, s1;

void parent()
{
  int pid, status;
  printf("INIT : waiting ...\n");

  while (1)
  {
    //wait the process
    pid = wait(&status);

    if (pid == console)
    {
      //fork a new console
      console = fork();
      //if sh at console dies, run login again for console
      if (console)
        continue;
      else
        exec("login /dev/tty0");
    }
    if (pid == s0)
    {
      //fork new for s0
      s0 = fork();
      //if sh at s0 dies, run login for s0
      if (s0)
        continue;
      else
        exec("login /dev/ttyS0");
    }
    if (pid == s1)
    {
      //fork new  terminal  for s1
      s1 = fork();
      //if sh at s1 dies, run login again for s1
      if (s1)
        continue;
      else
        exec("login /dev/ttyS1");
    }
    //when a process exits, bury it
    printf("INIT: buried an orphan child task %d\n", pid);
  }
}

int main(int argc, char *argv[])
{
  int in, out;
  int pid;

  // open read/write in QEMU console
  in = open("/dev/tty0", O_RDONLY);
  out = open("/dev/tty0", O_WRONLY);

  printf("proc %d in Umode ", getpid());
  printf("argc=%d %s %s\n", argc, argv[0], argv[1]);

  printf("%s\n", "INIT: fork a login task on console");

  //fork a console instance
  console = fork();

  if (console)
  { // parent
    //fork login for other terminals
    s0 = fork();
    if (s0)
    {
      s1 = fork();
      if (s1)
        parent();
      else
        exec("login /dev/ttyS1");
    }
    else
      exec("login /dev/ttyS0");
  }
  else
    exec("login /dev/tty0");
}

