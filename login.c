//login.c : Upon entry, argv[0] = login, argv[1] = /dev/ttyX
#include "ucode.c"
int in, out, err;
char name[128], password[128];
char *tokens[7];

void tokenize(char* line){

    for(int i = 0; i < 7; i++)
    {   
        tokens[i] = line;
        while(*line != ':' && *line != '\0')
        {
            printf("%c", *line);
            ++line;
        }
    *line = '\0';
    line++;    
    }
}

main(int argc, char *argv[])
{
    close(0);
    close(1);
    in = open(argv[1], 0);
    out = open(argv[1], 1), err = open(argv[1], 2);
    settty(argv[1]); //set tty name string in PROC.tty
    int passwd_file = open("/etc/passwd", O_RDONLY);
    while (1)
    {
        printf("login:");
        gets(name);
        printf("password:");
        gets(password);
        name[strlen(name) - 1] = '\0';
        password[strlen(password) - 1] = '\0';
        char line[256];
        while (getline(line))
        {
            //for each line in etc/pass file do
            //tokenize user account line
            tokenize(line);
            if (strcmp(tokens[0], name) == 0 && strcmp(tokens[1], password) == 0)
            {
                
                //change uid, gid to user's uid, gid  //chuid
                chuid();
                //change cwd to users home DIR
                chdir();
                //close opened /etc/password file
                close();
                //exec to program in user account
                exec();
            }
        }
    }
    printf("Login failed, try again\n");
}