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

(작성중) - options & regex engine, std::regex::optimize, capture group & std::smatch, std::regex_search & std::regex::suffix, std::sregex_iterator, std::regex_replace & back reference, nested capture group
