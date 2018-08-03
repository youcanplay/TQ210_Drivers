#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
	int char_dev_fd = -1;
	char buf[8];

	char_dev_fd = open("/dev/char_dev", 0666);
	if(char_dev_fd < 0) {
		char_dev_fd = open("/dev/char_dev", 0666);
		if(char_dev_fd < 0) {
			perror("open");
			exit(1);
		}
	}

	write(char_dev_fd, buf, 1);
	read(char_dev_fd, buf, 1);

	close(char_dev_fd);
	
	return 0;
}