#include "ucode.c";


int main(int argc, char *argv[])
{
    char buf[1024];

    int fd_src, fd_dest, n_bytes;

    if (argc < 3)
    {
        print2f("cp: missing file operands\n");
        exit(0);
    }

    fd_src = open(argv[1], O_RDONLY);
    fd_dest = open(argv[2], O_WRONLY|O_CREAT);

    if (fd_src < 0 || fd_dest < 0){
        print2f("Invalid source or destination\n");
        exit(2);
    }

    while(n_bytes = read(fd_src, buf, 1024))
    {
        write(fd_dest, buf, n_bytes);
    }

    close(fd_src);
    close(fd_dest);

    exit(0);
    
    
}