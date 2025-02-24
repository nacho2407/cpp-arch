# Perfect Forwarding

&nbsp;`bar` 함수가 Lvalue와 Rvalue 레퍼런스를 받도록 오버로딩한 상황에서, 이번엔 아래와 같은 함수를 작성한다고 가정해보자.

```C++
template <typename T>
void foo(T x)
{
        bar(x);
}
```

&nbsp;이런 함수를 래퍼(Wrapper) 함수라고 한다. 이런게 왜 필요하나 생각할 수도 있지만 Standard Libraries를 사용할 때도 종종 사용하는 코드다.


&nbsp;이런 래퍼 함수가 비효율적인 이유는 단지 `bar`라는 함수를 호출하기 위해서 `foo`라는 함수가 매개 변수의 크기만큼 메모리를 할당해야하기 때문이다. 메모리에 접근하는 연산은 매우 비용이 높기 때문에 시간 / 공간적으로 매우 비효율적이다. 이런 경우 간단히 **`bar`에게 `foo`가 받은 인자를 바로 전달하도록 구현할 수 있는데, 이를 완벽한 전달(Perfect Forwarding)이라고 한다**. 완벽한 전달을 수행할 때는 Rvalue 레퍼런스를 이용하여 전달할 수 있는데, 아래와 같이 구현한다.

```C++
template<typename T>
void foo(T&& x)
{
        bar(std::forward(x));
}
```

&nbsp;위와 같이 구현하면 'Rvalue만 인자로 받을 수 있게되는 것 아닌가?'하고 생각할 수도 있지만, C++ 11부터 도입된 **레퍼런스 겹침 규칙**(Reference Collapsing Rule)에 의해 `T&`(Lvalue)가 들어오면 `T&&&` -> `T&`, `T&&`(Rvalue)가 들어오면 자동으로 `T&&&&` -> `T&&`로 추론되기 때문에 Lvalue와 Rvalue를 모두 받을 수 있다.


&nbsp;이러한 패턴을 **보편적 레퍼런스**(Universal Reference)라고 하는데, 주의해야 할 점은 아래 코드와 같이

```C++
void foo(int&& x)
{
        bar(std::forward(x));
}
```

&nbsp;**템플릿을 사용하지 않고 그냥 사용하면 보편적 레퍼런스가 되는 것이 아니라 Rvalue 레퍼런스만 받는 함수**가 된다.


&nbsp;C++은 템플릿을 이용한 타입 추론 시, `const` 타입의 경우에는 그냥 `const`를 때버리고 타입 추론을 해버리기 때문에 **보편적 레퍼런스로 상수형 타입도 받을 수 있다**. `bar`가 상수형 레퍼런스를 받는 함수로 따로 오버로딩되어 있다면 해당 함수로 로딩된다.


## std::forward&lt;T>

&nbsp;`std::forward<T>`는 `std::move`와 함께 Rvalue 레퍼런스를 반환하는 함수로, `<utility>`에 정의되어 있다. 위의 `bar` 함수에서 그냥 `std::move`를 사용해버리면 `foo`가 받은 인자가 Lvalue로 추론되든, Rvalue로 추론되든 간에 모두 `bar(T&& x)`로 로딩되어 버린다. 이럴 때 사용하는 것이 `std::forward<T>`다. **`std::forward<T>`는 레퍼런스 겹침 규칙에 따라 알아서 Lvalue, Rvalue를 결정해 반환해준다**.
