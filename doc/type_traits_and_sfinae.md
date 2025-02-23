# Type Traits & SFINAE

## decltype

&nbsp;`decltype`은 **타입을 알고자하는 식을 받아 해당 타입으로 치환**되는 C++ 키워드다. 함수처럼 사용되지만 실제로는 키워드다. `decltype`에 일반적인 식별자 표현식을 전달하면 해당 식별자의 타입으로 치환되지만, 그 외의 식을 전달하면 어떤 식이 전달되느냐에 따라 결과가 달라진다.


&nbsp;`decltype(a + b)`와 같이 해당 결과 값이 Rvalue(Prvalue)인 경우에는 그냥 해당 결과 타입으로 치환된다. 참고로 이 때 **`decltype` 내의 연산은 컴파일 타임에 타입을 추론하는데에만 사용되고 실제로 해당 위치에서 런타임에 호출되는 것이 아니기 때문에 주의해야 한다**. 해당 연산이 실제로 일어날 것이라고 생각해서는 안된다. `decltype(std::move(x))`와 같이 결과 값이 소유권을 포기한 Xvalue인 경우에는 자연스럽게 Rvalue 레퍼런스 타입(`T&&`)으로 치환된다. `decltype((a))`와 같이 식별자 표현식이 아니면서 그냥 Lvalue인 경우에는 `decltype(a)`과 달리 레퍼런스 타입(`T&`)으로 치환된다.


&nbsp;자동으로 타입을 추론하는 `auto`가 있는데 왜 굳이 `decltype`을 사용하느냐고 생각할 수 있지만, `decltype`은 우리가 의도한 타입을 정확하게 전달할 수 있다는 장점이 있다. 다음 코드의 예를 보자.

```C++
const int i = 4;
int arr[4] = { 0 };

auto auto_i = i;
auto auto_arr = arr;

decltype(i) decl_i = i;
decltype(arr) decl_arr;
```

&nbsp;위 코드의 `auto_i`는 타입 추론될 때 `const`를 때버리지만, `decl_i`는 `const int`로 그대로 추론되며, `auto_arr` 또한 배열이 아닌 `int*` 타입으로 추론되는 반면, `decl_arr`은 `int[4]` 타입으로 정확하게 추론된다(때문에 `decl_arr = arr`처럼 사용할 수는 없다).


&nbsp;`decltype`은 템플릿과 함께 사용할 때 아래와 같이 매우 편리하게 사용할 수 있다.

```C++
template <typename T, typename U>
void add(T t, U u, decltype(t + u)& result)
{
        result = t + u;
}
```
<br>

&nbsp;다만 다음과 같이 함수의 반환 타입으로 사용할 때는 문제가 발생할 수 있다.

```C++
template <typename T, typename U>
decltype(t + u) add(T t, U u)
{
        return t + u;
}
```

&nbsp;위 코드의 문제는 `t`와 `u`가 `decltype`이 사용되는 지점 기준으로 뒤에서 정의된다는 문제가 있다. `decltype`에서 사용할 시점에는 알 수 없는 식이라 컴파일 에러가 발생한다. C++ 14부터는 아래와 같이 람다 함수 형태로 작성하여 사용한다.

```C++
template <typename T, typename U>
auto add(T t, U u) -> decltype(t + u)
{
        return t + u;
}
```


## std::declval

&nbsp;`std::declval`은 `decltype`과 달리 `<utility>`에 정의된 함수다. 아래의 코드를 보자.

```C++
template <typename T>
decltype(T().f()) foo(T t)
{
        return t.f();
}

class A
{
public:
        f() { return 1; }
}

class B
{
public:
        B(int n): b(n) { }

        f() { return b; }
private:
        int b;
}

int main(void)
{
        A a();
        B b(4);

        foo(a);
        foo(b);

        return 0;
}
```

&nbsp;위 코드는 `foo(b)`로 인해 컴파일 에러가 발생한다. 템플릿 인스턴스화 과정에서 `B()`라는 디폴트 생성자가 정의되어 있지 않음에도 `decltype`에서 `T()`처럼 기본 생성자를 사용하려하기 때문이다.


&nbsp;위에서 얘기하였듯이 `decltype`의 식은 컴파일 타임에 타입 추론에만 사용될 뿐 실제로 호출이 되는 것은 아니라고 하였는데, 그 덕분에 아래와 같이 `std::declval`을 통하여 문제를 해결할 수 있다.

```C++
#include <utility>

template <typename T>
decltype(std::declval<T>().f()) foo(T t)
{
        return t.f();
}

class A
{
public:
        f() { return 1; }
}

class B
{
public:
        B(int n): b(n) { }

        f() { return b; }
private:
        int b;
}

int main(void)
{
        A a();
        B b(4);

        foo(a);
        foo(b);

        return 0;
}
```
<br>

