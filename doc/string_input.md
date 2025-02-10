# String Input

&nbsp;오랜만에 C++ 코딩할 때마다 까먹어서 이 참에 싹 정리. 모든 입력은 `std::string`을 기준으로 한다.


## std::cin::operator >>

&nbsp;제일 기본적인 입력 방식. 공백 문자(`' '`, `'\n'`, `'\t'`)를 만나면 입력이 저장되며, 해당 **공백 문자는 읽지 않고 버퍼에 남겨둔다**. 단, 연산이 연속해서 일어날 때는 읽는 시점에 **최초로 만나는 White Space들은 모두 무시된다**.

```C++
#include <iostream>
#include <string>

int main(void)
{
        std::string s;

        std::cin >> s;

        std::cout << "read: " << s << std::endl;

        return 0;
}
```


## std::getline

&nbsp;개행 문자(`'\n'`)가 나올 때까지 입력받는 방식. **문자열에 개행 문자를 포함하지는 않고, 읽은 개행 문자는 버퍼에서 삭제한다**. 읽는 시점에 최초로 만나는 문자와 관계없이 무조건 개행 문자까지만 읽기 때문에 `std::cin::operator >>`를 쓰고 **버퍼에 개행 문자가 남아있는 상태에서 사용하면 아무것도 읽지 못하고 빈 문자열을 읽는다**.

```C++
#include <iostream>
#include <string>

int main(void)
{
        std::string s;

        std::getline(std::cin, s);

        std::cout << "read: " << s << std::endl;

        return 0;
}
```


## std::cin::read

&nbsp;char 버퍼를 이용하여 원하는 최대 길이만큼 입력받는 방식. 버퍼에 입력받는 방식이기 때문에 공백 문자든 아니든 읽을 수 있으면 모두 정해진 최대 길이만큼 읽는다. 주의할 점은 C-Style 문자열을 사용하는 주제에 읽은 **문자열 끝에 자동으로 `'\0'`를 추가해주지 않아서 읽은 후 사용하려면 직접 끝 처리를 해주어야 한다**. 읽은 문자 개수는 `std::cin::gcount`를 이용하여 확인할 수 있다.

```C++
#include <iostream>

int main(void)
{
        char buf[128];

        std::cin.read(buf, 127);
        buf[std::cin.gcount()] = '\0';

        std::cout << buf << std::endl;

        return 0;
}
```


## std::cin::getline

&nbsp;C++의 표준 입력(키보드)인 `std::cin` 자체에 있는 `getline`으로, `std::getline`과는 다른 기능이다. `std::cin::read`처럼 C-Style 문자열을 사용하여 원하는 최대 길이만큼 입력받는다. 특징은 **개행 문자를 만나면 해당 위치에서 멈추고, 해당 개행 문자는 읽지 않고 입력 버퍼에서 버린다**. 문자 배열에서 읽은 위치 끝에 자동으로 `'\0'`를 포함시켜주기 때문에 끝 처리를 신경쓰지 않아도 된다.

```C++
#include <iostream>

int main(void)
{
        char buf[128];

        std::cin.getline(buf, 127);

        std::cout << buf << std::endl;

        return 0;
}
```


## std::istream_iterator<T>

&nbsp;**공백 문자를 기준으로 여러 문자를 입력받아야 할 때 사용하기 좋은 `<iterator>`의 반복자다**. 반복자를 참조할 때마다 공백 문자를 기준으로 한 단어를 읽는다. 특히 **`std::vector<T>` 등 컨테이너와 함께 편하게 사용할 수 있는데, 아래 코드와 같이 사용하면 종료 문자(`EOF`)를 만날 때까지 값을 읽고, 읽은 문자열을 공백 문자 기준으로 나누어 `std::vector<T>`를 생성한다**.

```C++
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main(void)
{
        std::vector<std::string> words(std::istream_iterator<std::string>(std::cin), {});

        for(auto& w: words)
                std::cout << w << '\n';

        std::cout << std::flush;

        return 0;
}
```

&nbsp;참고로 위 코드의 `{}`는 `std::istream_iterator<T>`의 디폴트 생성자로써 사용된 것으로, `std::vector<T>`를 반복자로 생성하려면 시작 위치 반복자와 끝 위치 반복자를 받아야 하기 때문에 사용된 것이다. 첫번째 인자를 통해 타입은 바로 유추할 수 있으므로, 끝 지점 반복자로 사용할 디폴트 `std::istream_iterator<T>` 객체를 간단하게 `{}`로 생성한다.


## std::stringstream

&nbsp;표준 입출력인 `std::cin`와 `std::cout` 대신 **`std::string`으로부터 입출력을 할 수 있는 방법**. `<sstream>`의 스트림이며 `std::cin` / `std::cout`처럼 사용할 수 있다. **입력과 출력을 모두 사용할 수 있는 강력한 도구다**.

```C++
#include <iostream>
#include <string>
#include <sstream>

int main(void)
{
        std::string org = "Hello, world!";
        std::stringstream ss(org);

        std::string word;

        while(ss >> word)
                std::cout << word << '\n';

        std::cout << std::flush;

        return 0;
}
```
