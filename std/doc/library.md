# Library

&nbsp;라이브러리란 프로그램이 동작하기 위해 필요한 외부 목적 코드들이다. 일반적으로 C++ 프로젝트는 각각 외부 라이브러리(C++ 표준 라이브러리 포함)를 이용하여 필요한 작업들을 수행하는 함수들을 모아 프로젝트 내부 라이브러리를 제작하고, 최종 코드에서 해당 라이브러리들을 통합해 프로젝트를 완성한다. 결국 C++ 프로젝트를 진행하기 위해서는 라이브러리를 만들고 활용하는 방법을 익혀두는 것이 중요한데, 라이브러리는 어떻게 생성하고 사용하는가?


## Static Libraries

&nbsp;앞서 말했듯이 라이브러리는 그냥 필요한 기능들을 잔뜩 적어두고 미리 컴파일해둔 목적 코드 덩어리다. 그렇다면 그냥 목적 코드들을 하나로 뭉쳐 라이브러리로 만들고 최종 실행 파일 컴파일 시 포함하기만 되는 것 아닌가?


&nbsp;그것이 바로 정적 라이브러리(Static Libraries)다. **정적 라이브러리는 생성한 목적 코드들을 뭉쳐서 라이브러리로 만든 다음, 최종 실행 파일 자체에 해당 코드들을 포함**시킨다.


&nbsp;LLVM-MinGW 툴체인 Clang을 기준으로 정적 라이브러리를 생성하는 방법은 아래와 같다(GCC의 경우도 크게 다르지 않다). 우선 라이브러리로 뭉칠 코드들을 목적 파일(`.o`)로 컴파일한다. 컴파일러에서는 `-c` 옵션을 주면 실행 파일을 생성하지 않고 목적 파일로만 제작한다. 이후 LLVM 툴체인에 포함된 `ar`(Archiver)이라는 프로그램을 이용하여 정적 라이브러리를 생성한다.

```Shell
clang++ -c test.cpp -o test.o
ar rcs libtest.lib test.o
```

&nbsp;`ar`의 `rcs`에서, `r`은 기존의 라이브러리가 있으면 해당 라이브러리에 추가하거나 포함하여 새로 생성하라는 의미이고, `c`는 기존 라이브러리가 없는 경우 새로 생성, `s`는 인덱스를 추가하여 빠른 심볼 검색이 가능하도록 하는 옵션(크기가 약간 증가)이다. 라이브러리를 생성할 때는 앞에 `lib` 접두사를 붙이는 것이 관례다(라이브러리 이름을 지정하지 않는 경우, 자동으로 접두사 `lib`가 붙은 파일이 생성된다).


&nbsp;이후 라이브러리를 사용할 때 `main.cpp`에서는 내부 구현 파일 필요 없이 헤더 파일로 필요한 함수들을 사용하고(헤더 파일을 따로 정의하는 큰 이유), 컴파일 할 때 `-L`로 라이브러리를 검색할 위치를, `-l`로 실제 사용할 라이브러리 이름(접두사 `lib` 때고)을 넘겨주면 된다.

```Shell
clang++ main.cpp -o main -L. -ltest
```
<br>

&nbsp;대부분의 C++ 프로젝트에서 사용되는 **CMake를 사용하면 라이브러리를 생성하고 연결하기가 더 간편**한데, 자세한 내용은 [CMake 사용법](../../util/doc/cmake.md)에서 다루겠다.


&nbsp;CMake를 사용하는 것이 좋은 이유는 크로스 플랫폼 지원을 위해서다. 정적 라이브러리는 보통 Microsoft Windows 환경에서는 `.lib` 확장자를 가지고, POSIX 환경에서는 `.a`(Archive) 확장자를 가지는데, 두 플랫폼의 파일 포맷도 각각 COFF(Common Object File Format), ELF(Executable and Linkable Format)으로 다르고, 링크 시 네임 맹글링 방식도 달라 서로 호환이 안된다.


## Dynamic(Shared) Libraries

