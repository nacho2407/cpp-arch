# CMake

&nbsp;간단한 프로젝트는 Make로 빌드할 수 있다 해도, 조금 규모가 있는 프로젝트에서는 [Make](./make.md)만으로 빌드를 처리하기에 버거울 수 있다. CMake가 등장한 이후로는 대부분의 C/C++ 프로젝트에서 [CMake](https://cmake.org/)를 사용하고 있다.


&nbsp;CMake는 Make와 같은 빌드 프로그램이 아니라 Make, Ninja와 같은 프로그램들로 빌드할 수 있는 환경을 만들어주는 메타 빌드 프로그램이다. 쉽게 말해 Make처럼 빌드를 수행하는 것이 아니라 `Makefile`을 만들어주는 프로그램이라고 생각하면 된다.


## CMakeLists.txt

&nbsp;Make의 `Makefile`처럼 CMake는 `CMakeLists.txt`라는 파일을 입력으로 사용한다. `CMakeLists.txt`에 기본적으로 포함되어야 하는 내용은 아래와 같다.

```Text
cmake_minimum_required(VERSION 3.14)

project(
        Test
        VERSION 1.0
        DESCRIPTION "테스트 프로그램"
        LANGUAGES CXX
)
```

&nbsp;CMake도 어느정도 역사가 긴 프로그램이기 때문에(2025.02. 기준 4.0 Pre-release) 문법이나 주로 사용하는 기능들이 달라져 왔는데, 현재 자주 사용되는 문법들은 대부분 메이저 버전 3부터 사용되던 기능들이므로 `cmake_minimum_required`에 해당 문법이 사용 가능한 최소 버전을 입력한다. 일반적으로 `FetchContent` 모듈이 안정화된 `VERSION 3.14`를 사용한다.


&nbsp;`project`에는 본 프로젝트의 정보를 입력한다. 필수로 프로젝트 이름만 작성하면 되고, `VERSION`같은 내용들은 필수가 아니다.


## Main Program

&nbsp;이제 우리가 빌드할 목표 프로그램을 명시한다. `add_excutable`에 우리가 빌드할 프로그램 이름(Target)과 해당 프로그램을 빌드하기 위해 필요한 파일들(Prerequisites)을 정의한다.

```Text
add_excutable(test test.cpp)
```

&nbsp;원래는 Prerequisites를 작성하기 전에 해당 Target을 참조하는 방법을 접근 제한자로 명시해야 하지만, 실행 파일의 경우 다른 Target이 참조할 일이 없기 때문에 작성하지 않아도 괜찮다.


&nbsp;Target 별로 컴파일 할 때 옵션을 다르게 지정할 수 있다. `target_compile_options`는 특정 Target의 컴파일 옵션을 지정하는 역할을 한다.

```Text
target_compile_options(test -g -std=c++20 -stdlib=libc++ -lc++abi -O2)
```

&nbsp;`add_excutable`처럼, 원래는 옵션을 작성하기 전 접근 제한자를 작성한다. 프로젝트 내의 모든 Target에 대한 컴파일 옵션을 지정하고 싶으면 `add_compile_options`를 사용하면 된다.


## Internal Libraries

![C++_프로젝트의_일반적인_구성](https://modoocode.com/img/cpp/19.2.2.png)
*출처: [모두의 코드](https://modoocode.com/332)*


&nbsp;C++ 프로젝트는 일반적으로 그림과 같이 기능들을 내부 라이브러리로 나누어서 실행 파일에 사용하고, 내부 라이브러리를 작성할 때는 이것저것 외부 라이브러리를 이용하는 형태를 띄게 된다.


&nbsp;사용할 기능들을 라이브러리로 쪼개지 않고 하나의 거대한 실행 파일로 사용하게 되면 기능에 수정 사항이 생길 경우 프로젝트 전체를 다시 빌드해야하기 때문에 불편하고, 기능별로 라이브러리를 쪼개어두면 관리 / 테스팅에 유용하기 때문에 보통 라이브러리를 쪼개어 프로젝트를 개발하게 된다.


&nbsp;CMake를 사용하면 이러한 라이브러리들을 따로 빌드할 필요 없이 `lib`와 같은 별개의 디렉토리에 라이브러리 빌드에 필요한 파일들을 넣어두고 해당 내용을 프로젝트 단위에서 한 번에 CMake로 관리할 수 있다.


&nbsp;라이브러리 디렉토리에 빌드를 위한 `CMakeLists.txt` 파일을 하나 더 생성하고, 프로젝트 단위의 `CMakeLists.txt`에서는 `add_subdirectory`로 라이브러리 빌드를 위한 `CMakeLists.txt`가 포함된 디렉토리를 지정한다.

```Text
add_subdirectory(lib)
```
<br>

&nbsp;라이브러리의 헤더 파일이 `include` 등 다른 디렉토리에 존재한다면 프로젝트 단위 `CMakeLists.txt`에서 해당 위치를 `target_include_directories`로 지정한다.

```Text
target_include_directories(test ${CMAKE_SOURCE_DIR}/include)
```

&nbsp;`CMAKE_SOURCE_DIR`은 최상위(프로젝트) `CMakeLists.txt`의 위치를 뜻하는 CMake 상수다. 현재 사용 중인 `CMakeLists.txt`의 위치는 `CMAKE_CURRENT_SOURCE_DIR`로 사용한다. CMake에서는 변수 / 상수를 사용할 때 `${}`로 감싸서 사용한다.


&nbsp;이후 실제 사용할 라이브러리는 `target_link_libraries`로 지정한다.

```Text
target_link_libraries(test my_lib)
```
<br>

&nbsp;최종적으로 프로젝트 단위의 `CMakeLists.txt`는 아래와 같은 형태다. 주석은 Make와 동일하게 `#`로 처리한다.

```Text
# /CMakeLists.txt
cmake_minimum_required(VERSION 3.11)

project(
        Test
        VERSION 1.0
        DESCRIPTION "테스트 프로그램"
        LANGUAGES CXX
)

add_subdirectory(lib)

add_excutable(test test.cpp)
target_compile_options(test -g -std=c++20 -stdlib=libc++ -lc++abi -O2)
target_link_libraries(test my_lib)
target_include_directories(test ${CMAKE_SOURCE_DIR}/include)
```
<br>

&nbsp;이제 라이브러리 빌드를 위한 `CMakeLists.txt`를 작성해보자. 실행 파일을 `add_excutable`로 추가했던 것처럼 라이브러리와 해당 라이브러리를 빌드하기 위한 파일들을 `add_library`에 추가한다.

```Text
add_library(my_lib STATIC my_lib.cpp)
```

&nbsp;빌드할 라이브러리의 링크 방법(정적 / 동적)에 따라 `STATIC` / `SHARED` 또는 `MODULE`(런타임에 불러올 수 있는 동적 라이브러리)를 사용한다. 크기가 크지 않은 라이브러리의 경우 `STATIC`을 사용하면 된다.


&nbsp;이후 라이브러리의 `CMakeLists.txt`에도 헤더 파일 위치와 컴파일 옵션등을 명시한다. 최종적인 라이브러리 `CMakeLists.txt`는 아래와 같은 형태다.

```Text
# /lib/CMakeLists.txt
add_library(my_lib STATIC my_lib.cpp)
target_include_directories(my_lib PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(my_lib PRIVATE -std=c++20 -stdlib=libc++ -lc++abi -O2)
```


## Build

&nbsp;이제 프로젝트 단위(최상위) `CMakeLists.txt` 파일의 위치에서 `cmake`를 실행한다. 기본적으로 소스 코드와 동일한 위치에 빌드되는데, 프로젝트 디렉토리가 더러워지기 때문에 일반적으로 빌드 파일을 따로 만들어 해당 위치로 이동 후 `cmake ..`으로 빌드한다. `-B <빌드 디렉토리 이름>` 옵션으로 빌드 디렉토리를 생성함과 동시에 빌드할 수도 있다.


&nbsp;CMake가 성공적으로 작동하면 빌드 디렉토리에 `Makefile`이 생기는데, 해당 디렉토리에서 `make`(mingw-w64 환경의 경우 `mingw32-make`)를 실행한다. 아니면 최상위 디렉토리에서 `cmake --build <빌드 디렉토리 이름>`으로 빌드할 수 있다.


&nbsp;빌드 디렉토리에 우리 타겟이 생성되는데, 이를 우리가 원하는 위치에 설치할 수도 있다. `CMakeLists.txt`에 아래와 같이 정의한다.

```Text
install(
        TARGETS main lib
        DESTINATION ${CMAKE_SOURCE_DIR}/bin
)
```

&nbsp;위와 같이 정의해두면 `make install` 수행 시 지정한 `DESTINATION`로 우리의 타겟들이 복사(설치)된다. `RUNTIME`, `LIBRARY`(동적 라이브러리), `ARCHIVE`(정적 라이브러리) 등으로 종류에 따라 설치 위치를 달리할 수도 있다.


&nbsp;참고로 CMake는 기본적으로 `make clean`을 정의해주기 때문에 중간 빌드 파일들을 쉽게 정리할 수 있다.


## Access Modifier

&nbsp;프로젝트 단위의 `CMakeLists.txt`와 달리 라이브러리 단위의 `CMakeLists.txt`에는 Target에 접근 제한자(`PRIVATE`)를 모두 명시하였다. 이는 해당 라이브러리 Target을 프로젝트의 메인 프로그램이 참조하기 때문이다.


&nbsp;CMake에서 접근 제한자는 해당 Target을 참조하는 다른 Target에도 해당 내용들을 함께 넘겨줄 것인가를 결정한다. `PUBLIC`, `PRIVATE`, `INTERFACE`가 있으며, 특히 의존성(`target_link_libraries`)에는 어떤 접근 제한자를 선택해야 할 지는 아래 가이드를 따르는 것이 좋다.

- `PUBLIC`: 참조할 라이브러리가 자신의 헤더 파일과 구현 파일에 모두 사용될 때

- `PRIVATE`: 참조할 라이브러리가 자신의 내부 구현에만 사용되고 헤더 파일에는 추가할 필요가 없을 때

- `INTERFACE`: 참조할 라이브러리가 헤더 파일에만 명시되고 내부 구현에서는 사용하지 않을 때


## External Libraries

&nbsp;비교적 최근에 등장한 언어들은 외부 라이브러리를 쉽게 불러오고 관리하는 프로그램(Python의 `pip`, Node.js의 `npm` 등)이 기본으로 제공되지만, C++은 언어 상으로 제공되는 기능이 없기 때문에 외부 라이브러리를 불러오거나 설치하는 것이 상당히 불편하다.


&nbsp;C/C++ 프로젝트에 대부분 사용되는 CMake는 이러한 외부 라이브러리를 쉽게 설치하고 불러올 수 있도록 `FetchContent`라는 CMake 내부 모듈을 제공한다.

```Text
include(FetchContent)
FetchContent_Declare(
        fmt
        GIT_REPOSITORY "https://github.com/fmtlib/fmt"
        GIT_TAG "11.1.3"
)
FetchContent_MakeAvailable(fmt)

target_link_libraries(my_lib PRIVATE fmt::fmt)
```

&nbsp;참고로 외부 라이브러리를 링킹할 때 `target_link_libraries` 안에서 그냥 `fmt`가 아니라 `fmt::fmt`와 같이 정확한 타겟 이름을 넘겨주는 것이 최신 CMake 표준 방식이라고 한다. 이 내용은 라이브러리마다 다르기 때문에 설명 문서 등을 확인하여 작성해야한다.


&nbsp;`FetchContent` 모듈을 사용하기 위해서는 `include(FetchContent)`를 통해 들여온 다음 `FetchContent_Declare`를 통해 외부 라이브러리를 다운로드하고, `FetchContent_MakeAvailable`을 통해 외부 라이브러리를 프로젝트에 추가한다.


&nbsp;위의 경우는 Git Repository를 통해 라이브러리를 받아오는 방법이다. `GIT_REPOSITORY`를 통해 받아올 라이브러리의 주소를 정의하고 `GIT_TAG`는 필수는 아니지만 원하는 릴리즈가 있다면 해당 릴리즈의 태그를 정의한다.


&nbsp;`FetchContent`를 통해서 외부 라이브러리를 받아오려면 해당 라이브러리가 CMake 프로젝트로 구성되어 있어야 한다(대부분의 오픈 소스 C++ 프로젝트는 CMake 프로젝트로 구성되어 있다). 또한 `FetchContent_MakeAvailable`로 사용할 수 있도록 `*.cmake.in`이라는 파일이 추가되어 있어야 한다.


&nbsp;`FetchContent_MakeAvailable`을 사용하면 자동으로 해당 라이브러리를 빌드하지만, 어떤 경우에는 `FetchContent_Populate`를 통해 수동으로 디렉토리를 설정하고 다운로드해야 할 수도 있다.

```Text
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.1
)
FetchContent_Populate(spdlog)
add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})

target_link_libraries(my_lib PRIVATE spdlog)
```

&nbsp;물론 위의 경우에도 해당 라이브러리가 CMake 프로젝트로 구성되어 있어야 하고, 그 CMake 프로젝트에 `spdlog_SOURCE_DIR` 등이 정의되어 있어야 한다.


&nbsp;만약 사용할 라이브러리가 이미 자신의 시스템 안에 설치되어 있다면(Boost C++ Libraries, 패키지 매니저 등), `find_package`를 통해 외부 라이브러리를 받아올 수도 있다.

```Text
# 자동으로 Boost 경로 검색
find_package(Boost 1.87.0 REQUIRED COMPONENTS system)

# Boost.Asio는 링킹이 필요한 라이브러리 Boost.System에 의존적
target_link_libraries(my_lib PRIVATE Boost::system)

# Boost.Asio는 헤더 전용 라이브러리
target_include_directories(my_lib PRIVATE ${Boost_INCLUDE_DIRS})
```

&nbsp;`find_package`는 `<패키지 이름> [버전] [REQUIRED | QUIET] [COMPONENTS <패키지 내 사용할 라이브러리 이름>]`를 입력으로 받는다. `[REQUIRED | QUIET]`는 해당 패키지가 존재하지 않을 때 경고를 출력할 지, 하지 않을 지를 결정한다.


&nbsp;패키지 매니저 vcpkg를 CMake와 사용하는 방법은 [vcpkg 사용법](./vcpkg.md)에서 소개하겠다.


## Build System

&nbsp;CMake에서 사용할 빌드 시스템을 따로 설정하지 않는다면 디폴트로 UNIX Make(Linux)가 선택되지만 Ninja, MinGW Make(Microsoft Windows)와 같은 다른 빌드 시스템을 사용하는 것도 가능하다. 특히 Make의 경우 사용하는 시스템에 따라 어떤 `Makefile`을 생성할지가 달라지기 때문에 필요한 경우 반드시 설정해주어야 한다.


&nbsp;CMake를 실행할 때 인자로 `-DCMAKE_GENERATOR="MinGW Makefiles"`와 같이 넘겨주거나, 특정 시스템에서 자주 사용한다면 `CMakeLists.txt` 안에서 `set(CMAKE_GENERATOR "MinGW Makefiles")`로 정의해두어도 된다(권장하지는 않는다). 고전적인 방법으로 `-G "MinGW Makefiles"`와 같이 사용해도 된다.
