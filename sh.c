//pipes n stuff

int main(int argc, char* argv)
{
    int pid, pd[2];
    pipe(pd);
    pid = fork();
    if (pid){
        close(pd[1]);
        dup2(pd[0], 0);  //redirect stdin to pipe READ end
        exec();  //the fuck is cmd 2
    } else {
        close(pd[0]);
        dup2(pd[1], 1);
        exec();  //exec cmd 1
    }
}