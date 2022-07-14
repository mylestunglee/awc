#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#ifdef __cplusplus
#define getchar() 0
#else
#include <stdio.h>
#endif

char getch(void) {
    struct termios prev, curr;

    tcgetattr(STDIN_FILENO, &prev); /* grab prev terminal i/o settings */
    curr = prev;             /* make curr settings same as prev settings */
    curr.c_lflag &= ~ICANON; /* disable buffered i/o */
    curr.c_lflag &= ~ECHO;   /* set no echo mode */
    tcsetattr(STDIN_FILENO, TCSANOW,
              &curr); /* use these new terminal i/o settings now */

    char ch = getchar();
    tcsetattr(0, TCSANOW, &prev);
    return ch;
}

void get_console_size(int* const width, int* const height) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *width = w.ws_col;
    *height = w.ws_row;
}
