#include <termios.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

char getch(void) {
	struct termios old, new;

	tcgetattr(STDIN_FILENO, &old); /* grab old terminal i/o settings */
	new = old; /* make new settings same as old settings */
	new.c_lflag &= ~ICANON; /* disable buffered i/o */
	new.c_lflag &= ~ECHO; /* set no echo mode */
	tcsetattr(STDIN_FILENO, TCSANOW, &new); /* use these new terminal i/o settings now */

	char ch = getchar();
	tcsetattr(0, TCSANOW, &old);
	return ch;
}

void get_console_size(int* const width, int* const height) {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	*width = w.ws_col;
	*height = w.ws_row;
}
