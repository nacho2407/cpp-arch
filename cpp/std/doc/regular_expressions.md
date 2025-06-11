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

&nbsp;정확히 패턴이 일치하는 문자열을 찾는 `std::regex_match`와 달리, **어떤 패턴을 부분 문자열로 포함하는 문자열을 검색하고 싶다면 `std::regex_search`를 사용한다**. 사용법은 `std::regex_match`와 동일하다.


&nbsp;`std::regex_search`를 사용할 때 주의할 점은, 검색 작업을 반복적으로 수행할 때, 인자로 동일한 문자열을 넘겨준다면 항상 동일한(최초의) 결과만 반환한다는 것이다. 이를 위해 `std::match_results<I>::suffix`를 사용한다. 이는 지금까지 매치된 부분의 바로 뒷 부분부터 문자열 끝까지 반환한다. 사용법은 아래와 같다.

```C++
#include <iostream>
#include <regex>
#include <string>

int main(void)
{
        std::string s;
        std::cin >> s;

        std::regex r(R"([a]+bc)");
        std::smatch sm;

        while(std::regex_search(s, sm, r)) {
                std::cout << "Found string >> " << sm.str() << "\n";

                s = sm.suffix();
        }

        std::cout << std::flush;

        return 0;
}
```

&nbsp;참고로 `R"(...)"` 형태는 Raw String Literal을 의미하는 것으로, `\n` 같은 이스케이프 문자를 포함해 문자열 내 모든 문자를 있는 그대로 인식하는 포맷이다.


## std::sregex_iterator

&nbsp;`std::regex_search` 작업을 더 간단하게 수행할 수 있는 방법으로, `std::sregex_iterator`가 있다. `std::regex_iterator` 중 `std::string`을 사용하는 반복자로, 다음과 같이 사용한다.

```C++
#include <iostream>
#include <regex>
#include <string>

int main(void)
{
        std::string s;
        std::cin >> s;

        std::regex r(R"([a]+bc)");

        auto begin = std::sregex_iterator(s.begin(), s.end(), r);
        auto end = std::sregex_iterator();
        for(; begin != end; begin++)
                std::cout << "Found string >> " << begin->str() << "\n";

        std::cout << std::flush;

        return 0;
}
```


## std::regex_replace

&nbsp;`std::regex_replace`는 지정한 패턴을 입력받은 문자열로 모두 치환한 새 문자열을 반환한다. 다음과 같이 사용한다.

```C++
#include <iostream>
#include <regex>
#include <string>

int main(void)
{
        std::string input;
        std::cin >> input;

        std::regex r(R"(([a]+)(b)c)");

        input = std::regex_replace(input, r, "$2$1c");

        std::cout << "Replacing result >> " << input << std::endl;

        return 0;
}
```

&nbsp;치환할 문자열을 입력받을 때 사용한 `$1` 같은 형태는 캡쳐 그룹을 지정한 것으로, Back Reference라고 부른다. flex 등을 사용할 때도 자주 볼 수 있다. 캡쳐 그룹 안에 캡쳐 그룹을 중첩되게 사용하는 경우가 있는데, Back Reference를 이용하여 이를 지정하려면, 그 순서는 해당 캡쳐 그룹을 감싸는 여는 괄호의 등장 순서라고 생각하면 된다.
