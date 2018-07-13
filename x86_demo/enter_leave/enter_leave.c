#include <stdio.h>

int cal(int x, int y);

int main()
{
    int x, y;
    scanf("%d%d", &x, &y);
    printf("%d\n", cal(x, y));
    return 0;
}