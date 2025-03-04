# STL Patterns

STL에서 자주 사용되는 패턴 모음


## std::make_move_iterator

&nbsp;**모든 STL 컨테이너들은 다른 컨테이너의 시작과 끝 반복자를 통해 생성하는 복사 생성자를 가지고 있다**.

```C++
#include <iostream>
#include <set>
#include <vector>

int main(void)
{
        std::set<int> s = {1, 2, 3, 4, 5};

        std::vector<int> v(s.begin(), s.end());

        for(auto i: v)
                std::cout << i << " ";
        
        std::cout << std::flush;

        return 0;
}
```
<br>

&nbsp;하지만 복사 생성이 불가능한 자료형(`std::unique_ptr<T>` 등)의 경우, 이러한 패턴을 사용할 수 없는데, 대신 **해당 반복자를`<iterator>`의 `std::make_move_iterator`로 래핑 후 넘겨주면 이동 생성이 가능**하다.

```C++
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <vector>

int main(void)
{
        std::list<std::unique_ptr<int>> l;

        // std::unique_ptr<T>는 {}를 이용한 복사 생성 불가능
        l.push_back(std::make_unique<int>(1));
        l.push_back(std::make_unique<int>(2));
        l.push_back(std::make_unique<int>(3));

        std::vector<std::unique_ptr<int>> v(std::make_move_iterator(l.begin()), std::make_move_iterator(l.end()));

        for(auto& i: v)
                std::cout << *i << " ";
        
        std::cout << std::flush;

        return 0;
}
```

&nbsp;다만 **`std::make_move_iterator`를 이용한 이동 생성은 Associative Containers에는 사용이 안되고, `std::vector<T>`, `std::list<T>` 등 Sequential Containers에만 사용할 수 있다**.


&nbsp;이동 생성 외에도 삽입 연산에도 사용할 수 있는데, Sequential Container의 경우 (받을 컨테이너의 시작 위치, 보낼 컨테이너의 시작 위치, 보낼 컨테이너의 끝 위치)를 인자로 받는 `insert` 함수를 가지고 있어서 그렇다. 이 경우도 Associative Containers에는 사용할 수 없다.


&nbsp;컨테이너 원소가 이동되었다고 하여 **보낸 컨테이너의 원소가 사라지거나 완전히 이동되는 것은 아니다**. 실제로 위 코드의 `l.size()`는 여전히 `3`이다. 이동 연산은 복사 생성 과정에서 소유권을 완전히 이동한다는 보장 하에 swap 연산이 일어나지 않도록 하여 효율을 높힌 것 뿐이다(때문에 이미 소유권을 이전한 컨테이너의 원소를 참조하면 에러가 발생).


&nbsp;이동 반복자 패턴은 복사 생성이 불가능한 경우 뿐 아니라 컨테이너 내의 모든 원소를 소유권을 포함하여 한 번에 다른 컨테이너에 넘기고 싶을 때 더 효율적으로 작동하도록 사용할 수 있다.


## Removing Elements in Associative Containers

