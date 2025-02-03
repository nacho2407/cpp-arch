# Standard Libraries - &lt;algorithm>

## Sorting

### std::sort

&nbsp;기본 정렬 알고리즘. 퀵 정렬, 힙 정렬, 삽입 정렬을 복합적으로 사용하는 인트로 정렬을 이용하여 구현되었다. **`RandomAccessIterator` 타입을 만족하는 반복자를 반환하는 자료구조에 사용**할 수 있다. **기본적으로 오름차순 정렬**을 수행한다.

```C++
std::vector<int> vec;

vec.push_back(5);
vec.push_back(1);
vec.push_back(3);
vec.push_back(4);
vec.push_back(2);

std::sort(vec.begin(), vec.end());
```
<br>

&nbsp;정렬 순서를 변경하고 싶다면 **세 번째 인자**로 Functor를 넘겨주거나 아래와 같이 **람다 함수를 넘겨주면 된다**.

```C++
std::vector<int> vec;

vec.push_back(5);
vec.push_back(1);
vec.push_back(3);
vec.push_back(4);
vec.push_back(2);

std::sort(vec.begin(), vec.end(), [](int first, int second) {
        return first > second; // int 내림차순 정렬
});
```

&nbsp;`<functional>`의 `std::greater<int>` 등을 활용하여 더 간단하게 구현할 수도 있다. `std::greater<T>`, `std::less<T>` 등은 `T`에 `operator <`가 정의되어 있어야 한다.


### std::partial_sort

&nbsp;정렬하고자 하는 컨테이너의 일부분만 정렬하는 알고리즘. `std::sort`와 달리 **start, middle, end 세 개의 인자가 필요**하다. start부터 middle 전 까지의 원소들이 정렬되며, middle부터 end 전까지의 원소들도 의도치 않게 순서가 뒤바뀔 수 있기 때문에 **정말 [start, middle) 사이의 원소만 정렬하고 나머지의 순서는 중요하지 않을 때만 사용하는 것이 좋다**. 이런 경우에는 `std::sort`를 사용하는 것보다 시간 효율이 좋다.

```C++
std::vector<int> vec;

vec.push_back(5);
vec.push_back(1);
vec.push_back(3);
vec.push_back(4);
vec.push_back(2);

std::partial_sort(vec.begin(), vec.begin() + 3, vec.end());
```
<br>

&nbsp;정렬 순서를 바꾸고 싶을 때는 `std::sort`처럼 마지막 인자로 비교 함수 객체(람다 함수)를 넘겨준다.


### std::stable_sort

&nbsp;정렬 알고리즘에서 동일한 수준의 값들은 정렬 후에도 기존의 순서를 유지하는 것을 안정성(Stability)라 하는데, `std::sort`는 안정성을 만족하지 않는 퀵 정렬과 힙 정렬 등으로 구성되어 있다. 만약 안정성이 반드시 필요한 경우(pair 정렬 등)에는 `std::stable_sort`를 이용하여 정렬할 수 있다. `std::stable_sort`는 안정성을 만족하는 합병 정렬 및 삽입 정렬로 구현되었기 때문에 시간적 효율은 `std::sort`보다 떨어진다. 사용법은 `std::sort`와 동일하다.

```C++
std::vector<std::pair<int, char>> vec;

vec.push_back(std::make_pair(5, 'a'));
vec.push_back(std::make_pair(1, 'b'));
vec.push_back(std::make_pair(3, 'c'));
vec.push_back(std::make_pair(5, 'd'));
vec.push_back(std::make_pair(2, 'e'));

std::stable_sort(vec.begin(), vec.end());
```


## Container Manipulation

### Erasing

&nbsp;정수 벡터 내에서 3이라는 숫자를 모두 제거하고 싶을 때 반복자와 `erase` 함수를 이용하여 구현할 수도 있지만, `<vector>`와 같이 `RandomAccessIterator`를 이용하는 컨테이너들은 원소가 삭제되면 현재 반복자를 계속 사용할 수 없기 때문에 계속 반복자를 다시 생성해야하는 비효율적인 면이 있다. **`std::remove`는 이런 경우에 조건에 따라 효율적으로 컨테이너 내의 원소를 삭제하는 알고리즘을 제공**한다. **Sequential뿐 아니라 Associative 컨테이너에서도 사용**할 수 있다.

