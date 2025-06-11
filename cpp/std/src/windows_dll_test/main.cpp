#include "my_lib.h"

#include <iostream>

/**
 * @brief 동적 라이브러리에 정의된 함수를 테스트하는 함수
 */
static void test(void);

/**
 * 빌드 시에는 clang++ main.cpp -o main.exe -L. -lmy_lib로 컴파일
 */
int main(void)
{
        test();

        return 0;
}

static void test(void) {
        std::cout << "Input two numbers(a b) >> " << std::flush;

        int a, b;
        std::cin >> a >> b;

        int res = add(a, b);
        std::cout << "Result: " << res << std::endl;
}
