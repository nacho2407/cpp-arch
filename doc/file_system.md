# File System

&nbsp;C++에는 기존에 `<fstream>`을 통하여 파일을 조작하는 기능을 제공하였으나, `<fstream>`은 파일 하나에 대한 읽기/쓰기 기능만 지원하고 파일 시스템 전체에 대한 정보(파일 이름, 위치 등)를 조작하는 기능은 없었다. C++ 17 표준에 포함된 `<filesystem>`은 이러한 파일 메타 데이터를 조작하도록 도와주는 기능들이 포함되어 있다.


## std::filesystem::path, Finding

&nbsp;가장 기본적인 기능으로, 특정 위치에 해당하는 파일이 존재하는지 확인해보자. `std::filesystem::exists` 함수는 `std::filesystem::path`로 명시된 위치를 포함한 파일을 받아 해당 파일이 존재하는지 결과를 반환한다. 해당 파일이 존재하는 파일일 때, `std::filesystem::is_regular_file`, `std::filesystem::is_directory` 함수를 통해 해당 파일이 일반 파일(리눅스의 경우 device나 socket 등을 제외한 일반 파일)인지, 디렉토리인지 확인할 수 있다.


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


(작성중) - 경로 관련 함수, 파일 관련 에러 처리법, std::filesystem::directory_iterator / std::filesystem::directory_entry, 디렉토리 생성, 파일 복사 / std::filesystem::copy_options, std::filesystem::remove / std::filesystem::remove_all, std::filesystem::directory_iterator 사용 시 주의점
