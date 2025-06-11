# Modern Type Wrapping

&nbsp;정적 타입 언어인 C++은 비교적 엄격한 타입 체킹으로 인해 프로그래밍하는 입장에서는 불편하게 느껴질 수 있다. C++의 규칙을 어기지 않으면서 타입을 좀 더 유연하게 다룰 수 있는 현대적인 타입 래퍼(또는 컨테이너) 들이 있는데, 값이 있거나 없어도 되는 타입이나 여러 타입 중 하나의 타입이 될 수 있는 타입 등이 제공된다.


## &lt;optional>

&nbsp;`std::map<T>`같은 자료구조에서 데이터를 받아오는 함수를 작성한다고 가정하자. 우리가 키를 전달해주면 함수는 해당 키를 통해 값을 얻어오게 되는데, 만약 해당 키가 자료구조 내에 존재하지 않는다면 함수는 어떤 값을 반환해야할까? 그냥 예외로 처리하거나 `bool` 타입 등을 제공하여 값을 올바르게 찾았는지 확인시킬 수도 있지만 신뢰도나 함수 설계를 생각해보면 썩 좋은 방법은 아니다.


&nbsp;이런 경우에는 보통 디폴트 객체를 반환하도록 설계된 함수가 많다. 하지만 이런 방식의 문제점은, 반환하고자하는 타입이 디폴트 생성자를 지원하지 않을 수도 있고, 탐색하는 자료구조 내에 디폴트 객체가 이미 사용되고 있다면 이러한 값과 비교할 수 없다는 것이다.


&nbsp;**`std::optional<T>`는 이러한 '값이 있을 수도 있고 없을 수도 있는' 상황에서 유용하게 사용할 수 있는 클래스**다.

```C++
#include <iostream>
#include <optional>

class Nacho
{
public:
        Nacho(int i): n(i) {  }

        int n;
};

std::optional<Nacho> get_five_nacho(bool sure)
{
        if(sure)
                return Nacho(5);
        else
                return std::nullopt;
}

int main(void)
{
        auto nacho = get_five_nacho(true);

        if(nacho)
                std::cout << nacho->n << " nacho get!" << std::endl;
        else
                std::cout << "No nacho..." << std::endl;

        return 0;
}
```
<br>

&nbsp;`std::optional<T>`에는 보관하고자 하는 타입을 받는 생성자가 정의되어 있어 **그냥 해당 타입의 값을 반환하더라도 자동으로 `std::optional<T>` 객체가 생성되어 전달**된다. `std::optional<T>`의 큰 장점으로 객체에 보관하는 과정에 **동적 할당이 발생하지 않기 때문**에(`std::optinal<T>` 자체에 객체가 포함) 불필요한 오버헤드를 걱정하지 않아도 된다.


&nbsp;전달하고자하는 값이 없는 경우, 디폴트 객체를 생성하지 않고 **`std::nullopt`를 통해 값이 없음**을 알릴 수 있으며, 이를 통해 객체 자체에 `bool`로 변환되는 캐스팅 연산자가 포함되어 있어 그냥 **`std::optional<T>`를 조건문에서 검사할 수 있다**(명시적으로 확인하고자 한다면 `std::optional<T>::has_value`를 사용해도 된다).


&nbsp;`std::optional<T>` 객체의 값을 사용할 때는 `std::optional<T>::value` 함수를 통해 값을 받을 수 있으나, 역참조 연산자(`*`)를 사용해도 동일하게 값을 얻을 수 있다.


&nbsp;한 가지 주의할 점은, 아직 제대로 정착하지 못한(2025.02.) C++ 23을 사용하는 경우가 아니라면, `std::optional<T>`에 **레퍼런스 타입을 사용할 수 없다**는 것이다. 대신 `std::reference_wrapper`를 통해 래핑하여 사용할 수는 있다.

```C++
#include <functional>
#include <iostream>
#include <optional>

class Nacho
{
public:
        Nacho(int i): n(i) {  }

        int n;
};

int main(void)
{
        Nacho nacho(8);
        std::optional<std::reference_wrapper<Nacho>> r_n = std::ref(nacho);

        r_n->get().n = 16;

        std::cout << "nacho.n >> " << nacho.n << std::endl;

        return 0;
}
```


## &lt;variant>

&nbsp;하나의 함수에서 여러 타입의 값을 반환하고 싶다고 생각한 적 있는가? 다형성을 이용하여 이런 패턴을 구현할 수 있지만, 여러 타입의 값을 이용하고 싶다는 이유로 별 관계없는 타입들을 하나의 부모 클래스로 묶는 것은 설계 상 좋아보이지 않는다. 기존 C-Style로 공용체를 사용하여 여러 타입의 값을 사용할 수는 있었지만 공용체의 경우 현재 어떤 값이 저장되어 있는지를 확실히 확인할 수 없어 사용에 위험이 있었다. **`std::variant<T>`는 이런 one-of 패턴을 구현한 클래스**다.