&nbsp;그냥 정적 라이브러리로 뭉쳐서 실행 파일에 포함하면 되었지, 왜 동적 라이브러리(Dynamic/Shared Libraries)가 필요한 것일까? 앞서 설명했듯이 정적 라이브러리는 실행 파일에 통째로 붙어서 컴파일된다. 이런 경우 해당 프로그램에 포함되는 파일이 많아지면 라이브러리의 크기도 커질테고, 이 라이브러리를 모두 포함하는 실행 파일의 크기 또한 어마어마해질 것이다. 그에 따라 컴파일 시간도 매우 오래 걸릴 것이다.


&nbsp;또 다른 문제는 특히 협업 과정에서 생길 수 있는데, 만약 특정 라이브러리에 수정 사항이 생겨 라이브러리를 다시 컴파일해야 한다면, 라이브러리 수정 -> 라이브러리 다시 컴파일 -> 실행 파일 다시 컴파일 등 그렇지 않아도 오래 걸리는 컴파일 과정을 처음부터 다시 수행해야 하는 문제가 생긴다.


&nbsp;다른 이유로는 동적 라이브러리가 공유 라이브러리라고도 불리는 이유인데, 만약 동일한 라이브러리를 사용하는 프로그램을 여러 개 실행시키는 경우 그렇지 않아도 한정적인 메모리에 동일한 코드를 여러 개 올려 사용해야 한다는 문제가 생긴다.


&nbsp;이러한 점을 해결할 방안으로 동적 라이브러리가 탄생했다. **동적 라이브러리는 라이브러리를 실행 파일에 포함시키지 않고 프로그램을 실행할 때, 동적으로 메모리에 로딩**하여 사용한다. 특히 **한 번 메모리에 올린 동적 라이브러리는 해당 라이브러리를 사용하는 다수의 프로세스들이 추가로 메모리에 올리지 않고 공유**한다.


&nbsp;컴퓨터 구조를 학습한 사람들은 '각 프로세스들은 다른 프로세스와 독립적이라 서로의 메모리 공간을 접근할 수 없지 않는가?'라고 생각할 수도 있는데, 각 프로세스가 보는 가상 메모리를 매핑하는 페이지 테이블에 물리 메모리 상 동적 라이브러리가 올라온 공간을 동일하게 매핑해주는 식으로 구현되어있다.


&nbsp;보통 POSIX 환경에서는 `.so`(Shared Object) 확장자를 사용하고, Microsoft Windows 환경에서는 우리가 많이 보았던 `.dll`(Dynamic Link Library) 파일을 사용한다. Microsoft Windows에서 게임과 같은 프로그램의 디렉토리에 `.dll` 파일이 잔뜩 들어있는 이유가 바로 이것이다.


&nbsp;LLVM-MinGW/Clang에서 동적 라이브러리는 아래와 같이 생성한다. 정적 라이브러리와 동일하게 우선 코드들을 목적 파일로 컴파일하고, 다음과 같이 동적 라이브러리를 생성한다.

```Shell
clang++ -c test.cpp -o test.o
clang++ -shared test.o -o test.dll -Wl,--out-implib,libtest.lib
```

&nbsp;위와 같은 괴상한 옵션이 붙는 이유를 이해하기 위해서는 POSIX 시스템과 Microsoft Windows 시스템의 차이를 조금 알아야 하는데, 간단히 말하자면 POSIX 시스템은 `.so` 파일을 통해 그냥 바로 동적 링크가 가능하지만, Microsoft Windows 시스템의 `.dll` 파일은 실제 실행할 때 필요한 파일이고, 실행 프로그램에 컴파일할 때는 임포트 라이브러리(참조 테이블 정보 포함)를 링크시켜주어야 하기 때문이다. Microsoft Windows 환경에서는 동적 라이브러리 생성 과정에 임포트 라이브러리(`.lib`, 확장자는 같지만 정적 라이브러리와는 다르다!)를 함께 생성해주어야 한다.


&nbsp;이후 동적 라이브러리를 실행 파일에 링크할 때는 정적 라이브러리를 링크하듯이 넘겨주면 된다. 이 경우에는 임포트 라이브러리가 링크되는 것이다.

```Shell
clang++ main.cpp -o main -L. -ltest
```
<br>

&nbsp;사실 Microsoft Windows 환경에 종속적인 코드이긴 하지만(`windows.h` 사용), `__declspec`을 활용하여 임포트 라이브러리 없이 동적 라이브러리를 링크할 수 있도록 하는 방법이 있다. 다만 이 방법으로 제작한 동적 라이브러리는 아래에서 설명할 명시적 링크 방법으로는 사용할 수 없다. 설명을 포함한 예제 코드는 [`../src/windows_dll_test`](../src/windows_dll_test)에서 확인할 수 있다.


