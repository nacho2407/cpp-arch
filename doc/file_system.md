# File System

&nbsp;C++에는 기존에 `<fstream>`을 통하여 파일을 조작하는 기능을 제공하였으나, `<fstream>`은 파일 하나에 대한 읽기/쓰기 기능만 지원하고 파일 시스템 전체에 대한 정보(파일 이름, 위치 등)를 조작하는 기능은 없었다. **C++ 17 표준에 포함된 `<filesystem>`은 이러한 파일 메타 데이터를 조작하도록 도와주는 기능들이 포함**되어 있다.


## std::filesystem::path, Finding

&nbsp;가장 기본적인 기능으로, 특정 위치에 해당하는 파일이 존재하는지 확인해보자. **`std::filesystem::exists` 함수는 `std::filesystem::path`로 명시된 위치를 포함한 파일을 받아 해당 파일이 존재하는지 결과를 반환**한다. 해당 파일이 존재하는 파일일 때, `std::filesystem::is_regular_file`, `std::filesystem::is_directory` 함수를 통해 해당 파일이 일반 파일(Linux의 경우 device나 socket 등을 제외한 일반 파일)인지, 디렉토리인지 확인할 수 있다.


&nbsp;작성하면서도 느꼈지만, 왠만한 관련 기능이 모두 `std::filesystem`에 포함된지라 너무 길어져서 보통은 `namespace fs = std::filesystem`으로 간략하게 사용한다고 한다.

```C++
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(void)
{
        fs::path p("./test.cpp");

        if(fs::exists(p)) {
                std::cout << "File " << p << " exists." << std::endl;

                if(fs::is_regular_file(p))
                        std::cout << "It's a regular file!" << std::endl;
                else if(fs::is_directory(p))
                        std::cout << "It's a directory file!" << std::endl;
        }
        else
                std::cout << "File " << p << " doesn't exists." << std::endl;

        return 0;
}
```


## Path Related Functions

&nbsp;`<filesystem>`은 다음과 같은 경로 관련 함수를 제공한다.

- `std::filesystem::current_path`: 현재 실행 중인 프로그램의 위치

- `std::filesystem::relative_path`: 해당 `std::filesystem::path`의 상대 경로

- `std::filesystem::absolute`: 인자로 전달된 `std::filesystem::path`의 절대 경로, `../`와 같은 정보가 포함될 수 있음

- `std::filesystem::canonical`: 인자로 전달된 `std::filesystem::path`의 공식적인 절대 경로


&nbsp;경로 관련 함수들은 해당 `std::filesystem::path`가 실제로 존재하지 않으면 모두 예외를 발생시키기 때문에 **호출 전 반드시 `std::filesystem::exists`로 확인**해주어야 한다.


&nbsp;`std::filesystem::path`는 `operator /`가 정의되어 있어서 `std::filesystem::current_path() / "test"`와 같이 편하게 경로를 지정할 수 있다.


## Directory Traversal

&nbsp;`<filesystem>`은 **디렉토리 순회를 쉽게 할 수 있는 `std::filesystem::directory_iterator`라는 반복자를 제공**한다. `std::filesystem::directory_iterator`의 참조 결과는 `std::filesystem::directory_entry`로, 파일의 이름, 크기, 경로 등 여러 정보를 포함하고 있다.


&nbsp;`std::filesystem::directory_iterator`는 범위 기반 for문에 사용할 수 있다.

```C++
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(void)
{
        auto cur_p = fs::current_path();
        fs::directory_iterator di(cur_p);

        for(auto& entry: di)
                std::cout << entry.path() << "\n";

        std::cout << std::flush;

        return 0;
}
```
<br>

&nbsp;`std::filesystem::directory_iterator`는 디렉토리 내의 다른 디렉토리도 그냥 파일로써 취급하지만, 순회 시 자식 디렉토리를 포함하여 모든 파일을 순회하고 싶다면 대신 `std::filesystem::recursive_directory_iterator`를 사용하면 된다.


## Create Directory

