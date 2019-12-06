
#include "ucode.c"



char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";
struct stat mystat, *sp;

int ls_file(char *fname, char *name)  //lists a single file
{
  char * ret;
  char ch = '/';
  struct stat fstat, *sp;
  int r, i, in;
  char ftime[64];
  char temName[100];
  sp = &fstat;

  //printf("ls_file \n\r");
  

  r= stat(fname, &fstat);
  //printf(" this is r : %d\n\r",r);

  if (r<0)
  {
    printf("Stat Error ");
    prints(fname);
    printf("\n\r");
    exit(1);
  }
  
  if ((sp->st_mode & 0170000)==0100000)//if (sp->st_mode==0X8124) // file
  {
    printc('-');
  }
  if((sp->st_mode & 0170000)==0040000)//if(sp->st_mode==0X41ED) // dir
  {
    printc('d');
  }
  if((sp->st_mode & 0170000)==0120000) //symbol link 
  {
    printc('l');
  }

  for(i=8; i>=0; i--)
  {
    if(sp->st_mode &(1<<i))
    {
      printc(t1[i]);
    }
    else
    {
      printc(t2[i]);
    }
  }

  printf("    %d ",sp->st_nlink);
  printf("  %d ",sp->st_uid);
  printf("  %d ",sp->st_gid);

  //printf(" %d ",sp->st_size);
  i = 0;
  while(name[i]!='\0')
  {
    i++;
  }
  //printi(i);
  strcpy(temName,name);
  temName[i]=0;
  prints(temName);
  printf("\n\r");
  
  //strcpy(ftime, ctime(&sp->st_ctime));

}

int ls_dir(char *dname)
{
  printf("ls_dir\n\r");
  char name[256], tempName[256];  // EXT2 filename: 1-255 chars
  char buf[1024];  
  struct dirent *ep;
  char *cp;   
  DIR *dp;
  int dir_fd;
  int i =0;
 // open DIR to read names

  dir_fd = open(dname,O_RDONLY);   //get directory file descriptor

  read(dir_fd,buf,1024); //read dir into buf
  cp = buf;
  dp = (DIR *) cp;
  while(cp < &buf[1024])
  {
    
    strncpy(tempName,dp->name,dp->name_len);
    strcpy(name, dname);
    strcat(name,"/");
    strcat(name,tempName);
    ls_file(name,tempName);

    
    cp+= dp->rec_len;
    dp=(DIR*)cp;
  }
  //prints(dp->name);
  close (dir_fd);
  printf("-----ls dir done\n\r");

 
}




int main (int argc, char *argv[])
{

  struct stat mystat, *sp = &mystat;  //stat file status info
  int r;
  char *s;
  char filename[1024], cwd[1024], path[1024];


  if (argc==1) // no parameter
    strcpy(filename,"./");
  
  else
  {
    strcpy(path, argv[1]);
  }

  if ((stat(filename, sp)) < 0) {  //stat() syscall
      print2f("Error when stat'ing in ls"); exit(1);
  }

  if((sp->st_mode & 0xF000) == 0x4000) //if (S_ISDIR(sp->st_mode))
  {
    ls_dir(filename);
  }
  else if ((sp->st_mode & 0xF000) == 0x8000)
  {
    ls_file(filename,filename);
  }

  exit(0);

}
