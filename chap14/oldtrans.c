#include "../include/MyAPUE.h"
#include <ctype.h>
#include <fcntl.h>

unsigned char buf[BUFSIZE];

unsigned char translate(unsigned char ch) {
	if (isalpha(ch)) {
		if (ch >= 'n')
			ch -= 13;
		else if (ch >= 'a')
			ch += 13;
		else if (ch >= 'N')
			ch -= 13;
		else
			ch += 13;
	}
	return ch;
}


int main(int argc, char* argv[])
{
	int ifd, ofd;
	int rval, wval;
	
	if (argc != 3)
		err_quit("usage: rot13 infile outfile");
	if ((ifd = open(argv[1], O_RDONLY)) < 0)
		err_sys("can't open file %s", argv[1]);
	if ((ofd = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, FILE_MODE)) < 0)
		err_sys("can't create file %s", argv[2]);

	while ((rval = read(ifd, buf, BUFSIZE)) > 0) {
		for (int i = 0; i < rval; ++i)
			buf[i] = translate(buf[i]);
		if ((wval = write(ofd, buf, rval)) != rval) {
			if (wval < 0)
				err_sys("write error");
			else
				err_quit("short write (%d/%d)", wval / rval);
		}
	}

	close(ifd);
	close(ofd);
	exit(EXIT_SUCCESS);
}
