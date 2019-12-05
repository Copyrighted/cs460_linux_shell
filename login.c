//login.c : Upon entry, argv[0] = login, argv[1] = /dev/ttyX
#include "ucode.c"
int in, out, err;
char uname[128], password[128];
char *tokens[7];

void tokenize(char *line)
{
    char *cp;
    cp = line;

    argc = 0;

    while (*cp != 0)  //while cp is not null
    {
        while (*cp == ':')
            *cp++ = 0; //make null
        if (*cp != 0) 
            tokens[argc++] = *cp;
        while (*cp != ':' && *cp != 0)
            cp++;
        if (*cp != 0)
            *cp = 0;
        else
            break;
        cp++;
    }
    tokens[argc] = 0;
}

main(int argc, char *argv[])
{
    close(0); //close stdin
    close(1); //close stdout
    in = open(argv[1], 0);
    out = open(argv[1], 1), err = open(argv[1], 2);
    settty(argv[1]); //set tty name string in PROC.tty
    int fd = open("/etc/passwd", O_RDONLY);
    while (1)
    {
        prints("login:");
        gets(uname);
        print2f("password:");
        gets(password);
        uname[strlen(uname) - 1] = '\0';
        password[strlen(password) - 1] = '\0';
        char line[256];

        while (getline_fd(line, fd))
        {

            //for each line in etc/pass file do
            //tokenize user account line
            //print2f("tokenizing");
            tokenize(line);
            //printf("username %s\n", tokens[0]);
            //printf("password %s\n", tokens[1]);
            if ((strcmp(tokens[0], uname) == 0) && (strcmp(tokens[1], password) == 0))
            { //username:password:gid:uid:fullname:HOMEDIR:program
                prints("FUCK YOU");
                //change uid, gid to user's uid, gid  //chuid
                chuid(atoi(tokens[3]), atoi(tokens[2]));
                //change cwd to users home DIR
                chdir(tokens[6]);
                //close opened /etc/password file
                close(fd);
                //exec to program in user account
                exec(tokens[7]);
            }
        }
    }
    printf("Login failed, try again\n");
}