&nbsp;`std::declval`은 **템플릿 인자로 받은 타입을 따로 생성 과정을 거치지 않고 객체가 존재하는 것처럼 사용할 수 있도록 한다**. 물론 이런 경우는 컴파일 타임에 타입 추론에만 사용되기 때문에 타입 연산 외에 런타임에 사용하려하면 에러가 발생한다.


## &lt;type_traits>

&nbsp;`<type_traits>`에는 타입과 관련된 많은 함수가 포함되어 있다. 이러한 함수들은 보통 템플릿 메타 프로그래밍 기법을 통해 작성되어 있는데, 예를 들어 해당 객체가 `void` 타입인지 확인하는 `std::is_void<T>`의 작동 원리를 보면 아래 코드와 유사한 형태로 되어있다.

```C++
template <typename T>
class is_void
{
public:
        static constexpr bool value = false;
}

template<>
class is_void<void>
{
public:
        static constexpr bool value = true;
}
```

&nbsp;위의 코드는 템플릿 특수화를 사용하여 `T`가 `void`가 아니면 `is_void<T>::value`가 거짓이도록 구성되어 있다.


&nbsp;C++ 11에 추가된 키워드 중에 `static_assert`라는 것이 있는데, `constexpr bool`을 검사하여 거짓이면 컴파일 과정에서 에러를 출력하는 키워드다. `<type_traits>`의 메타 함수들과 함께 사용하여 정말 특정 타입이 와야하는 경우를 보장할 수 있다.


## Runtime Type Checking

&nbsp;`<type_traits>`에 포함되어 있는 `std::is_same_v<T>`는 템플릿 인자로 받은 두 타입이 같은 타입인지를 확인한다. C++에서는 const 여부나 레퍼런스형 여부도 모두 확인하기 때문에 보통 이러한 미사여구를 떼어주는 `std::decay_t<T>`와 함께 사용한다.


&nbsp;`if constexpr`과 함께 사용되어 컴파일 타임 타입 분기도 수행할 수 있다.

```C++
#include <iostream>
#include <type_traits>

template <typename T>
void int_checking()
{
        if constexpr(std::is_same_v<std::decay_t<T>, int>)
                std::cout << "T is int!" << std::endl;
        else
                std::cout << "T is not int!" << std::endl;
}

int main(void)
{
        int_checking<int>();
        int_checking<const int&>();
        int_checking<long long int>();
        int_checking<double>();
        
        return 0;
}
```


## SFINAE

&nbsp;`<type_traits>`에는 해당 객체가 클래스 타입인지를 확인하는 메타 함수가 있다! 가능한 구현은 아래와 같은 형태다.

```C++
namespace detail
{
        template <typename T>
        char test(int T::*);

        struct two
        {
                char c[2];
        }

        template <typename T>
        two test(...);
}

template <typename T>
class is_class: std::integral_constant<bool, sizeof(detail::test<T>(0)) == 1 && !std::is_union_v<T>> { };
```

&nbsp;보기 싫은 TMP 코드의 특징을 여실히 보여주고 있다... 위 코드를 이해하기 위해서는 우선 알아야 할 것들이 몇가지 있다.


&nbsp;우선 `int T::*`라는 형태가 익숙하지 않은데, 이는 **`T`라는 클래스 내부의 `int` 타입 멤버를 가리킬 수 있는 포인터 타입**을 의미한다. 다른 `int` 타입 자료는 가리킬 수 없고, 오직 `T` 내부의 `int` 멤버만 가리킬 수 있다. 그냥 함수의 선언인 만큼, 매개변수의 이름은 생략되어있는 상태다.


&nbsp;`std::integral_constant<T, V>`는 단순히 `T` 타입의 `V` 값을 가지는 `static` 자료 `value`를 가지는 클래스다. 즉, 현재 `is_class`라는 클래스는 타입이 `bool`이고 값이 `sizeof(detail::test<T>(0)) == 1 && !std::is_union_v<T>`인 `static` 자료 `value`를 상속받은 상태다.


&nbsp;`detail::test<T>(0)`는 `T`에 따라 `test(int T::*)`가 인스턴스화 될 지, `test(...)`가 인스턴스화 될 지를 결정하게 된다. `int T::*` 같은 형태는 클래스나 공용체 자료만 컴파일이 가능한 코드이기 때문에 클래스인지 아닌지를 구분할 수 있게 된다.


&nbsp;클래스나 공용체 자료가 아닌 자료를 검사할 때 `test(int T::*)`라는 코드에서 컴파일 에러가 발생하지 않는 이유는, C++의 **'치환 실패는 에러가 아니다'('Substitution Failure Is Not An Error', SFINAE)**라는 원칙 때문이다. 덕분에 컴파일 에러가 발생하는 것이 아니라, 컴파일 타임에 템플릿 인스턴스화 과정에서 그냥 무시된다. 다시 말해 **그런 함수 자체가 컴파일되지 않는 것**이다!


