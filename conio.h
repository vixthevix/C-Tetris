//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>
//#include <sys/select.h>
//#include <termios.h>
//
//struct termios orig_termios;
//
//void reset_terminal_mode()
//{
//    tcsetattr(0, TCSANOW, &orig_termios);
//}
//
//void set_conio_terminal_mode()
//{
//    struct termios new_termios;
//
//    /* take two copies - one for now, one for later */
//    tcgetattr(0, &orig_termios);
//    memcpy(&new_termios, &orig_termios, sizeof(new_termios));
//
//    /* register cleanup handler, and set the new terminal mode */
//    atexit(reset_terminal_mode);
//    cfmakeraw(&new_termios);
//    tcsetattr(0, TCSANOW, &new_termios);
//}
//
//int kbhit()
//{
//    struct timeval tv = { 0L, 0L };
//    fd_set fds;
//    FD_ZERO(&fds);
//    FD_SET(0, &fds);
//    return select(1, &fds, NULL, NULL, &tv) > 0;
//}
//
//int getch()
//{
//    int r;
//    unsigned char c;
//    if ((r = read(0, &c, sizeof(c))) < 0) {
//        return r;
//    } else {
//        return c;
//    }
//}

//#include <stdio.h>
//#include <sys/ioctl.h>
//#include <unistd.h>
//
//int kbhit(void)
//{
//    int k;
//
//    ioctl(STDIN_FILENO,FIONREAD,&k);
//
//    return(k);
//}

#include <unistd.h>     // For write(), read(), STDIN_FILENO
#include <termios.h>    // For termios, tcgetattr, tcsetattr
#include <sys/select.h> // For select(), fd_set, timeval

// --- Terminal Control ---

// Store original terminal settings
struct termios original_termios;

/**
 * @brief Restores original terminal settings on exit.
 * This function is registered with atexit() to ensure
 * the terminal is not left in a broken state.
 */
void disable_raw_mode() {
    // Restore cursor visibility
    write(STDOUT_FILENO, "\033[?25h", 6);
    // Restore original terminal attributes
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

/**
 * @brief Enables "raw" (non-canonical, no-echo) mode for the terminal.
 * This allows us to read key presses one at a time without
 * waiting for the Enter key.
 */
void enable_raw_mode() {
    // Get current terminal attributes
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1) {
        perror("tcgetattr failed");
        exit(1);
    }
    // Register the cleanup function to run on exit
    atexit(disable_raw_mode);

    // Create a copy to modify
    struct termios raw = original_termios;
    
    // Disable canonical mode (line buffering) and echo
    raw.c_lflag &= ~(ICANON | ECHO);
    
    // Set the new attributes immediately
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr failed");
        exit(1);
    }
}

/**
 * @brief Checks if a key has been pressed. (The Linux `kbhit()`)
 * @return 1 if a key is waiting, 0 otherwise.
 */
int kbhit() {
    struct timeval tv = { 0L, 0L }; // No waiting
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); // Watch stdin
    // Check if stdin has data
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}

/**
 * @brief Reads a single key press. (The Linux `getch()`)
 * @return The character read.
 */
char getch() {
    char c;
    // Read 1 byte from stdin
    if (read(STDIN_FILENO, &c, 1) != 1) {
        // This should not happen in this blocking read
        return '\0';
    }
    return c;
}
