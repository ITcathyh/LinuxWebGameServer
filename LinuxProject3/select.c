#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	int fd1,fd2;
	fd_set rfds;
	
	if ((fd1 = open("/dev/input/mice", O_RDONLY)) == -1)
	{
		exit(1);
	}
	
	if ((fd2 = open("/dev/pts/0", O_RDONLY)) == -1)
	{
		exit(1);
	}
	
	FD_ZERO(&rfds);
	FD_SET(fd1, &rfds);
	FD_SET(fd2, &rfds);

	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	int retval;
	int n;
	char buf[512];

	while (1)
	{
		retval = select((fd1 > fd2 ? fd1 : fd2) + 1, &rfds, NULL, NULL, &tv);

		if (retval == -1)
		{
			perror("select()");
			exit(1);
		}
		else if (retval)
		{
			printf("Data is available now.\n");
		
			if (FD_ISSET(fd1, &rfds))
			{
				n = read(fd1, buf, sizeof(buf));
				printf("mice:%.*s\n", n, buf);
			}	
			else if (FD_ISSET(fd2, &rfds))
			{
				n = read(fd2, buf, sizeof(buf));
				printf("tty:%.*s\n", n, buf);
			}
		}
	}
	
	return 0;
}