&nbsp;즉, `detail::test<T>(0)`이 어떤 크기의 타입을 반환하는지에 따라 클래스인지 아닌지를 구분할 수 있게 된다. **`sizeof`에서 사용되는 함수 호출은 `decltype`처럼 컴파일 타임에만 사용되고, 실제로 해당 함수가 호출되는 것이 아니기 때문에** `detail::test<T>`는 선언만 되어있어도 된다.


&nbsp;하나 더 참고할 만한 부분은, `sizeof(char)`인데, C++ 표준에서는 시스템 종류에 상관없이 `sizeof(char) == 1`을 만족한다고 규정해두었다. `char`의 크기가 항상 1 바이트라는 것인데, 재미있는 것은 시스템의 종류에 따라 1 바이트가 항상 8 비트를 의미하지는 않는다는 것이다. 현대의 시스템 대부분은 1 바이트가 8 비트이지만, 과거 일부 시스템에서는 1 바이트가 16 비트인 경우도 있었다고 한다...


&nbsp;코드에서 `std::is_union_v<T>`(`std::is_union<T>::value`)도 사용되는데, 사실 C++에는 해당 자료형이 공용체 타입인지 구분할 방법이 없어서 이를 구현한 방법은 사용하는 시스템 하드웨어에 따라 다르다고 한다.


## std::enable_if&lt;T>

&nbsp;SFINAE를 이용하여 우리가 원하는 타입에만 템플릿 인스턴스화가 일어나도록 하는 방법이 있다. `<type_traits>`의 `std::enable_if_t<T>`(`std::enable_if<T>::type`)는 `constexpr bool`을 템플릿 인자로 받는 클래스인데, **만약 `T`가 참이면 해당하는 타입으로 치환되어 정상적으로 템플릿 인스턴스화되고, 거짓이면 인스턴스화 과정에서 치환될 타입이 없어져서 그 템플릿 자체가 무시된다**. 아래 코드는 `char`나 `int`같은 정수형 자료에만 인스턴스화되는 템플릿 함수다.

```C++
template <typename T, typename = typename std::enable_if_t<std::is_integral_v<T>>>
void foo(T t)
{
        std::cout << "t: " << t << std::endl;
}
```

&nbsp;`typename =`과 같이 쓴 이유는 단순히 인스턴스화 여부만 결정할 디폴트 인자이기 때문에 그냥 익명 타입으로 작성해도 되기 때문이고, `std::enable_if_t<T>` 앞에 `typename`이 붙은 이유는 `std::enable_if_t<T>`가 `T`에 따라 달라지는 의존 타입이기 때문이다(의존 타입은 반드시 앞에 `typename`을 붙여 타입임을 알려주어야 한다).


## std::void_t&lt;T>

&nbsp;`std::enable_if_t<T>`외에도 `decltype` 키워드를 이용하여 템플릿의 인스턴스화 여부를 결정하는 방법이 있는데, 보통 어떤 클래스가 반드시 특정 함수를 포함시켜야만 할 때 사용한다. 아래와 같은 경우다.

```C++
template <typename T, typename = decltype(std::declval<T>().print())>
void print_t(const T& t)
{
        t.print();
}
```

&nbsp;만약 `T`가 `print`라는 함수를 가지고 있지 않으면 `decltype(std::declval<T>().print())` 자체가 불가능한 문장이 되므로 인스턴스화 과정에서 무시된다. 만약 반환 타입까지 강제하고자 한다면 다음과 같이 구현한다.

```C++
template <typename T, typename = typename std::enable_if_t<std::is_void_v<decltype(std::declval<T>().print())>>>
void print_t(const T& t)
{
        t.print();
}
```

&nbsp;점점 한 눈에 알아보기 힘든 코드가 되고 있다. 심지어 Iterator의 `begin`, `end`와 같이 반드시 포함해야 하는 함수가 여러 개라면 템플릿 인자를 해석하는 것만으로도 끔찍한 일이 될 것이다.


&nbsp;도토리 키재기긴 하지만, 이러한 코드 길이를 조금 줄일 수 있는 방법이 있다. `std::void_t<T>`는 그냥 `template<typename ...> using void_t = void;`로 정의된 타입으로, `void_t<A, B, C, D>`와 같이 여러 템플릿 인자를 받을 수 있으나 결과는 그냥 항상 `void`인 타입이다. 인스턴스화 여부를 결정할 때는 그냥 해당 코드가 말이 되나 안되나만 결정하면 되기 때문에 여러개의 함수를 포함해야하는 경우에는 다음과 같이 작성하여 조금은 간결하게 만들 수 있다.

```C++
template <typename T, typename = std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>>
void print_t(const T& t)
{
        for(auto i = t.begin(); i != t.end(); i++)
                std::cout << *i << " ";

        std::cout << std::endl;
}
```
<br>

&nbsp;C++ 20부터 등장한 concepts로 인해 앞으로 점점 끔찍한 TMP 코드를 볼 일은 줄겠지만, 세상엔 항상 레거시 코드라는 것이 존재하는 만큼 이런 메타 프로그래밍 코드를 해석하는 실력을 기르는 것은 분명 도움이 될 것이다.
