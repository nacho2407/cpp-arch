# Compile-time Constants

&nbsp;C로 코딩하다가 C++로 넘어오면서 이제는 대면대면한 사이가 되어버린, 정적 배열을 생성할 때 `a[size]`와 같이 사용할 수 없는 이유는 컴파일 과정에서 정적 배열이 메모리 상에 적재될 때 그 크기를 컴파일 타임에 알 수 없기 때문이다. **템플릿 인자나 정적 배열의 크기 등은 반드시 컴파일 타임에 확인할 수 있는 상수가 와야 하는데** 이를 오해하여 `const std::size_t SIZE = 10`과 같이 작성하면 사용이 가능하다고 생각할 수 있지만, 이런 형태는 런타임에 결정되는 상수라 여전히 사용할 수 없다.


## constexpr

&nbsp;이렇게 컴파일 타임에 바로 확인할 수 있도록 상수를 정의하는 방법이 있는데, `constexpr` 키워드를 사용하는 것이다. **`constexpr`은 해당 변수가 컴파일 타임에 확인 가능한 상수임을 알리는 키워드**로, 런타임에는 `const`와 동일하게 취급된다. 때문에 아래와 같은 코드가 가능하다.

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


(작성중) - constexpr 생성자, if constexpr