&nbsp;동적 라이브러리 또한 CMake를 활용하면 간편하게 만들고 연결할 수 있다.


### Position Independent Code

&nbsp;운전을 배우기 위해서 엔진의 구조를 반드시 알 필요가 없듯이, 아래서 설명할 내용은 일반적인 프로그래밍 환경에서는 필요없는 내용이다. 하지만 좋은 레이서가 되려면 좋은 엔지니어가 되라는 말이 있듯이, 학문적 흥미로 탐구해보기 나쁘지 않은 내용이라 추가한다. 자세한 내용은 생략하고 간략하게만 설명한다.


&nbsp;GCC 툴체인에서는 동적 라이브러리를 정적 라이브러리와 동일한 ELF 포맷으로 생성되기 때문에 임포트 라이브러리가 필요 없이 바로 `.so` 파일을 링크해주면 된다. 대신 동적 라이브러리를 컴파일할 때 거의 필수로 `-fPIC`(Position Independent Code)를 옵션으로 주어야 하는데, 그 이유는 다음과 같다.


&nbsp;공유(동적) 라이브러리는 여러 프로세스에서 동시에 사용하지만 각 프로세스의 가상 메모리 공간이 다르기 때문에 라이브러리를 링크하기 위한 주소를 코드에 고정적으로 정해놓으면 안된다. 절대 주소를 사용하면 공유 라이브러리가 다른 위치에 로딩될 때 링크를 재배치해야하기 때문에 성능이 저하된다. `-fPIC` 옵션을 사용하면 공유 라이브러리를 절대 주소가 아니라 상대 주소로 간접 참조할 수 있는 코드가 생성된다.


