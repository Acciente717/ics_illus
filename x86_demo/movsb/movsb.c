#include <stdio.h>
#include <string.h>

void blkcpy(void *tgt, const void *src, size_t len);

char ibuff[1 << 10], obuff[1 << 10];

int main()
{
    fgets(ibuff, 1 << 10, stdin);
    blkcpy(obuff, ibuff, strlen(ibuff));
    printf("%s", obuff);
    return 0;
}