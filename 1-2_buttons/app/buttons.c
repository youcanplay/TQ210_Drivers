#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int fd;

	fd = open("/dev/buttons", O_RDWR);
	if(fd < 0) {
		perror("open");
		exit(1);
	}

	close(fd);

	return 0;
}