![공유 라이브러리 함수가 처음 실행될 때](https://modoocode.com/img/cpp/20.3.4.png)
*출처: [모두의 코드](https://modoocode.com/321)*


&nbsp;이 때 PLT(Procedure Linkage Table)와 GOT(Global Offset Table)가 사용되는데, GOT는 각종 데이터 및 함수의 실제 메모리를 저장하는 테이블이고, PLT는 공유 라이브러리의 함수를 호출하기 위한 간접 참조 테이블이다. 각 프로세스는 GOT에 접근하여 공유 라이브러리를 이용하게 되는데, 간접 참조 방식인 만큼 최초에는 GOT에 해당 공유 라이브러리 정보가 없어 PLT를 또 참조하여 정보를 가져와야 하지만 그 다음부터는 GOT가 해당 주소 정보를 가지고 있으므로 더 빠르게 공유 라이브러리에 접근할 수 있다.


## Explicit Link

&nbsp;위에서 설명한 동적 라이브러리 링크 방식은 이론적으로는 암묵적 링크(Implicit Link)라 하며, 이는 프로그램 실행 시에 동적 라이브러리를 모두 로딩하는 방식이다. 이 방식이 실행 성능은 좋지만, 프로세스 런타임에 라이브러리에서 필요한 기능만 로드하여 사용하는 방법도 있다.


&nbsp;이러한 방식을 **명시적 링크(Explicit Link) 방식**이라고 한다. 사실 정확하게 설명하자면 명시적 링크보다는 명시적 로드라고 하는 것이 더 정확하다. 이 방식은 **실행 파일 컴파일 시 사용할 라이브러리 정보를 넘겨주는 것이 아니라 프로그래머가 코드 상에서 명시적으로 라이브러리를 로드하는 방식**이다.


&nbsp;파일을 메모리에 적재하는 기능은 운영 체제의 역할이기 때문에 동적 라이브러리의 명시적 링크도 운영 체제 별로 다르게 코드를 생성해야 하므로 코드가 복잡해지고, 런 타임에 라이브러리를 로딩하기 때문에 성능이 많이 떨어질 수 있다. 때문에 실제 상용 C++ 프로그램들은 **기본적으로 암묵적 링크 방식으로 동적 라이브러리를 사용하고, 이후에 추가로 프로그램에 붙일 플러그인이나 운영 체제 API 등을 사용할 때 명시적 링크 방식을 사용**한다.


&nbsp;정말 규모가 큰 상용 프로젝트가 아닌 이상 명시적 링크를 할 일은 많이 없을 것 같아 POSIX 환경과 Microsoft Windows 환경에서 명시적 링크하는 방법만 간단하게 소개한다.


&nbsp;두 플랫폼 모두에서 중요한 부분은, **명시적 링크를 위한 동적 라이브러리를 작성할 때는 명시적으로 링크할 함수들이 모두 [`extern "C"`](./fragments.md#extern-c)로 정의되어 있어야 한다는 것**이다. 명시적 링크 과정에서 함수 이름을 통해 라이브러리에서 함수를 링크하게 되는데 네임 맹글링이 적용되면 우리가 해당 함수의 이름을 정확히 알 수 없기 때문에 `extern "C"`를 붙이는 것이다.


### POSIX

&nbsp;POSIX는 UNIX 계열 운영 체제의 표준으로 Linux나 Apple Mac 등이 해당한다. 사실 POSIX에서는 명시적 로딩을 위한 기능을 정의하지 않는다고 표준 자체에 이미 정의해두었지만, POSIX를 따르는 운영 체제들은 사실상 표준(de facto standard)으로 `dlfcn.h`라는 API를 제공한다.


&nbsp;`dlfcn.h`는 라이브러리를 로드하여 핸들을 넘겨주는 `dlopen`, 핸들로부터 함수의 정보를 가져오는 `dlsym`, 라이브러리를 언로드하는 `dlclose` 등이 정의되어 있다. `dlfcn.h`를 사용하려면 실행 파일 컴파일 시 `-ldl` 옵션을 추가로 주어야 한다. 사용하는 예시는 아래와 같다.

```C++
#include <dlfcn.h>
#include <iostream>

int main() {
        // 동적 라이브러리 로드
        void* handle = dlopen("./test.so", RTLD_LAZY);
        if (!handle) {
                std::cerr << "Loading library failed: " << dlerror() << "\n";

                return 1;
        }

        // 함수 심볼 찾기
        typedef int (*add_func)(int, int);

        add_func add = (add_func) dlsym(handle, "test_add");
        if (!add) {
                std::cerr << "Finding functoin failed: " << dlerror() << "\n";

                dlclose(handle);

                return 1;
        }

        // 함수 호출
        std::cout << "Input two numbers(a b) >> " << std::flush;

        int a, b;
        std::cin >> a >> b;

        int res = add(a, b);
        std::cout << "Result: " << res << std::endl;

        // 동적 라이브러리 닫기
        dlclose(handle);

        return 0;
}
```

&nbsp;`RTLD_LAZY`는 동적 라이브러리를 로드하는 방법이다. `RTLD_LAZY`는 필요한 심볼이 실제 사용될 때, `RTLD_NOW`는 `dlopen` 호출 즉시 로드한다.


### Microsoft Windows

&nbsp;Microsoft Windows에서 운영 체제와 관련된 API는 `windows.h`로 이용할 수 있다. 명시적 링크와 관련된 함수로는 라이브러리를 로드하여 핸들(`HMODULE`)을 넘겨주는 `LoadLibrary`, 핸들로부터 함수의 정보를 가져오는 `GetProcAddress`, 라이브러리를 언로드하는 `FreeLibrary` 등이 있다.

```C++
#include <iostream>
#include <windows.h>

int main() {
        // 동적 라이브러리 로드
        HMODULE handle = LoadLibrary("./test_lib.dll");
        if (!handle) {
                std::cerr << "Loading library failed: " << GetLastError() << "\n";

                return 1;
        }

        // 함수 심볼 찾기
        typedef int (*add_func)(int, int);

        add_func add = (add_func) GetProcAddress(handle, "test_add");
        if (!add) {
                std::cerr << "Finding functoin failed: " << GetLastError() << "\n";

                FreeLibrary(handle);

                return 1;
        }

        // 함수 호출
        std::cout << "Input two numbers(a b) >> " << std::flush;

        int a, b;
        std::cin >> a >> b;

        int res = add(a, b);
        std::cout << "Result: " << res << std::endl;

        // 동적 라이브러리 닫기
        FreeLibrary(handle);

        return 0;
}
```

&nbsp;비교해보면 알겠지만 함수 이름만 다르지 사용법은 크게 다르지 않다!
