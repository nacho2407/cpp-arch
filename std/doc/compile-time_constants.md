# Compile-time Constants

&nbsp;C로 코딩하다가 C++로 넘어오면서 이제는 대면대면한 사이가 되어버린, 정적 배열을 생성할 때 `a[size]`와 같이 사용할 수 없는 이유는 컴파일 과정에서 정적 배열이 메모리 상에 적재될 때 그 크기를 컴파일 타임에 알 수 없기 때문이다. **템플릿 인자나 정적 배열의 크기 등은 반드시 컴파일 타임에 확인할 수 있는 상수가 와야 하는데** 이를 오해하여 `const std::size_t SIZE = 10`과 같이 작성하면 사용이 가능하다고 생각할 수 있지만, 이런 형태는 런타임에 결정되는 상수라 여전히 사용할 수 없다.


## constexpr

&nbsp;이렇게 컴파일 타임에 바로 확인할 수 있도록 상수를 정의하는 방법이 있는데, `constexpr` 키워드를 사용하는 것이다. **`constexpr`은 해당 변수가 컴파일 타임에 확인 가능한 상수임을 알리는 키워드**로, 런타임에는 `const`와 동일하게 취급된다. 덕분에 아래와 같은 코드가 가능하다.

```C++
constexpr std::size_t SIZE = 3;

int arr[SIZE] = { 1, 2, 3 };
```


## constexpr Function

&nbsp;만약 **함수의 반환값을 컴파일 타임에 계산할 수 있다면, 해당 함수의 반환값 또한 컴파일 타임 상수가 와야하는 자리에 사용될 수 있다**. 함수의 반환 타입이 `constexpr`로 선언되어 있으면 되는데, 모든 함수가 `constexpr` 상수를 반환할 수 있는 것은 아니다. `constexpr` 상수를 반환하기 위한 함수의 조건은 C++ 표준 버전마다 조금씩 달라졌지만, 대표적으로 **리터럴 타입이 아닌 변수를 정의하거나, 변수를 정의하면서 초기화를 하지 않거나, `constexpr` 상수를 반환하지 않는 다른 함수를 호출하지만 않으면** `constexpr` 상수를 반환하도록 함수를 정의할 수 있다. 또한 당연하게도 **인자로 `constexpr` 상수가 아닌 값을 넘겨준다면 해당 함수로 `constexpr` 상수를 반환받을 수 없는데**, 대신 이 경우에는 런타임 상수를 반환하는 일반 함수로 인식된다.

```C++
constexpr int sum(int begin, int end)
{
        int sum = 0;

        for(int i = begin; i < end; i++)
                sum += i;

        return sum;
}

int main(void)
{
        int arr[sum(1, 3)] = { 0 };

        return 0;
}
```


## constexpr Constructor

&nbsp;앞서 함수가 `constexpr` 상수를 반환하기 위해서는 리터럴 타입이 아닌 변수를 정의해서는 안된다라고 얘기했었다. 여기서 리터럴 타입이란 보통 `void` 및 `char`, `int` 등 스칼라 타입을 의미하는데, 이 외에도 리터럴 타입이 될 수 있는 클래스를 정의하는 방법이 있다. 어떤 클래스가 리터럴 타입이 되기 위해서는 **디폴트 소멸자**를 가지면서, **Arggregate 타입(사용자 정의 생성자, 소멸자가 없고 모든 데이터 멤버가 `public`)이거나 `constexpr` 생성자를 가지면서 복사 및 이동 생성자가 없어야 한다**. 참고로 람다 함수도 리터럴 타입이다.

```C++
class Val
{
public:
        constexpr Val(int n): m_i(n) { }

        constexpr int get(void) { return m_i; };
private:
        int m_i;
}

constexpr Val add(Val a, Val b)
{
        // Val이 리터럴 타입이기 때문에 constexpr Val을 반환 가능

        return { a.get() + b.get() }; // 균일한 초기화 표현식
}

int main(void)
{
        constexpr Val v1{1};
        constexpr Val v2{2};

        int arr[add(v1, v2).get()] = { 0 };

        return 0;
}
```

&nbsp;당연하지만, `constexpr` 생성자를 가지는 클래스라도 사용할 멤버 함수가 `constexpr` 상수를 반환하지 않는 함수라면 컴파일 타임에 사용할 수 없다. 하지만 모든 멤버 함수가 `constexpr` 상수를 반환해야 하는 것은 아니다.


## if constexpr

&nbsp;`if constexpr`은 위의 `constexpr`과는 조금 다른 기능이다. 우리가 동일한 작업을 하는 함수를 일반 타입을 받을 때와 포인터 타입을 받을 때로 나누어서 개발하고자 할 때, 오버로딩을 하지 않고 하나의 함수 내에서 타입을 구분하여 작성하는 경우를 생각해보자.

```C++
template <typename T>
void is_ptr(T t)
{
        if(std::is_pointer_v<T>)
                std::cout << "pointer value: " << *t << std::endl;
        else
                std::cout << "primitive value: " << t << std::endl;
}
```

&nbsp;`std::is_pointer_v<T>` 또는 `std::is_pointer<T>::value`는 `<type_traits>`의 기능으로, `typename T`가 포인터 형인지를 확인할 때 사용된다. 위 코드의 문제는 템플릿이 컴파일 타임에 인스턴스화될 때 만약 `typename T`가 기본 자료형이라면 `*t`가 컴파일이 불가능한 코드가 된다는 것이다.


&nbsp;이런 경우에 사용하는 것이 `if constexpr`이다. `if constexpr`은 **조건문이 거짓일 경우 `if constexpr`문 내의 내용을 아예 컴파일하지 않는다**.

```C++
template <typename T>
void is_ptr(T t)
{
        if constexpr(std::is_pointer_v<T>)
                std::cout << "pointer value: " << *t << std::endl;
        else
                std::cout << "primitive value: " << t << std::endl;
}
```
