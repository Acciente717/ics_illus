#include <stdio.h>
#include <math.h>

int main()
{
    double _nan, _posinf, _neginf;
    _nan = -sqrt((double)-1);
    _posinf = (double)1 / 0;
    _neginf = (double)-1 / 0;

    printf("Add & Sub:\n");
    printf("nan + nan = %f\n", _nan + _nan);
    printf("nan + inf = %f\n", _nan + _posinf);
    printf("inf + inf = %f\n", _posinf + _posinf);
    printf("inf - inf = %f\n", _posinf - _posinf);
    printf("(-inf) + (-inf) = %f\n", _neginf + _neginf);
    printf("(-inf) - (-inf) = %f\n", _neginf - _neginf);
    printf("nan + 1 = %f\n", _nan + 1);
    printf("inf + 1 = %f\n", _posinf + 1);
    printf("inf - 1 = %f\n", _posinf - 1);

    printf("\nMul & Div:\n");
    printf("nan * 2 = %f\n", _nan * 1);
    printf("nan / 2 = %f\n", _nan / 1);
    printf("nan * 0 = %f\n", _nan * 0);
    printf("inf * 2 = %f\n", _posinf * 2);
    printf("(-inf) / 2 = %f\n", _neginf / 2);
    printf("inf * 0 = %f\n", _posinf * 0);

    printf("\nComparison:\n");
    printf("(nan != nan) = %d\n", _nan != _nan);
    printf("(nan == nan) = %d\n", _nan == -_nan);
    printf("(nan != 0) = %d\n", _nan != 0);
    printf("(nan > 0) = %d\n", _nan > 0);
    printf("(nan < 0) = %d\n", _nan < 0);
    printf("(inf > inf) = %d\n", _posinf > _posinf);
    printf("(inf > (-inf)) = %d\n", _posinf > _neginf);
    printf("(inf > 1) = %d\n", _posinf > 1);
    
    return 0;
}