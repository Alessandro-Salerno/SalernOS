#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

volatile sig_atomic_t count = 0;

void sigalrm_handler(int signum) {
    count++;
    printf("SIGALRM %d\n", count);

    if (count >= 5) {
        // Stop the timer and exit
        struct itimerval stop_timer = {0};
        setitimer(ITIMER_REAL, &stop_timer, NULL);
        exit(0);
    }
}

int main() {
    struct itimerval timer;

    // Set up SIGALRM handler
    signal(SIGALRM, sigalrm_handler);

    // Timer: first after 1 sec, then every 1 sec
    timer.it_value.tv_sec     = 1;
    timer.it_value.tv_usec    = 0;
    timer.it_interval.tv_sec  = 1;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer");
        return 1;
    }

    // Infinite loop waiting for signals
    while (1) {
        poll(NULL, 0, 1000);
    }

    return 0;
}
