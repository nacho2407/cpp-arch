#include "my_lib.h"

/**
 * 빌드 시에는 clang++ -shared my_lib.cpp -o my_lib.dll -D_MY_LIB_EXPORT로 컴파일
 */
int add(int a, int b)
{
        return a + b;
}
