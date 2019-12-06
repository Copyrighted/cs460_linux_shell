#include "ucode.c"
#define BLK 1024

char *name[16], components[64];
int nk;
int nowait;

char buf[1024];
int color = 0x00C;

char* get_cmd()
{
  
}

void help()
{
  print2f("~~~~~~~~~~~~~~Assistance~~~~~~~~~~~~~~\n\r");
  print2f(" ls   cd    pwd   cat   cp    mv   ps \n\r");
  print2f(" mkdir rmdir creat rm chmod more grep \n\r");
  print2f(" lpr (I/O and Pipe) :  >  >>  <  |    \n\r");
  print2f("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r");
}

int main(int argc, char *argv[])  //~~~~start here~~~~
{
  int pid, status, i;
  char buf[256], tbuf[256], *cp, *cq;

  color = getpid() + 0x000A;  //adjusts color based on PID

  while (1)
  {
    printf("sh %d# ", getpid());  //prints shell number

    gets(buf);  //gets command lines

    if (buf[0] == 0)  //if buf is empty repeat loop
      continue;  

    cp = buf;
    while (*cp == ' ') // skip leading blanks
      cp++;

    cq = cp;
    while (*cq) // zero our trailing blanks
      cq++;
    cq--;
    while (*cq == ' ')
    {
      *cq = 0;
      cq--;
    }

    //printf("input=%s\n", buf);

    if (strcmp(cp, "") == 0) // if nothing or a bunch of spaces
      continue;              //    repeat the while loop

    strcpy(buf, cp);
    strcpy(tbuf, buf);
  
    nk = eatpath(tbuf, name); //tokenizes

    if (strcmp(name[0], "cd") == 0)
    {
      if (name[1] == 0)
        chdir("/");
      else
        chdir(name[1]);
      continue;
    }

    if (strcmp(name[0], "pwd") == 0)
    {
      pwd();
      continue;
    }

    if (strcmp(name[0], "echo") == 0)
    {
      for (i = 1; i < nk; i++)
      {
        printf("%s ", name[i]);
      }
      //printf("\n");
      continue;
    }

    /* chname must be done by sh itself */
    if (strcmp(name[0], "chname") == 0)
    {
      //printf("sh chname to %s\n", cp);
      chname(name[1]);
      continue;
    }

    if (strcmp(name[0], "help") == 0)
    {
      help();
      continue;
    }

    if (strcmp(name[0], "logout") == 0)
    {
      print2f("~GO AWAY\007!~\n\r");
      chdir("/");
      exit(0);
    }

    if (strcmp(name[0], "exit") == 0)
    {
      exit(0);
      continue;
    }

    printf("parent sh %d: fork a child\n", getpid()); //if command is not trivial command forks a child

    pid = fork(); /* sh forks child */

    if (pid)
    { /* parent sh */

      printf("parent sh %d: wait for child %d to die\n", getpid(), pid);
      pid = wait(&status);
      printf("sh %d: child %d exit status = %x\n", getpid(), pid, status);
      continue;
    }
    else
    {
      // printf("child sh %d return to Umode pid=%d\n", getpid(), pid);
      printf("child sh %d running : cmd=%s\n", getpid(), buf);
      do_pipe(buf, 0);
    }
  }
}

// scan breaks up buf = "head | tail" into  "head"  "tail"

int scan(buf, tail) char *buf;
char **tail;
{
  char *p;

  p = buf;
  *tail = 0;

  while (*p) // scan to buf end line
    p++;

  while (p != buf && *p != '|') // scan backward until |
    p--;

  if (p == buf) // did not see any |, so head=buf
    return 0;

  *p = 0;           // change | to NULL
  p++;              // move p right by 1
  while (*p == ' ') // skip over any blanks
    p++;

  *tail = p; // change tail pointer to p

  return 1; // head points at buf; return head
}

