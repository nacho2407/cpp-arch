# Regular Expressions

## &lt;regex>

&nbsp;C++ 11부터는 표준에 추가된 `<regex>`를 통해 정규 표현식을 쉽게 사용할 수 있다. `std::regex`와 `std::regex_match`를 이용한 기본적인 사용 방법은 다음과 같다.

```C++
#include <iostream>
#include <regex>
#include <string>

int main(void)
{
        std::regex rx("Hello, [a-z]+!");

        std::string s;

        std::getline(std::cin, s);

        if(std::regex_match(s, rx))
                std::cout << "matched!" << std::endl;

        return 0;
}
```
<br>

&nbsp;정규 표현식은 사실 정확히 어떤 문법을 가진다고 정해진 것은 아니고 형식 언어의 한 종류(제3유형/정규 문법)라서 많이 사용되는 정규식 문법도 하나가 아니다. C++의 `<regex>`에서 사용하는 문법은 JavaScript를 위해 제작되어 가장 많이 사용되는 ECMAScript를 디폴트로 사용한다. 그 외에 다른 정규식 엔진(basic POSIX, grep 등)을 사용하려면 마지막 인자로 `std::regex::grep` 등의 옵션을 주면 된다. 이 외에도 추가적인 기능을 지정할 수 있는데, 예를 들어 대소문자를 구별하지 않으려면 `std::regex::icase`를 사용한다. 여러 개의 옵션을 사용하기 위해서는 옵션들을 비트 와이즈 OR 연산자(`|`)를 이용하여 연결한다.


&nbsp;`std::regex::optimize`라는 옵션도 있는데, 이는 `std::regex` 객체 생성은 조금 더 오래 걸리지만, 반복적인 객체 사용은 더 빠르게 수행할 수 있는 옵션이다.


## Capture Group & std::smatch

&nbsp;정규식을 사용할 때 찾아낸 문자열 전체를 사용하기보다는 찾은 문자열에서 필요한 일부분만 사용하는 경우가 많다. 정규식에서는 `()`로 감싸는 캡쳐 그룹을 사용하여 부분 패턴을 추출한다. 패턴에서 캡쳐된 부분 문자열은 `std::smatch`라는 컨테이너에 저장되고, 원소의 `str` 함수를 통해 문자열로 받을 수 있다. 참고로 C-Style의 문자 배열을 사용하려면 `std::cmatch`를 사용한다.

```C++
#include <iostream>
#include <regex>
#include <string>

int main(void)
{
        std::regex rx("010-([0-9]{3,4})-[0-9]{4}");

        std::string s;
        std::cin >> s;

        std::smatch sm;
        if(std::regex_match(s, sm, rx))
                std::cout << "matched: " << sm[1].str() << std::endl;

        return 0;
}
```

&nbsp;참고로 `std::smatch`에 저장되는 값 중 **첫번째 원소(`[0]`)는 항상 매치된 문자열 그 자체다**.


## std::regex_search

&nbsp;정확히 패턴이 일치하는 문자열을 찾는 `std::regex_match`와 달리, **어떤 패턴을 부분 문자열로 포함하는 문자열을 검색하고 싶다면 `std::regex_search`를 사용한다**. (작성중)


(작성중) - std::regex_search & std::regex::suffix, std::sregex_iterator, std::regex_replace & back reference, nested capture group
