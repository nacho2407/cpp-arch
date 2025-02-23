# Fragments

&nbsp;단락 모음.


## Keywords

- explicit

&nbsp;C++ 컴파일러는 특정 클래스가 들어와야 할 자리에 다른 타입의 값이 들어오면, **해당 타입을 받는 클래스의 생성자를 자동으로 호출하여 사용**하는데, 이를 **암시적 변환**이라고 부른다. **생성자에 `explicit` 키워드를 사용하여 해당 생성자를 통해 암시적 변환이 일어나는 것을 막을 수 있다**.

```C++
class dorito
{
pubilc:
        dorito();

        explicit dorito(int n);

        //...

        ~dorito();
private:
        int dodohan;

        //...
}
```
<br>

- mutable

&nbsp;**`mutable` 키워드는 멤버 변수를 변화시키지 않는 constant 멤버 함수 내에서도 해당 멤버 변수를 변화시킬 수 있도록 한다**. 클로저 객체를 `mutable`로 선언하면 캡처한 변수들을 함수 내에서 변화할 수 있도록 하는데, 결국은 값으로 캡처한 변수들이라 함수 외부에 영향을 주지는 않는다. 함수 외부 변수의 값을 바꾸려면 레퍼런스로 캡처하여야 한다.


- noexcept

&nbsp;`noexcept`는 해당 함수가 예외를 발생시키지 않는다는 것을 컴파일러에게 명시적으로 알려주는 키워드다. **이동 연산을 구현하는 오버로드 함수는 예외를 발생시키지 않도록 정의하고 이를 `noexcept` 키워드를 활용하여 이를 컴파일러에게 명시**하는 것이 좋은데, 왜냐하면 **컴파일러는 연산의 효율 상승보다 예외에 대한 안정성을 확보하는 것을 더 중요하게 여기기 때문**이다. 실제로 `noexcept` 키워드 없이 정의한 오버로드 함수는 때때로 예상과 달리 이동 연산이 수행되지 않는 경우를 볼 수 있다. 특히 Standard Libraries의 `std::vector`는 오버로드 함수가 `noexcept`로 선언되어 있지 않으면 이동 연산이 수행되지 않는다.


- delete

&nbsp;`delete`는 동적 메모리의 해제 뿐 아니라 C++ 11부터는 원치 않는 함수를 의도적으로 삭제하는데에도 사용된다. 컴파일러는 가끔 우리가 의도하지 않는 생성자 등을 (시키지도 않았는데!) 암시적으로 생성해서 사용하는 경우가 있는데, 이러한 경우를 의도적으로 막고자 한다면 `dorito(const int& n) = delete`와 같이 `delete`를 이용하여 삭제된 함수임을 컴파일러에게 명시할 수 있다.


- sizeof...

&nbsp;`sizeof` 연산자는 인자의 크기를 반환한다. **파라미터 팩에 `sizeof...` 연산자를 사용하면 해당 파라미터 팩이 포함하고 있는 인자의 개수를 반환한다**.


## Functor

&nbsp;각 원소에 적용할 연산은 람다 함수를 사용하면 간단하게 표현이 가능하나, 특수한 상황에서 Functor를 사용해야하는 경우가 생길 수 있다. Functor는 다음과 같이 작성한다.

```C++
template <typename T>
class functor
{
public:
        functor(T value): factor(value) {}

        template <typename U>
        void operator ()(U& value)
        {
                // ...
        }
private:
        T factor; // Optional
}
```

&nbsp;`class`대신 `struct`를 사용할 수도 있고, `class`로 구현할 경우 생성자와 괄호 연산자를 클래스 외부에서 정의할 수도 있다(짧은 함수라면 그냥 클래스 정의 내에서 함수 정의 추천). Functor 내 각 매개변수는 필요에 따라 레퍼런스 타입으로 선언한다.


## Scope Operator

&nbsp;`.` 연산자나 `->` 연산자와는 달리 스코프 연산자 `::`는 클래스의 정적 멤버 변수 접근에 사용된다. 정적 멤버 또한 `.`나 `->`를 통해 접근할 수 있지만, `::`를 사용하는 것이 더 일반적이다. `using`을 이용한 타입 멤버 또한 스코프 연산자를 통해 접근한다.


## Exception Handling

&nbsp;기본 C++ 문법이라 짧게만 정리한다. `try` 문을 실행하다가 `throw`로 예외가 던져지면 `catch` 문에서 잡아 처리한다.

&nbsp;C++의 모든 예외는 `<exception>` 헤더의 `std::exception` 클래스를 상속받으며, `std::exception::what` 함수를 통해 예외에 대한 설명을 반환한다. `<stdexcept>` 헤더는 구체적인 표준 예외 클래스를 제공한다. 이것저것 있지만 `std::runtime_error` 정도만 알고가자.