/*pipe has a read end and a write end

**/
int do_pipe(char *buf, int *rpd)
{
  int hasPipe, pid;  //haspipe is so we know where pipe is
  char *tail;
  int lpd[2]; 

  if (rpd)
  {
    // as writer on RIGHT side pipe
    close(rpd[0]);
    close(1);
    dup(rpd[1]);
    close(rpd[1]);
  }

  //printf("proc %d do_pipe: buf=%s ", getpid(), buf);
  /*
  if (rpd) print2f("rpd=[%d %d]  ", rpd[0], rpd[1]);
  printf("\n");
  *****************/

  hasPipe = scan(buf, &tail);  //looks to see if theres pipe
  //printf("after scan: buf=%s  tail=%s\n", buf, tail);

  if (hasPipe)
  { // buf=head; tail->tailString
    if (pipe(lpd) < 0)
    { // create LEFT side pipe
      printf("proc %d pipe call failed\n", getpid());
      exit(1);
    }
    // printf("after pipe(): lpd[0|1]=[%d %d]\n", lpd[0], lpd[1]);
    pid = fork();  //parent id and child id, child is left  half of first pipe and parents right side of pipe

    if (pid < 0)
    {
      printf("proc %d fork failed\n", getpid());
      exit(1);
    }

    if (pid)
    { // parent as reader on LEFT side pipe
      close(lpd[1]);
      close(0);  // close the read
      dup(lpd[0]);  //dup is duplication of file descriptor, read from the pipe
      close(lpd[0]);
      printf("proc %d exec %s\n", getpid(), tail);
      command(tail);
    }
    else
    { // child gets LEFT pipe as its RIGHT pipe
      do_pipe(buf, lpd);
    }
  }
  else
  { // no pipe symbol in string must be the leftmost cmd
    command(buf);
  }
  return 1;
}

int command(char *s)
{
  char *name[16], tbuf[64];
  int i, j, nk, I;
  char cmdline[64];

  strcpy(tbuf, s);
  nk = eatpath(tbuf, name);

  I = nk;

  for (i = 0; i < nk; i++)
  {
    if (strcmp(name[i], "<") == 0)
    {
      printf("proc %d redirect input from %s\n", getpid(), name[i + 1]);
      if (I > i)
        I = i; // I = index of first < or > or >>
      if (name[i + 1] == 0)
      {
        printf("invalid < in command line\n\r");
        exit(1);
      }
      close(0);
      if (open(name[i + 1], 0) < 0)
      {
        printf("no such input file\n");
        exit(2);
      }
      break;
    }
  }
  for (i = 0; i < nk; i++)
  {
    if (strcmp(name[i], ">") == 0)
    {
      printf("proc %d redirect outout to %s\n", getpid(), name[i + 1]);
      if (I > i)
        I = i; // I = index of first > or < or >>
      if (name[i + 1] == 0)
      {
        printf("invalid > in command line\n\r");
        exit(3);
      }
      close(1);
      open(name[i + 1], O_WRONLY | O_CREAT);
      break;
    }
  }
  for (i = 0; i < nk; i++)
  {
    if (strcmp(name[i], ">>") == 0)
    {
      printf("proc %d append output to %s\n", getpid(), name[i + 1]);
      if (I > i)
        I = i;
      if (name[i + 1] == 0)
      {
        printf("invalid >> in command line\n\r");
        exit(4);
      }

      close(1);
      //open the string after >> for append
      open(name[i + 1], O_WRONLY | O_CREAT | O_APPEND);
      break;
    }
  }
  //printf("after I/O redirection ");

  strcpy(cmdline, name[0]);
  //printf("cmdline=%s I=%d ", cmdline, I);

  for (j = 1; j < I; j++)
  {
    strcat(cmdline, " ");
    strcat(cmdline, name[j]);
  }
  /********* must write to 2 for correct output redirect **********/
  //print2f("proc %d exec %s\n", getpid(), name[0]);
  //printf("%d before exec\n", getpid());
  if (getpid() < 9)
  {
    if (exec(cmdline) < 0)
      exit(1);
  }

  while (1)
  {
    printf("%d : enter a key : ", getpid());
    getc();
  }
  return 1;
}