&nbsp;`std::ofstream`은 생성자로 받은 경로의 파일이 존재하지 않으면 자동으로 생성하기 때문에 파일을 생성하는 것은 어렵지 않다. 하지만 `std::ofstream`을 통해 디렉토리를 생성할 수는 없는데, 이는 `std::filesystem::create_directory`를 통해 수행할 수 있다.


&nbsp;`std::filesystem::create_directory`에 인자로 `"./a/b"`와 같이 경로를 넘겨주는 경우, `./a` 디렉토리가 반드시 존재해야 한다(`std::ofstream` 또한 부모 디렉토리가 존재하지 않으면 파일을 생성할 수 없다). 경로에 포함된 존재하지 않는 모든 디렉토리를 생성하는 법이 있는데, 간단하게 `std::filesystem::create_directories` 함수를 사용하면 된다.

```C++
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(void)
{
        fs::path p("./a/b");

        fs::create_directories(p / "c");

        if(fs::exists(p / "c"))
                std::cout << "Path " << (fs::canonical(p / "c")) << " exists." << std::endl;
        else
                std::cout << "Path " << (fs::canonical(p / "c")) << " doesn't exists." << std::endl;

        return 0;
}
```


## Copy & Delete File

&nbsp;`<filesystem>`은 파일 복사 방법으로 `std::filesystem::copy`를 지원하며, 사용법은 Linux의 `cp`와 유사하다. 복사할 디렉토리의 `std::filesystem::path`만 넘겨주면 해당 위치의 파일들만 복사가 되고 디렉토리는 복사가 되지 않는데, `std::filesystem::copy`의 마지막 인자로 `std::filesystem::copy_options`의 옵션들을 전달할 수 있다. 디렉토리를 포함하여 복사하려면 `std::filesystem::copy_options::recursive`를 넘겨주면 된다.

```C++
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(void)
{
        fs::path from("../src");
        fs::path to("../test");

        fs::copy(from, to, fs::copy_options::recursive);

        if(fs::exists(to))
                std::cout << "File copying success..." << std::endl;
        else
                std::cout << "File copying failed..." << std:: endl;

        return 0;
}
```
<br>

&nbsp;`std::filesystem::copy_options`는 이미 존재하는 파일에 대한 옵션도 있다.

- `std::filesystem::copy_options::skip_existing`: 이미 존재하는 파일은 무시, 예외 발생하지 않음

- `std::filesystem::copy_options::overwrite_existing`: 이미 존재하는 파일은 덮어씀

- `std::filesystem::copy_options::update_existing`: 이미 존재하는 파일이 더 오래되었을 경우에만 덮어씀.

&nbsp;여러 옵션을 사용할 때는 다른 매개 변수 옵션처럼 파이프 기호(`|`)를 통해 연결한다.


&nbsp;파일 삭제 또한 `std::filesystem::remove`를 통해 간단하게 수행할 수 있다. `std::filesystem::remove`는 디렉토리를 삭제하는 경우 해당 디렉토리가 반드시 비어있어야 하는데, 만약 비어있지 않은 디렉토리를 강제로 삭제하려면 `std::filesystem::remove_all`을 사용해야 한다.


&nbsp;파일 삭제와 함께 `std::filesystem::directory_iterator`를 사용할 때 주의할 점이 있는데, `std::vector<T>`의 원소 삭제 후 기존 반복자(`std::random_access_iterator`)가 무효화되는 것처럼, `std::filesystem::directory_iterator` 또한 파일 삭제 등 디렉토리의 구조가 바뀔 때마다 무효화되기 때문에 디렉토리 구조가 바뀐다면 다시 초기화해주어야 한다.


## Exception Handling

&nbsp;파일 처리 중 발생하는 예외를 코드에서 처리하고 싶지 않다면, 파일 관련 함수들에게 마지막 인자로 `std::error_code`를 전달하면 된다. 이 경우 예외가 발생할 상황이 생기면 예외를 전파하는 대신 `std::error_code` 객체에 발생한 오류를 설정한다. `<filesystem>`의 예외 발생 가능 함수들은 모두 `std::error_code`를 통한 오버로드를 지원한다.