&nbsp;`std::variant<T>`는 템플릿을 통해 사용할 타입들을 받으며, 이 중에 하나를 타입으로 쓸 것이라는 뜻이기에 **중복된 타입이 들어갈 수는 없다**. 컴파일 타임에 사용할 타입이 결정되어야하기 때문에 **`std::variant<T>`는 항상 초기화된 상태**여야하고, 만약 비어있는 `std::variant<T>`를 생성하면, 첫 번째 템플릿 인자 타입의 디폴트 생성자가 호출된다(이 경우 해당 타입에 디폴트 생성자가 없으면 에러가 발생한다). `std::optional<T>`처럼 생성 시 동적 할당을 수행하지 않기 때문에 오버헤드를 걱정하지 않아도 된다.


&nbsp;`std::variant<T>`에서 값을 가져오는 방법은 크게 두가지다. **저장되어 있는 타입을 정확히 알고 있는 상황이면 `std::get<T>` 함수를 통해 바로 값을 얻어올 수 있고**(이 경우 `T`에 타입 대신 `std::varaint<T>`의 타입 인덱스를 사용해도 된다), 또다른 방법으로는 `std::visit` 함수를 통해 컴파일 타임 타입 분기(`<type_traits>`)를 활용하여 값을 이용하는 방법이다.

```C++
#include <iostream>
#include <type_traits>
#include <variant>

class Nacho
{
public:
        int n;
};

class Cheese
{
public:
        int c;
};

std::variant<Nacho, Cheese> getNachoCheese(bool is_nacho)
{
        if(is_nacho)
                return Nacho{8};
        else
                return Cheese{16};
}

int main(void)
{
        auto v = getNachoCheese(true);

        std::visit([](auto&& v) {
                if constexpr(std::is_same_v<std::decay_t<decltype(v)>, Nacho>)
                        std::cout << "Nacho >> " << v.n << std::endl;
                else
                        std::cout << "Cheese >> " << v.c << std::endl;
        }, v);

        return 0;
}
```

&nbsp;`std::visit`은 `<variant>`에 포함된 함수로, `std::variant<T>` 객체의 값에 쉽게 접근할 수 있게 해준다. 첫 번재 인자로 방문자 함수를 넘겨주는데, 타입 추론과 완벽한 전달 패턴을 위해 **거의 반드시 `auto&&` 타입을 매개 변수로 사용**한다.


&nbsp;`std::optional<T>`의 `std::nullopt`처럼 아무 값도 가지고 있지 않은 상태를 표현하고자 한다면 `std::monostate`를 템플릿 인자로 사용하면 된다. 특히 첫 타입으로 `std::monostate`를 지정하면 빈 객체를 생성할 때 디폴트로 `std::monostate` 상태가 되어 타입 관련 위험을 해소할 수 있다.


## &lt;tuple>

&nbsp;Python의 사용성을 증대시켜주는 큰 요인 중 여러 타입을 쉽게 묶을 수 있는 Tuple이 존재하는데, C++에서는 동일 타입의 객체들이야 `std::vector<T>`나 배열을 통해 저장할 수 있지만, 서로 다른 타입의 객체들을 저장하려면 일반적으로 구조체를 따로 정의해서 저장해야만 했다.


&nbsp;C++ 11부터는 `<tuple>`헤더가 추가되어 서로 다른 타입 객체들을 간단하게 묶을 수 있는 방법을 제공한다. `std::tuple<T>`을 그냥 사용하려면 `std::tuple<int, std::string, double>`처럼 못생기게 사용해야 하지만, 다행히 `std::pair<T>`의 `std::make_pair`처럼 `std::make_tuple`이란 함수를 제공하기 때문에 `std::tuple<T>`를 쉽게 생성할 수 있다. 값을 사용할 때는 `std::variant<T>`처럼 `std::get<T>` 함수를 사용한다.

```C++
#include <iostream>
#include <string>
#include <tuple>

std::tuple<int, std::string> get_information(int id)
{
        if(id == 0)
                return std::make_pair(0, "Nacho");
        else
                return std::make_pair(1, "Cheese");
}

int main(void)
{
        auto emp = get_information(0);

        std::cout << "Id: " << std::get<0>(emp) << "\n";
        std::cout << "Name: " << std::get<1>(emp) << std::endl;

        return 0;
}
```
