//login.c : Upon entry, argv[0] = login, argv[1] = /dev/ttyX
#include "ucode.c"
int in, out, err;
char uname[128], password[128];
char *tokens[7];

//line is char pointer to whole file
//used KC's CRTC0 for tokenizing stuff
int tokenize(char *line, char *buf[], char delim)
{
    char *cp = line;
    int len = 0;

    while (*cp != 0) //while cp is not null
    {
        while (*cp == ' ') //while whitespace 
            *cp++ = 0; //assigns null then increments
        if (*cp != 0)
            buf[len++] = cp;  //this is setting this to the first non null non whitespace char
        while (*cp != delim && *cp != 0)  //keep incrementing while we see good ole chars
            cp++;
        if (*cp != 0)  //this is making end of string NULL instead of \n
            *cp = 0;  
        else
            break;
        cp++;
    }
    return len;
}   

main(int argc, char *argv[])
{
    char *lines[256], *account_info[256], file_lines[1024];

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
            tokenize(lines[i], account_info, ':');
            
            if (strcmp(uname, account_info[0]) == 0 && strcmp(password, account_info[1]) == 0)
            {
                chuid(atoi(account_info[2]), atoi(account_info[3])); // change uid, gid to user's uid, gid;

                
                chdir(account_info[5]);  //change cwd to users home DIR

                close(passwd_fd);  // close opened /etc/passwd file

                exec(account_info[6]);  // exec to program in user account // exec


                break;
            }
        }
        printf("Login failed, try again\n");
    }
}