#ifndef _PTY_H
#define _PTY_H

#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

int openpty(int            *amaster,
            int            *aslave,
            char           *name,
            struct termios *termp,
            struct winsize *winp);

int forkpty(int            *amaster,
            char           *name,
            struct termios *termp,
            struct winsize *winp);
#ifdef __cplusplus
}
#endif

#endif /* _PTY_H */
