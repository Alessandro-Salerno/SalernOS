import os
import sys

# Global constant string
EXCLUDED_FILES = """
auxv.h
seek-whence.h
vm-flags.h
errno.h
fcntl.h
in.h
reboot.h
resource.h
stat.h
statx.h
signal.h
sigval.h
socket.h
sigevent.h
termios.h
time.h
blkcnt_t.h
blksize_t.h
dev_t.h
gid_t.h
ino_t.h
mode_t.h
nlink_t.h
pid_t.h
uid_t.h
access.h
wait.h
limits.h
utsname.h
ptrace.h
poll.h
epoll.h
packet.h
inotify.h
clockid_t.h
shm.h
mqueue.h
suseconds_t.h
fsfilcnt_t.h
fsblkcnt_t.h
socklen_t.h
statvfs.h
ioctls.h
xattr.h
msg.h
"""

def main(directory_path):
    # Convert the excluded files string into a set for faster lookup
    excluded_set = set(EXCLUDED_FILES.split('\n'))

    try:
        for filename in os.listdir(directory_path):
            filepath = os.path.join(directory_path, filename)

            # Check if it's a file and not in the excluded set
            if os.path.isfile(filepath) and filename not in excluded_set:
                print(filename)
    except FileNotFoundError:
        print(f"Directory not found: {directory_path}")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python script.py <directory_path>")
    else:
        main(sys.argv[1])
