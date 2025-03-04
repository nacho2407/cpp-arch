# String

&nbsp;`std::string` 관련 기능들을 정리해보자.


## Input

&nbsp;제일 기본적인 기능들이지만 자주 까먹어서 이 참에 싹 정리한다. 모든 입력은 `std::string`을 기준으로 한다.


### std::cin::operator >>

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


### std::getline

&nbsp;개행 문자(`'\n'`)가 나올 때까지 입력받는 방식. **문자열에 개행 문자를 포함하지는 않고, 읽은 개행 문자는 버퍼에서 삭제한다**. 읽는 시점에 최초로 만나는 문자와 관계없이 무조건 개행 문자까지만 읽기 때문에 `std::cin::operator >>`를 쓰고 **버퍼에 개행 문자가 남아있는 상태에서 사용하면 아무 것도 읽지 못하고 빈 문자열을 읽는다**.

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


### std::cin::read

&nbsp;char 버퍼를 이용하여 원하는 최대 길이만큼 입력받는 방식. 버퍼에 입력받는 방식이기 때문에 공백 문자든 아니든 읽을 수 있으면 모두 정해진 최대 길이만큼 읽는다. 주의할 점은 C-Style 문자열을 사용하는 주제에 **읽은 문자열 끝에 자동으로 `'\0'`를 추가해주지 않아서 읽은 후 사용하려면 직접 끝 처리를 해주어야 한다**. 읽은 문자 개수는 `std::cin::gcount`를 이용하여 확인할 수 있다.

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


### std::cin::getline

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


### std::istream_iterator<T>

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


## Parsing

### std::stringstream

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


### Substring

&nbsp;**`std::string::find`를 통해 문자열 내에서 특정 문자열과 일치하는 부분을 찾고, `std::string::substr`을 통해 해당 부분을 잘라낼 수 있다**. 첫 번째 인자 위치부터 두 번째 인자 크기만큼 문자열을 자른다.


&nbsp;`std::string::find`는 해당 문자열을 찾지 못하면 `std::string::npos`를 반환하는데, 이는 `std::size_t`의 최댓값으로 절대 인덱스로 사용될 수 없음을 보장한다. 아래와 같이 검사하고 사용할 수 있다.

```C++
#include <iostream>
#include <string>

int main(void)
{
        std::string s = "Hello, world!";

        std::string t;
        std::getline(std::cin, t);

        auto n = s.find(t);
        if(n == std::string::npos)
                std::cout << "Not found..." << std::endl;
        else
                std::cout << "Found @ " << n << " >> " << s.substr(n, t.size()) << std::endl;

        return 0;
}
```


### String Conversion

&nbsp;C에서 제공되는 `atoi`처럼 C++의 `<string>`에도 문자열 변환 함수가 있는데, **`std::stoi`, `std::stoll`, `std::stof`, `std::stod` 등이 제공된다**. 이름에서 변환되는 형식은 유추할 수 있을 것이다.


&nbsp;`std::stoi` 등의 함수가 유용한 점은, **반드시 문자열 전체가 해당 함수의 입력 형태가 아니라도 변환이 가능**하다는 것이다. 예를 들어 `std::stoi` 함수는 문자열의 첫 글자가 숫자라면 숫자로 읽을 수 있는 부분까지 모두 읽고 정수로 변환 불가능한 문자를 만나면 해당 위치에서 멈추고 그 앞부분 까지만 변환을 수행한다(단, 첫 글자부터 변환 불가능한 문자이면 예외를 출력한다). 두 번째 인자로 **`std::size_t` 타입의 변수를 레퍼런스로 전달하면 변환을 중단한 위치를 넘겨준다**.


```C++
#include <cstddef>
#include <iostream>
#include <string>

int main(void)
{
        std::string s = "123asdf";

        std::size_t sz;
        int i = std::stoi(s, &sz);

        std::cout << "Extracted integer >> " << i << "\nRemain string >> " << s.substr(sz) << std::endl;

        return 0;
}
```


## std::string_view

&nbsp;사실 실제 프로그래밍을 하다보면 복사할 일이 제일 많은 데이터는 문자열 데이터일 것이다. 때문에 이동 연산과 참조를 할 일이 매우 많은데, 문자열을 가리키는 포인터처럼 사용할 수 있는 `std::string_view`가 C++ 17부터 사용 가능하다.


&nbsp;그냥 `std::string`의 레퍼런스를 사용하면 되지, 왜 `std::string_view`가 필요한가? `std::string_view`는 `const char*`, `std::string` 등 다양한 문자열 타입을 받을 수 있다. `const std::string&` 타입을 매개 변수로 받는 함수가 있을 때, 이 함수의 입력으로 `"Hello, world!"` 등을 사용하게 되면 `const char*`가 `std::string`으로 변환되는 과정에서 임시 객체가 생성된다. 하지만 매개 변수를 `std::string_view` 타입으로 받으면 임시 객체 생성 없이 빠르고 효율적으로 문자열을 사용할 수 있다.


