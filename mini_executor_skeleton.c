#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s COMMAND [ARG ...]\n", argv[0]);
        return 2;
    }
    /* TODO: batasi argv[1] ke daftar command yang diizinkan. */
    pid_t child = fork();
    if (child < 0) { perror("fork"); return EXIT_FAILURE; }
    if (child == 0) {
        /* TODO: panggil execvp dan tangani error dengan perror + _exit. */
        _exit(127);
    }
    int status = 0;
    if (waitpid(child, &status, 0) < 0) {
        perror("waitpid");
        return EXIT_FAILURE;
    }
    /* TODO: tampilkan pesan yang lebih informatif untuk exit dan signal. */
    if (WIFEXITED(status)) return WEXITSTATUS(status);
    if (WIFSIGNALED(status)) return 128 + WTERMSIG(status);
    return EXIT_FAILURE;
}
