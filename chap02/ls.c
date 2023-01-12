#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int main(void)
{
  DIR *dp;
  struct dirent *dir;

  if ((dp = opendir(".")) == NULL) {
    fprintf(stderr, "directory oepn error\n");
    exit(-1);
  }

  while ((dir = readdir(dp)) != NULL) {
    if (dir->d_ino == 0) continue;
    if (dir->d_type == DT_DIR) continue;

    printf("%s\n", dir->d_name);
  }

  closedir(dp);
}