&nbsp;구현된 방법은 간단하다. 마치 삽입 정렬을 수행하는 것처럼 현재 확인 중인 값을 지금까지 찾은 (목적) 원소 개수만큼 앞에 떨어진 원소에 덮어쓰고, 찾고자하는 원소를 찾으면 그냥 계산할 떨어진 위치만 하나 늘려준다. 때문에 실제로 값을 컨테이너에서 삭제하는 것이 아니라 찾은 개수만큼 컨테이너의 끝에 확인되지 않은 원소들이 남게되고, 그 부분이 시작되는 반복자를 반환하기에 실제 삭제를 위해서는 그 부분을 직접 삭제해야한다. 작성을 간단하게 해주긴 하지만 시간 복잡도가 향상되는 것은 아니다(O(n)). 아래와 같은 형태로 사용한다.

```C++
std::vector<int> vec;

vec.push_back(5);
vec.push_back(1);
vec.push_back(3);
vec.push_back(4);
vec.push_back(3);

vec.erase(std::remove(vec.begin(), vec.end(), 3)/*3의 개수만큼 vec.end()에서 떨어진 위치의 반복자 반환*/, vec.end());
```
<br>

&nbsp;`std::remove`는 찾으려는 원소의 동일성만 확인하는데, 동일성 뿐 아니라 우리가 해당 조건을 직접 지정할 수도 있다. **`std::remove_if`는 세 번째 인자로 원소 하나를 인자로 받고, bool(또는 int)을 반환하는 조건 함수를 지정할 수 있다**. Functor를 사용할 수도 있으나, 람다 함수의 사용을 권장한다. 아래와 같이 사용한다.

```C++
std::vector<int> vec;

vec.push_back(5);
vec.push_back(1);
vec.push_back(3);
vec.push_back(4);
vec.push_back(3);

vec.erase(std::remove_if(vec.begin(), vec.end(), [](int e) -> int {
        return e % 2 == 1;
})); // 홀수 원소 삭제
```


### Finding

&nbsp;`std::set<T>`과 같은 Associative Container들은 기본적으로 `find` 함수를 제공하지만, `std::vector<T>`같은 **Sequential Container들은 `find` 함수를 기본으로 제공하지 않는다**. 대신 `<algorithm>`의 **`std::find` 함수를 사용**할 수 있는데, 조건에 맞는 원소를 찾으면 **해당 원소를 가리키는 반복자를 반환**한다.

```C++
std::vector<int> vec;

vec.push_back(5);
vec.push_back(1);
vec.push_back(3);
vec.push_back(4);
vec.push_back(3);

auto e = std::find(vec.begin(), vec.end(), 3);
```
<br>

&nbsp;**Associative Container에도 `std::find`를 사용할 수는 있지만, 각 컨테이너에서 제공하는 `find` 함수보다 시간 복잡도 차이가 크기 때문에 사용을 지양하는 것이 좋다**. Associative Container에도 `<algorithm>`의 탐색 함수가 필요한 경우가 있는데, **`std::find_if`와 같이 찾고자하는 조건을 직접 지정하고자 할 때 사용**한다. `std::find_if`는 `std::remove_if`처럼 세 번째 인자로 조건 함수를 넘겨주어 사용한다.


&nbsp;그 외에도 **조건을 만족하는 원소가 존재하는지를 탐색하는 `std::any_of`, `std::all_of`도 있다**. 둘 다 조건 만족 원소 여부를 `bool` 형태로 반환하며, `std::find_if`처럼 사용한다. `std::any_of`는 범위 내 원소 중 하나라도 조건을 만족하면 `true`를 반환하고, `std::all_of`는 범위 내 모든 원소가 조건을 만족하면 `true`를 반환한다.
