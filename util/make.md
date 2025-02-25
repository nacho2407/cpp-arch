# Make

&nbsp;코드 한 두개 빌드하는 것 쯤이야 터미널 명령어로도 쉽게 수행할 수 있지만, 실제 프로젝트에서 수십, 수백 개의 코드들을 컴파일하려고 하면 코드 작성하는 시간보다 터미널에 명령어 입력하는 시간이 더 오래 걸릴 것이다.


&nbsp;이러한 C/C++ 빌드를 도와줄 프로그램으로, UNIX 시절부터 사용된 유서깊은 Make라는 프로그램이 있다. `Makefile`이라는 입력 파일을 받아 해당 파일에 명시된 빌드 구조를 따라 자동으로 빌드를 수행해준다. 일반적으로 GCC를 주로 사용하는 Linux 환경에서 많이 사용되고, Microsoft Windows 환경에서는 MSVC가 자체적인 MSBuild 시스템으로 프로젝트를 관리하기 때문에 크게 필요가 없다(NMake 등 Make와 유사한 프로그램을 이용하기는 한다).


## Makefile

&nbsp;Makefile은 크게 3가지 요소라 나누어져 있다.

- Target: 생성하고자 하는 파일

- Recipes: 주어진 Target을 생성하기 위한 명령어 나열

- Prerequisites: 주어진 Target을 생성하기 위해 필요한 의존 파일의 나열

```Makefile
# Target: Prerequisites
#         recipes

all: main

main: main.o foo.o bar.o
        clang main.o foo.o bar.o -o main -g -std=c++20 -stdlib=libc++ -lc++abi -O2

main.o: main.cpp foo.h bar.h
        clang -c main.cpp -o main.o -std=c++20 -stdlib=libc++ -lc++abi -O2

foo.o: foo.cpp foo.h
        clang -c foo.cpp -o foo.o -std=c++20 -stdlib=libc++ -lc++abi -O2

bar.o: bar.cpp bar.h
        clang -c bar.cpp -o bar.o -std=c++20 -stdlib=libc++ -lc++abi -O2

clean:
        rm -f foo.o bar.o main.o
```
<br>

&nbsp;Make는 `Makefile`이 위치한 디렉토리에서 `make`로 실행하면 자동으로 `Makefile`을 찾아 읽는다. 실행 시 아무 인자를 주지 않으면 `Makefile`에서 `all`을 찾아 가리키는 Prerequisite을 생성하며, 위 파일의 경우에는 `make main`으로 실행하여 `main`을 생성하도록 할 수도 있다.


&nbsp;`Makefile`을 생성할 때 주의할 점은 `\t`와 `\s`를 정확하게 구분해야 한다는 것이다. `Makefile`은 따로 브라켓(`{}`) 없이 탭 문자를 이용하여 Recipes를 구분하기 때문에 공백 문자로 공간을 띄우는 것은 안되고 엄격하게 탭 문자 하나만을 통해서 Recipes를 구분해야 한다. 주석은 `#`로 처리한다.


## Variables

&nbsp;`Makefile`도 변수를 정의할 수 있다. Make에 변수가 왜 필요한가 싶을 수도 있지만, 수 많은 파일들을 함께 빌드해야 하는 프로젝트에서는 빌드에 필요한 파일을 하나하나 관리하는 것이 어려울 수 있기 때문에 변수를 통해 파일들을 정의하고 수정이 필요할 때 해당 변수만 수정할 수 있도록 하는 것이 필요하다. 프로그래밍적 관점에서 보면 변수보다는 상수라고 보는 것이 더 정확할 것이다.


&nbsp;예를 들어 자주 사용되는 변수 중에는 컴파일러의 종류를 명시하는 `CC` 변수가 있다(UNIX의 C 컴파일러 이름에서 유래). `CC = clang`과 같이 정의한 후 변수를 사용할 때는 `&(CC): -c foo.cpp`와 같이 `$()`로 감싸서 사용한다. 정의하지 않은 변수는 자동으로 빈 문자열로 치환된다.


&nbsp;컴파일러 종류를 명시하는 `CC` 외에 자주 사용되는 변수로는 해당 컴파일러에서 사용할 플래그(옵션)들을 정의한 `CXXFLAGS`(C의 경우에는 `CFLAGS`), 빌드에 사용될 오브젝트 파일(`*.o`, `*.obj`)들을 정의한 `OBJS` 등이 있다.


## PHONY

