#include  <errno.h>
#include  <stdio.h>
#include  <sys/stat.h>
#include  <sys/unistd.h>

#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart1;

extern int errno;

void stdio_setup(int no_init) {
    // Turn off buffers, so I/O occurs immediately
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

int _read(int file, char *data, int len) {
    if (file != STDIN_FILENO) {
        errno = EBADF;
        return -1;
    }

    if (HAL_UART_Receive(&huart1, (uint8_t *)data, len, 0) != HAL_OK) {
      return -1;
    }
    return len;
}

int _write(int file, char *data, int len) {
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }

    if (HAL_UART_Transmit(&huart1, (uint8_t *)data, len, 100) != HAL_OK) {
      return -1;
    }
    return len;
}

int _close(int file) {
    return -1;
}

int _lseek(int file, int ptr, int dir) {
    return 0;
}

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    if ((file == STDOUT_FILENO) || (file == STDIN_FILENO) || (file == STDERR_FILENO)) {
        return 1;
    }

    errno = EBADF;
    return 0;
}
