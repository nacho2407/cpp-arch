# String Input

&nbsp;오랜만에 C++ 코딩할 때마다 까먹어서 이 참에 싹 정리. 모든 입력은 `std::string`을 기준으로 한다.


## std::cin::operator >>

&nbsp;제일 기본적인 입력 방식. White Space(`' '`, `'\n'`, `'\t'`)를 만나면 입력이 저장되며, 해당 White Space는 읽지 않고 버퍼에 남겨둔다. 단, 연산이 연속해서 일어날 때는 읽는 시점에 최초로 만나는 White Space들은 모두 무시된다.

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

&nbsp;개행 문자(`'\n'`)가 나올 때까지 입력받는 방식. 문자열에 개행 문자를 포함하지는 않고, 읽은 개행 문자는 버퍼에서 삭제한다. 읽는 시점에 최초로 만나는 문자와 관계없이 무조건 개행 문자까지만 읽기 때문에 `std::cin::operator >>` 쓰고 버퍼에 개행 문자가 남아있는 상태에서 사용하면 아무것도 읽지 못하고 빈 문자열을 읽음.

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

(작성중)