&nbsp;위의 `Makefile` 예시에서 작성한 `clean`처럼, 필요없는 파일들을 정리하는 등의 각종 명령어 또한 `Makefile`에 정의할 수 있다. 사용할 때는 `make clean`과 같이 명령하는데, 이런 경우 문제는 만약 디렉토리 내에 `clean`이라는 파일이 존재하면 Make는 이미 존재하는 파일이기 때문에 아무런 명령을 실행하지 않게 된다.


&nbsp;이런 경우를 방지하기 위해 `.PHONY`라는 Target을 따로 명시하여 파일 유무와 관계없이 반드시 명령을 실행하도록 할 수 있다. 실제 파일이 아니라(Phony) 명령으로 인식하라는 의미다.

```Makefile
.PHONY: clean
clean:
        rm -f $(OBJS) main
```


## Pattern

&nbsp;수십, 수백개의 코드를 동시에 빌드해야하는 상황에서는 변수만으로는 `Makefile`을 편리하게 작성하기 쉽지 않다. 다행히 Make는 패턴을 이용한 정의 또한 가능하다. 정규식에서 자주 사용되는 `*` 대신 Make에서는 와일드 카드로 `%`를 이용한다.

```Makefile
%.o: %.cpp %.h
        $(CC) $(CXXFLAGS) -c $<
```

&nbsp;만약 `%`에 `foo`라는 파일명이 들어가면 Prerequisite 또한 `foo.h foo.cpp`로 바뀌어 인식된다.


&nbsp;주의할 점은 Make에서 패턴은 Target과 Prerequisite에서만 사용 가능하다는 것이다. 실제 명령을 작성할 Recipes에는 대신 Make의 자동 변수를 활용해야 한다. 대표적인 자동 변수는 다음과 같다.

- `$@`: Target

- `$^`: Prerequisite(파일 중복 없음)

- `$<`: Prerequisite의 첫 번째 파일

- `$?`: 빌드 시점에 Target보다 최신 상태인 Prerequisite

- `$+`: 중복된 파일을 포함한 모든 Prerequisite


## -MMD

&nbsp;패턴을 활용할 수 있더라도, 모든 `%.o` 타겟이 `%.cpp %.h`를 의존하는 것은 아니기 때문에 예외 경우는 결국 따로따로 정의해주어야 한다. 결국 `Makefile`을 작성하는 것은 번거로운 일이 될 수 밖에 없는데, 이를 위해 대부분의 컴파일러에는 컴파일 대상 파일에 대한 의존 파일들을 `Makefile` 형식으로 생성해주는 옵션이 있다.


&nbsp;가장 기본적인 옵션은 `-MD`이지만, 대신 `-MMD`를 권장하는데, `-MD`의 경우 의존 파일에 표준 라이브러리들을 포함하여 표시하기 때문이다(표준 라이브러리의 경우 따로 의존 파일로 명시하지 않아도 된다). `-MMD`는 표준 라이브러리를 제외한 의존 파일 목록을 `Makefile`의 `Target: Prerequisites` 형식으로 반환하며, GCC와 Clang 모두에서 사용 가능한 옵션이다.


&nbsp;`-MMD` 옵션을 통해 생성한 `*.d` 파일을 사용하기 위해 `Makefile`에서는 `-include $(DEPS)`로 정의한다. `DEPS`는 `$(OBJS:.o=.d)`로 정의할 수 있다. `OBJS`의 파일들에서 `.o`를 `.d`로 바꾸라는 의미다. `-include` 대신 `include`로 작성하면 포함하려는 파일이 없는 경우 Make 실행이 중단된다. `*.d` 파일을 활용하여 `Makefile`을 작성하면 다음과 같다.

```Makefile
CC = clang++

CXXFLAGS = -std=c++20 -stdlib=libc++ -O2 -Wall -Wextra -fcolor-diagnostics -fdiagnostics-color=always -fansi-escape-codes

TARGET = main

SRCS = main.cpp foo.cpp bar.cpp

OBJS = $(SRCS:.cpp=.o)

DEPS = $(SRCS:.cpp=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
        $(CC) $(OBJS) -o $(TARGET) -g $(CXXFLAGS)

# 작성중
```


(작성중) - 멀티 쓰레딩(`-j<병렬 처리할 쓰레드 수>`), CMake의 필요성


```참고 자료
ifneq ($(MAKECMDGOALS), clean)   # "clean"을 실행하는 경우가 아니라면
    -include $(DEPS)             # .d 파일을 포함 (없으면 무시)
    ifeq ($(wildcard $(DEPS)),)  # .d 파일이 없으면
        $(MAKE) main             # make main을 다시 실행
    endif
endif
```
