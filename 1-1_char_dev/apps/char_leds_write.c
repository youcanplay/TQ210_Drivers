#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
	int on;
	int led_on;
	int fd;
	int ret;
	char allon[2] = {1, 1};
	char alloff[2] = {0, 0};

	fd = open("/dev/leds", O_RDWR);
	if(fd < 0) {
		fd = open("/dev/led", O_RDWR);
		if(fd < 0) {
			perror("open");
			exit(1);
		}
	}
	
	ret = write(fd, allon, 2);
	if(ret == -1) {
		perror("write");
		exit(1);
	}
	sleep(5);
	ret = write(fd, alloff, 2);
	if(ret == -1) {
		perror("write");
		exit(1);
	}

	close(fd);
	
	return 0;
}
