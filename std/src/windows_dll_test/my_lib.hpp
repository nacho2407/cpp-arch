#ifndef MY_LIB_H
// 헤더 가드
#define MY_LIB_H

// Microsoft Windows에 종속적인 코드
#include <windows.h>

// 동적 라이브러리를 컴파일할 때는 -D_MY_LIB_EXPORT 옵션으로 매크로를 추가하여 외부에서 사용할 동적 라이브러리 함수임을 명시
#ifdef _MY_LIB_EXPORT
        #define MY_LIB_DECLSPEC __declspec(dllexport)
#else
        #define MY_LIB_DECLSPEC __declspec(dllimport)
#endif

/**
 * @brief 동적 라이브러리로 사용할 함수
 */
MY_LIB_DECLSPEC int add(int a, int b);
#endif
