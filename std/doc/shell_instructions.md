# Shell Instructions

&nbsp;쉘 명령어를 실행하는 기능은 C 시절부터 존재하던 기능이고, C++에서도 C에서 넘어온 라이브러리를 이용하여 쉘 명령어를 실행한다.


## std::system

&nbsp;`std::system`은 `<cstdlib>`에 포함된 기능으로, 인자로 받은 명령어 줄을 그대로 시스템 쉘에 실행한다. 그대로 실행시켜 버린다는 점 때문에 보안적으로는 굉장한 허점이 될 수 있는데, 마치 AI 챗봇에게 채팅 인젝션 공격을 하듯이 명령어 인젝션 공격을 받을 가능성이 있어 주의해서 사용해야 한다.


&nbsp;`std::system`은 해당 프로그램의 반환 값(성공이면 `0`, 실패하면 그 외의 값)을 반환하기 때문에 해당 프로그램이 출력하는 값을 캡쳐할 수는 없다. 프로그램의 출력값이 필요하다면 리다이렉션(`>`)을 통해 새 파일에 결과를 저장하고 그 값을 읽어와야 한다.

```C++
#include <cstdlib>
#include <fstream>
#include <iostream>

int main(void)
{
        int ret = 0;

        #ifdef _WIN32
                ret = std::system("dir > out");
        #else
                ret = std::system("ls > out");
        #endif

        if(!ret)
                std::cout << std::ifstream("out").rdbuf() << std::flush;

        return 0;
}
```


## popen / pclose

&nbsp;C/C++ 표준에 정의된 함수는 아니지만, POSIX(Portable Operating System Interface - UNIX) 환경 C의 표준 함수로 `popen`과 `pclose`가 있다. POSIX를 준수하는 Linux에서 사용 가능하며, Microsoft Windows에서도 `_popen`, `_pclose`의 형태로 사용은 가능하다. `<cstdio>`에 정의되어 있다.


&nbsp;`std::system`과 달리 프로그램의 출력을 캡쳐할 수 있으며, C-Style `FILE*`을 반환하는데, 원칙적으로 `FILE*`은 `std::ifstream`로 변환할 수 없어서(온갖 편법을 활용하면 가능은 하다고 한다) 출력 또한 C-Style로 해야 한다.

```C++
#include <cstdio>
#include <iostream>

int main(void)
{
        FILE* ret;

        #ifdef _WIN32
                ret = _popen("dir", "r");
        #else
                ret = popen("ls", "r");
        #endif

        if(!ret) {
                std::cerr << "Excution failed.\n";

                return 1;
        }

        char buf[256];
        while(fgets(buf, sizeof(buf), ret))
                std::cout << buf;

        std::cout << std::flush;

        #ifdef _WIN32
                _pclose(ret);
        #else
                pclose(ret);
        #endif

        return 0;
}
```
