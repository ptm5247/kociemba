#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tables.h"

void read_table(const char *dir, const char *name, void *ptr, size_t len) {
  char    path[MAX_PATH_LENGTH] = { '\0' };
  int     result;
  ssize_t count;
  int     fd;

  result = snprintf(path, sizeof(path), "%s/%s", dir, name);
  if (result >= sizeof(path)) {
    fprintf(stderr, "table path (%s/%s) is too long\n", dir, name);
    exit(EXIT_FAILURE);
  }

  result = access(path, F_OK | R_OK);
  if (result == -1) {
    fprintf(stderr, "failed to access %s: %s\n", path, strerror(errno));
    exit(EXIT_FAILURE);
  }

  fd = open(path, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "failed to open %s: %s\n", path, strerror(errno));
    exit(EXIT_FAILURE);
  }

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
