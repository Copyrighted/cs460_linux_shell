#include "ucode.c"
#define BLK 1024

char *name[16];
int nk;
char buf[1024];

void help()
{
    print2f("~~~~~~~~~~~~~~Assistance~~~~~~~~~~~~~~\n\r");
    print2f(" ls   cd    pwd   cat   cp    mv   ps \n\r");
    print2f(" mkdir rmdir creat rm chmod more grep \n\r");
    print2f("                >  >>  <  |           \n\r");
    print2f("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r");
}

int main(int argc, char *argv[]) //~~~~start here~~~~
{
    int pid, status, i;
    char buf[256], tempBuf[256], *cp, *cq;

    while (1)
    {
        printf("sh %d# ", getpid()); //prints shell number

        gets(buf); //gets command lines

        if (buf[0] == 0) //if buf is empty repeat loop
            continue;

        cp = buf;  //set cp to buf
        while (*cp == ' ') //skip leading blanks
            cp++;

        cq = cp;
        while (*cq) //zero our trailing blanks
            cq++;
        cq--;
        while (*cq == ' ')
        {
            *cq = 0;
            cq--;
        }

        if (strcmp(cp, "") == 0) // if nothing or a bunch of spaces
            continue;            //    repeat the while loop

        strcpy(buf, cp);
        strcpy(tempBuf, buf);

        nk = eatpath(tempBuf, name); //eatpath separates words and puts it in name char array, pass tempbuf so we dont fuck up buf

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
            continue;
        }

        /* chname must be done by sh itself */
        if (strcmp(name[0], "chname") == 0)
        {
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
            print2f("~GO AWAY!~\n\r");
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
        { /* parent sh */ //parents gonna wait

            printf("parent sh %d: wait for child %d to die\n", getpid(), pid);
            pid = wait(&status);
            printf("sh %d: child %d exit status = %x\n", getpid(), pid, status);
            continue; //once parent dies continue
        }
        else //childs gonna go here
        {
            printf("child sh %d running : cmd=%s\n", getpid(), buf);
            do_pipe(buf, 0); //pass commands/make sure cmd is empty
        }
    }
}

//scan breaks up buf = "head | tail" into  "head"  "tail"
int scan(char *buf, char **tail)
{
    char *p;    

    p = buf;  //point p to buf
    *tail = 0;  //set tail to 0

    while (*p) //get to end of buf
        p++;

    while (p != buf && *p != '|') //while we're not at front of buf and not at |  GOTO |
        p--;

    if (p == buf) //if at front of buf return 0  means no pipe
        return 0;

    *p = 0;           // change | to NULL
    p++;              // move p right by 1
    while (*p == ' ') // skip over any blanks
        p++;

    *tail = p; // change tail pointer to p

    return 1; // head points at buf; return head
}

//does single pipes
int do_pipe(char *buf, int *cmd)
{
    //pd ~ pipe itself
    char *tail; //output for pipe
    int pid, pd[2], hasPipe;

    hasPipe = scan(buf, &tail);

    if (hasPipe)  //read is 0 write is 1
    {   
        pid = fork(); //fork for writer, parent will be reader

        //print2f("\n\nENTERING HASPIPE\n\n");
        pipe(pd);       
        if (pid) //parent as pipe reader
        {
            close(pd[1]); //close writer for parent 
            dup2(pd[0], 0);  //dup2 is taking control of stdout
            command(tail);
        }
        else
        { //writer
            close(pd[0]);
            dup2(pd[1], 1);  //dup2 is taking control of stdin
            command(buf);
        }
    } 
    else 
    {
        command(buf);  //if no pipe
    }
    return 1;
}

/*
takes in tail and head for pipe
also does redirect
*/
int command(char *s)
{
    char *name[16], tempBuf[64];
    int i, j, nk, I;  //, used to check if double carrot
    char cmdline[64];

    strcpy(tempBuf, s);

    nk = eatpath(tempBuf, name);  //eats tempBuf and throws it all in name

    I = nk;  

    for (i = 0; i < nk; i++)
    {
        if (strcmp(name[i], "<") == 0) 
        {
            printf("proc %d redirect input from %s\n", getpid(), name[i + 1]);
            if (I > i)  //if I > then index
                I = i; // I = index of either < or > or >>
            if (name[i + 1] == 0) //if nothing to take in from
            {
                print2f("invalid < attempt\n\r");
                exit(1);
            }
            close(0);
            if (open(name[i + 1], 0) < 0)
            {
                print2f("no such input file\n");
                exit(2);
            }
            break;
        }
    }
    for (i = 0; i < nk; i++)
    {
        if (strcmp(name[i], ">") == 0)  //outputs to an existing file
        {
            printf("proc %d redirect outout to %s\n", getpid(), name[i + 1]);
            if (I > i)
                I = i; // I = index of first > or < or >>

            if (name[i + 1] == 0)  //if nothing to output to
            {
                print2f("invalid > attempt\n\r");
                exit(3);
            }
            close(1);
            open(name[i + 1], O_WRONLY | O_CREAT);
            break;
        }
    }
    for (i = 0; i < nk; i++)  //append and creat if file doesnt exist yet
    {
        if (strcmp(name[i], ">>") == 0)
        {
            printf("proc %d append output to %s\n", getpid(), name[i + 1]);
            if (I > i)
                I = i;
            if (name[i + 1] == 0)
            {
                print2f("invalid >>\n\r");
                exit(4);
            }

            close(1);
            //open the string after >> for append
            open(name[i + 1], O_WRONLY | O_CREAT | O_APPEND);  
            break;
        }
    }

    strcpy(cmdline, name[0]);

    for (j = 1; j < I; j++)
    {
        strcat(cmdline, " ");
        strcat(cmdline, name[j]);
    }

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