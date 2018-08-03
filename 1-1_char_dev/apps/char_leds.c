#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
	int on; //led status
	int led_no; //led number

	int leds_fd = -1;

	if(argc != 3 || sscanf(argv[1], "%d", &led_no) != 1 || 
		sscanf(argv[2], "%d", &on) != 1 || on < 0 || on > 1 ||
		led_no < 0 || led_no > 1) {
		fprintf(stderr, "Usage: char_leds led_no 0|1\n");
		exit(1);
	}

	leds_fd = open("/dev/leds", 0666);
	if(leds_fd < 0) {
		leds_fd = open("/dev/led", 0666);
		if(leds_fd < 0) {
			perror("open");
			exit(1);
		}
	}

	ioctl(leds_fd, on, led_no);

	close(leds_fd);
	
	return 0;
}