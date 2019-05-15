#ifndef DAEMON_H
#define DAEMON_H

#include <string>

#include <signal.h>
#include <sys/file.h>
#include <sys/stat.h>

class Daemon {
public:

    static void checkInstance(const std::string &pidFilePath);

    static void daemonize();

};

void Daemon::checkInstance(const std::string &pidFilePath) {
    char pidbuf[16];
    int pid_file = open(pidFilePath.c_str(), O_CREAT | O_RDWR, 0666);
    int rc = flock(pid_file, LOCK_EX | LOCK_NB);


    if (rc) {
        if (EWOULDBLOCK == errno) {
            ELOG(ELogger::INFO_SYSTEM, ELogger::LEVEL_FATAL) << "Another instance is running!";
            exit(EXIT_FAILURE); // another instance is running
        }
    } else {
        // this is the first instance
        ftruncate(pid_file, 0);
        sprintf(pidbuf, "%ld", (long)getpid());
        write(pid_file, pidbuf, strlen(pidbuf));
        write(pid_file, "\n", 1);
    }
}

void Daemon::daemonize() {
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>0; x--) {
        close (x);
    }
}

#endif // DAEMON_H

