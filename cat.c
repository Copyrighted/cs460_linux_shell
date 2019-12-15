
#include "ucode.c"

char newline = '\n';
char CR = '\r';

int main(int argc, char *argv[])
{
	int fd, i, n;
	char *cp;
	char buf[1024];

	print2f("Tyler's obese cat\n");

	fd = 0;
	if (argc > 1)  //if we have more than one arg
	{
		fd = open(argv[1], O_RDONLY);  //open what youre passing in
		if (fd < 0)  //if it doesnt work throw error
		{
			printf("cat %s error\n", argv[1]);
			exit(0);
		}
	}

	if (argc < 2) //for cat w no args
	{
		while (gets(buf))  //get the buf
		{
			print2f(buf);  //print it out
			print2f("\n\r");
		}
		exit(0);
	}

	while ((n = read(fd, buf, 1024)))  //while reading from file
	{
		buf[n] = 0;  //setting null terminator to end of buf
		cp = buf;  //setting up cp
		if (fd)  //if fd is open
		{
			for (int i = 0; i < n; i++)  //for each byte youre writing byte by byte to stdout
			{
				write(1, &buf[i], 1);
				if (buf[i] == '\n')  //if new line youre writing carriage return
					write(2, &CR, 1);  
			}
		}
		else
		{
			cp = buf;  //set cp to buf
			if (*cp == '\r')  //check if it equals carriage return
				write(2, &newline, 1);  //literally write new line to stdout
			write(1, cp, 1);  //write new line to stdin
		}
	}
	
	close(fd);
	exit(0);
}

