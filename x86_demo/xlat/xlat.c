#include <stdio.h>

char table[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

char lut(int idx);

int main()
{
    while (1)
    {
        int idx;
        scanf("%d", &idx);
        printf("%d\n", (int)lut(idx));
    }
    return 0;
}