&nbsp;[`<algorithm>` 기능 소개](./algorithm.md#erasing)에서도 설명했듯이 원소의 순서를 마음대로 변경할 수 없는 Associative Containers의 경우에는 `<algorithm>`의 `std::remove`나 `std::remove_if`를 사용할 수 없다.


&nbsp;가장 보편적인 방법은 각 컨테이너의 포함된 `erase` 함수를 사용하는 것이다. **`erase`는 반복자를 받아 해당 위치의 원소를 삭제하고 그 다음 원소를 가리키는 반복자를 반환**한다.

```C++
#include <iostream>
#include <set>

int main(void)
{
        std::set<int> s = {1, 2, 3, 4, 5, 7, 8};

        for(auto i = s.begin(); i != s.end(); ) {
                // 짝수 원소 삭제
                if(*i % 2 == 0)
                        i = s.erase(i);
                else
                        i++;
        }

        for(auto i: s)
                std::cout << i << " ";

        std::cout << std::endl;

        return 0;
}
```
<br>

&nbsp;시간 복잡도는 그리 차이나지 않지만 이러한 코드를 **람다 식을 이용해 간단하게 표현할 수 있는 `std::erase_if` 함수가 C++ 20에서 추가**되었다.

```C++
#include <algorithm>
#include <iostream>
#include <set>

int main(void)
{
        std::set<int> s = {1, 2, 3, 4, 5, 7, 8};

        std::erase_if(s, [](int x) -> bool {
                return x % 2 == 0;
        });

        for(auto i: s)
                std::cout << i << " ";

        std::cout << std::endl;

        return 0;
}
```
<br>

## Extracting Keys or Values from Map

&nbsp;C++ 표준에는 아쉽게도 STL 맵 컨테이너(`std::map<T>`, `std::unordered_map<T>` 등)에서 키나 값을 추출하는 기능을 제공하지 않는다. 최선의 방법은 결국 for 문을 활용하여 하나하나 뽑아내는 수 밖에 없다.

```C++
#include <iostream>
#include <map>
#include <string>
#include <vector>

int main(void)
{
        std::map<int, std::string> m = {{1, "a"}, {2, "b"}, {3, "c"}};

        std::vector<int> keys;
        for(const auto& [k, v]: m)
                keys.push_back(k);

        for(auto k: keys)
                std::cout << k << " ";

        std::cout << std::endl;

        return 0;
}
```
<br>

&nbsp;키나 값을 따로 관리해야할 필요가 있을 때, 복사를 동적 공간을 활용하거나 복사를 통해 관리하지 말고 맵 내의 원소를 가리키는 컨테이너로 관리할 수 있을까?


&nbsp;`std::map<T>`나 `std::unordered_map<T>` 같은 경우엔 다른 원소의 위치가 변경되더라도 기존 원소의 레퍼런스는 유지되어야 한다고 C++ 표준에 명시되어있다. 덕분에 맵 컨테이너 내의 키나 값을 레퍼런스로 추출하여 관리할 수 있다.

```C++
#include <iostream>
#include <map>
#include <string>
#include <vector>

int main(void)
{
        std::map<int, std::string> m = {{1, "a"}, {2, "b"}, {3, "c"}};

        std::vector<const std::string*> values;
        for(const auto& [k, v]: m)
                values.push_back(&v);

        for(auto v: values)
                std::cout << *v << " ";

        std::cout << std::endl;

        return 0;
}
```


## std::back_inserter

&nbsp;한 컨테이너의 값을 다른 컨테이너로 옮기려고 할 때, for 문을 사용해서 작성하는 것보다 좋은 방식이 있다. `<iterator>`의 `std::back_inserter`는 이러한 경우에 편하게 사용할 수 있는 함수로, `std::back_insert_iterator<T>`를 반환하는 함수다. 이 반복자는 값을 넣을 때 내부적으로 `push_back` 함수를 사용하기 때문에 값을 받을 컨테이너의 크기를 신경쓰지 않고 복사하여 넘길 수 있다.


&nbsp;사용할 때는 `<algorithm>`의 `std::copy` 등의 함수를 활용하여 사용한다.

```C++
#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main(void)
{
        std::vector<std::string> v = {"a", "b", "c"};

        std::deque<std::string> d;
        std::copy(v.begin(), v.end(), std::back_inserter(d));

        for(const auto& s: d)
                std::cout << s << " ";

        std::cout << std::endl;

        return 0;
}
```
<br>

&nbsp;특히 `<algorithm>`의 `std::transform`과 함께 사용할 때 매우 편하게 사용할 수 있다.

```C++
#include <algorithm>
#include <cstddef>
#include <deque>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main(void)
{
        std::vector<std::string> v = {"apple", "banana", "cacao"};

        std::deque<std::size_t> d;
        std::transform(v.begin(), v.end(), std::back_inserter(d), [](const std::string& s) {
                return s.size();
        });

        for(auto t: d)
                std::cout << t << " ";

        std::cout << std::endl;

        return 0;
}
```
<br>

&nbsp;`std::back_inserter`는 `push_back` 함수가 포함되어 있지 않은 컨테이너에는 사용할 수 없다.
