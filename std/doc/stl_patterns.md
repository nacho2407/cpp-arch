# STL Patterns

STL에서 자주 사용되는 패턴 모음


## std::make_move_iterator

&nbsp;모든 STL 컨테이너들은 다른 컨테이너의 시작과 끝 반복자를 통해 생성하는 복사 생성자를 가지고 있다.

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

&nbsp;하지만 복사 생성이 불가능한 자료형의 경우, 이러한 패턴을 사용할 수 없는데, 대신 해당 반복자를 `std::make_move_iterator`로 래핑 후 넘겨주면 이동 생성이 가능하다.

```C++
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <vector>

// 디버깅 필요
int main(void)
{
        std::set<std::unique_ptr<int>> s = {std::make_unique<int>(1), std::make_unique<int>(2), std::make_unique<int>(3)};

        std::vector<std::unique_ptr<int>> v(std::make_move_iterator(s.begin()), std::make_move_iterator(s.end()));

        for(auto i: v)
                std::cout << i << " ";
        
        std::cout << std::flush;

        return 0;
}
```


*작성중*
