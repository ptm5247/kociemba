#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>

#include "tables.h"

static int open_file(const char *dir, const char *name, char *path_buf) {
  int   result, fd;

  result = snprintf(path_buf, MAX_PATH_LENGTH, "%s/%s", dir, name);
  if (result >= MAX_PATH_LENGTH) {
    fprintf(stderr, "table path (%s/%s) is too long\n", dir, name);
    exit(EXIT_FAILURE);
  }

  result = access(path_buf, F_OK | R_OK);
  if (result == -1) {
    fprintf(stderr, "failed to access %s: %s\n", path_buf, strerror(errno));
    exit(EXIT_FAILURE);
  }

  fd = open(path_buf, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "failed to open %s: %s\n", path_buf, strerror(errno));
    exit(EXIT_FAILURE);
  }

  return fd;
}

void read_table(const char *dir, const char *name, void *ptr, size_t len) {
  char    path[MAX_PATH_LENGTH] = { '\0' };
  int     result, fd;
  ssize_t count;

  fd = open_file(dir, name, path);

  do {
    count = read(fd, ptr, len);
    if (count == -1) {
      fprintf(stderr, "failed to read table %s: %s", path, strerror(errno));
      exit(EXIT_FAILURE);
    }
    len -= count;
    ptr += count;
  } while (len);

  result = close(fd);
  if (result) {
    fprintf(stderr, "failed to close %s: %s\n", path, strerror(errno));
    exit(EXIT_FAILURE);
  }
}

void map_table(const char *dir, const char *name, void **ptr, size_t len) {
  char      path[MAX_PATH_LENGTH] = { '\0' };
  int       result, fd;
  ssize_t   count;

  fd = open_file(dir, name, path);

  *ptr = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
  if (*ptr == MAP_FAILED) {
    fprintf(stderr, "failed to mmap %s: %s\n", path, strerror(errno));
    exit(EXIT_FAILURE);
  }

  result = close(fd);
  if (result) {
    fprintf(stderr, "failed to close %s: %s\n", path, strerror(errno));
    exit(EXIT_FAILURE);
  }
}
