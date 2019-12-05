//login.c : Upon entry, argv[0] = login, argv[1] = /dev/ttyX
#include "ucode.c"
int in, out, err;
char uname[128], password[128];
char *tokens[7];

int tokenize(char *line, char *buf[], char delim)
{
    char *cp = line;
    int len = 0;

    while (*cp != 0) //while cp is not null
    {
        while (*cp == ' ')
            *cp++ = 0; //make null
        if (*cp != 0)
            buf[len++] = cp;
        while (*cp != delim && *cp != 0)
            cp++;
        if (*cp != 0)
            *cp = 0;
        else
            break;
        cp++;
    }
    return len;
}

main(int argc, char *argv[])
{
    char *lines[128], *acc_info[128], file_lines[1024];

    close(0); //close stdin why?!
    close(1); //close stdout  why?!?!

    in = open(argv[1], 0);
    out = open(argv[1], 1);
    err = open(argv[1], 2);

    settty(argv[1]); //set tty name string in PROC.tty

    while (1)
    {
        int passwd_fd = open("/etc/passwd", O_RDONLY);
        print2f(" ");
        print2f("LOGIN YA PIECE OF SHIT");
        print2f(" ");
        prints("login:");
        gets(uname);
        print2f("password:");
        gets(password);
        //uname[strlen(uname) - 1] = '\0';
        //password[strlen(password) - 1] = '\0';
        read(passwd_fd, file_lines, 1024);

        int len = tokenize(file_lines, lines, '\n');

        for (int i = 0; i < len; i++)   
        {
            tokenize(lines[i], acc_info, ':');
            
            if (strcmp(uname, acc_info[0]) == 0 && strcmp(password, acc_info[1]) == 0)
            {
                // change uid, gid to user's uid, gid;
                chuid(atoi(acc_info[2]), atoi(acc_info[3]));

                // change cwd to users home DIR
                chdir(acc_info[5]);

                // close opened /etc/passwd file
                close(passwd_fd);

                // exec to program in user account // exec
                exec(acc_info[6]);

                break;
            }
        }
        printf("Login failed, try again\n");
    }
}