&nbsp;또한 레퍼런스 형으로 값을 받는다고 하더라도 입력이 임시 객체라면 결국 복사가 발생하게 된다. 하지만 `std::string_view`를 사용하면 임시 객체라 하더라도 복사 없이 임시 객체 자체를 참조하는 객체를 사용하므로, 복사 없이 사용할 수 있다.

```C++
#include <iostream>
#include <string>
#include <string_view>

std::string my_str(void)
{
        return "Hello, world!";
}

void say_hi_s(const std::string& s)
{
        std::cout << s << std::endl;
}

void say_hi_s_v(std::string_view s)
{
        std::cout << s << std::endl;
}

int main(void)
{
        say_hi_s(my_str()); // 임시 객체 -> 복사 발생
        say_hi_s_v(my_str()); // 복사 없음!

        return 0;
}
```
<br>

&nbsp;참고로 `std::string_view`는 원본 `std::string`을 참조하는 객체이기 때문에 원본 `std::string`이 변경되면 참조하는 `std::string_view`도 변경된다.


## Replacing & Searching

&nbsp;`std::string`에는 탐색을 지원하는 `find`와 문자열 교체를 지원하는 `replace` 함수를 지원한다.

```C++
#include <iostream>
#include <string>

int main(void)
{
        std::string s = "I love cats!";

        s.replace(s.find("cats"), 4, "nacho");

        std::cout << s << std::endl;

        return 0;
}
```

&nbsp;`find`는 찾은 문자열의 첫 위치를 반환하고(두 번째 인자로 탐색 시작 위치 지정), `replace`는 첫 번째 인자 위치부터 두 번째 인자만큼을 세 번째 인자로 변경하는 함수다. `std::string`은 모든 문자열을 변경하는 `replace_all` 같은 함수를 지원하지 않기 때문에 그런 기능이 필요하다면 직접 구현해야 한다.

```C++
void replace_all(std::string& src, std::string_view from, std::string_view to)
{
        for(std::size_t cur = src.find(from); cur != std::string::npos; cur = src.find(from, cur))
                src.replace(cur, from.size(), to);
}
```
<br>

&nbsp;선형 탐색을 통해 단일 값을 찾아내는 `std::find`(`std::string` 및 컨테이너에 포함된 `find`와는 다름) 외에도 컨테이너에서 특정 패턴을 검사하는 `std::search`라는 함수도 있는데, 특히 문자열에서 많이 사용된다. `<algorithm>`에 포함된 이 함수들은 인덱스가 아닌 첫 위치를 가리키는 반복자를 반환한다.


&nbsp;`std::search`는 단순 선형 검색을 통해 값을 탐색하는 `find`와 달리 좀 더 진보된 알고리즘을 사용할 수 있는데, 문자열을 빠르게 탐색하는 대표적인 알고리즘인 보이어-무어 알고리즘을 사용할 수 있다.

```C++
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>

int main(void)
{
        std::string s = "Hello, world!";

        std::string n = "world";

        auto it = std::search(s.begin(), s.end(), std::boyer_moore_searcher(n.begin(), n.end()));

        if(it != s.end())
                std::cout << "Found @ " << std::distance(s.begin(), it) << std::endl;
        else
                std::cout << "Not found" << std::endl;

        return 0;
}
```

&nbsp;이 알고리즘은 `<functional>`의 `std::boyer_moore_searcher`라는 알고리즘 객체를 통해 사용할 수 있다. `std::distance`는 두 반복자 간 거리를 계산하는 `<iterator>`의 함수다.


## Case Conversion

&nbsp;알파벳의 대소문자 변경은 C에서 넘어온 `<cctype>`의 `std::toupper`, `std::tolower`를 이용한다.


## Substring

&nbsp;`std::string`은 `substr` 수행 시 잘라낸 새로운 문자열을 생성하게 되는데, `std::string_view`의 `substr`은 참조하고 있는 범위만 조절하여 문자열 생성 / 복사 없이 부분 문자열 처리를 할 수 있다는 장점이 있다. 이 경우 시간 복잡도가 O(1)이다.


&nbsp;또한 `std::string_view`의 `remove_prefix`, `remove_suffix` 함수를 통해 문자열의 연속된 접두사, 접미사를 빠르게 처리할 수 있다.

```C++
#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>

int main(void)
{
        std::string s = "   Hello, world!   ";

        std::string_view sv(s);

        sv.remove_prefix(std::min(s.find_first_not_of(" "), s.size()));

        sv.remove_suffix(std::min((s.size() - 1) - s.find_last_not_of(" "), s.size()));

        std::cout << sv << std::endl;

        return 0;
}
```

&nbsp;위 코드는 `std::string`의 `find_first_not_of`, `find_last_not_of` 함수를 이용하여 문자열 앞 뒤의 연속된 문자(`" "`)를 삭제한다. `std::string`의 `substr`을 이용했다면 시간 / 공간 복잡도가 더 높았겠지만 `std::string_view`를 이용하여 매우 효율적으로 작업을 처리했다.


&nbsp;참고로 `std::min`, `std::max` 함수는 C의 `min`, `max`와 달리 `<algorithm>`에 포함되어 있다.
