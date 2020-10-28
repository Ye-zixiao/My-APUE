#include "../include/MyAPUE.h"
#include <sys/ioctl.h>

int set_sockaio(int sockfd) {
	int val = 1;

	if (fcntl(sockfd, F_SETOWN, getpid()) == -1)
		return -1;
	if (ioctl(sockfd, FIOASYNC, &val) == -1)
		return -1;
	return 0;
}


int clr_sockaio(int sockfd) {
	int val = 0;

	if (ioctl(sockfd, FIOASYNC, &val) == -1)
		return -1;
	return 0;
}


int main(void) {
	return 0;
}
