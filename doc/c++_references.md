# C++_References

&nbsp;C++ 개발하면서 참고할 만한 내용 모음(주로 모던 C++ 내용)


## 클래스 선언 시 고려해야 할 것들

- (기본) 생성자
- 복사 생성자(동적 할당 고려)
- 이동 생성자
- 대입 연산자(동적 할당 고려)
- 이동 대입 연산자
- 소멸자


## 범위 기반 for-loop

&nbsp;**`<argorithm>`의 `std::for_each`**. `func`의 반환값은 무시된다. `InputIterator`를 역참조한 결과는 `Type`과 동일하거나 `Type`으로 변환 가능해야 한다.

```C++
// func은 아래와 같은 형태. 반드시 constant reference일 필요는 없음.
void func(const Type& arg);

template <typename InputIterator, typename Function>
Function std::for_each(InputIterator begin, InputIterator end, Function func);
```
<br>

&nbsp;**`<algorithm>`의 `std::transform`**. **`std::for_each`는 container 원소의 내용에 변화를 주지 않으나, `std::transform`는 container 원소의 내용에 변화를 줄 수 있다**.

&nbsp;`UnaryOperation`은 하나의 container에, `BinaryOperation`은 두 개의 container에 각 연산을 수행한다. `d_begin`은 결과를 저장할 범위로, `begin`(1/2)과 동일해도 된다. `InputIterator`(1/2)를 역참조한 결과는 `Type`(1/2)과 동일하거나 `Type`(1/2)으로 변환 가능해야 한다. 동일하게, `OutputIterator`를 역참조한 결과는 `ReturnType`과 동일하거나 `ReturnType`으로 변환 가능해야 한다.

```C++
// unary_op는 아래와 같은 형태. 반드시 constant reference일 필요는 없음.
ReturnType unary_op(const Type& arg);

// binary_op는 아래와 같은 형태. 반드시 constant reference일 필요는 없음.
ReturnType binary_op(const Type1& arg1, const Type2& arg2);

template <typename InputInterator, typename OutputIterator, typename UnaryOperation>
OutputIterator std::transform(InputIterator begin, InputIterator end, OutputIterator d_begin, UnaryOperation unary_op);

template <typename InputIterator1, typename InputIterator2, typename OutputIterator, typename BinaryOperation>
OutputIterator std::transform(InputIterator1 begin1, InputIterator1 end1, InputIterator2 begin2, InputIterator2 end2, OutputIterator d_begin, BinaryOperation binary_op);
```
<br>

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
                // value에 필요한 작업
        }
private:
        T factor;
}
```

&nbsp;`class`대신 `struct`를 사용할 수도 있고, `class`로 구현할 경우 생성자와 괄호 연산자를 클래스 외부에서 정의할 수도 있다(짧은 함수라면 그냥 클래스 정의 내에서 함수 정의 추천). Functor 내 각 매개변수는 필요에 따라 레퍼런스 타입으로 선언한다.


## 람다(Lambda) 함수 & 클로저(Closure)

&nbsp;간단하게 **익명 함수**를 생성하는 방법. 특히 범위 기반 for-loop와 함께 사용되어 매우 유용하게 사용할 수 있다. 함수의 내용과 그 함수가 정의된 환경을 함께 묶은 개념을 **클로저**라고 하는데, 함수가 정의되는 시점의 환경을 기억하고 그 환경에서 외부 변수에 접근할 수 있도록 한다. 람다 함수는 다음과 같이 정의한다.

```C++
[introducer_capture] (parameters) -> return_type { statements; }
```

&nbsp;parameters와 return_type은 없다면 생략이 가능하나, **`[]`는 생략할 수 없다**.


&nbsp;람다 함수를 사용할 때 단순히 인자로 값을 넘겨서 함수 내부에서 사용할 수도 있지만, `std::for_each`등 Standard Libraries와 함께 사용하는 경우와 같이 인자를 마음대로 설정할 수 없는 경우가 있는데, 이 때 외부의 변수를 함수 내부에서 사용할 수 있도록 introducer_capture에 선언할 수 있다.


&nbsp;`[&] {}`와 같이 작성하여 외부의 모든 변수를 레퍼런스로 캡처하거나 `[=] {}`와 같이 작성하여 외부의 모든 변수를 값으로 캡처할 수 있다. 물론 `[=, &x, &y] {}`와 같이 지정한 방식으로 캡처할 수도 있다. **캡처되는 내용은 컴파일 기준이 아닌 해당 클로저 객체가 생성되는 시점을 기준으로 한다**. 값으로 캡처되는 변수가 클래스 형이라면 해당 클래스의 (복사) 생성자 또한 클로저 객체가 생성되는 시점에 호출된다.


&nbsp;주의할 점은 **값으로 캡처된 변수들은 constant로 취급**되기 때문에 캡처된 변수의 값을 바꿀 수 없는데, **`[=] mutable {}`와 같이 `mutable` 키워드를 사용하면 캡처된 값들이 함수 내의 지역 변수처럼 취급**된다.


&nbsp;**`mutable` 키워드는 멤버 변수를 변화시키지 않는 constant 멤버 함수 내에서도 해당 멤버 변수를 변화시킬 수 있도록 한다**. 클로저 객체를 `mutable`로 선언하면 캡처한 변수들을 함수 내에서 변화할 수 있도록 하는데, 결국은 값으로 캡처한 변수들이라 함수 외부에 영향을 주지는 않는다. 함수 외부 변수의 값을 바꾸려면 레퍼런스로 캡처하여야 한다.


&nbsp;람다 함수를 이용한 클로저 객체는 설계 의도 상 **어떤 형태의 함수가 사용될 지 바로 예측할 수 없기 때문**에, 실제로 함수의 인자 등으로 사용하려면 **템플릿이나 `auto`로 사용**할 수 있다. 함수 포인터를 이용해서 저장할 수도 있는데, 이 경우 외부 변수를 캡처하는 람다 함수는 저장할 수 없다는 단점이 있었다. 대신 **C++ 11부터는 `<functional>`의 `std::function`을 통해서 쉽게 클로저 객체를 저장하고 접근**할 수 있다. `std::function`은 아래와 같이 사용한다.

```C++
std::function<return_type(parameters)> func = [introducer_capture] (parameters) -> return_type { statements; }
```
<br>

&nbsp;**람다 함수를 이용하여 재귀 함수를 작성**할 때도 `std::function`이 도움을 준다. `std::function`을 사용하지 않고 람다 함수를 재귀적인 형태로 만들 때는 **호출 시점에 해당 람다 함수 내에서 재귀 호출되는 함수가 어떤 형태인지를 파악할 수 없어서** 자기 자신을 인자로 넘겨주는 식의 형태로 구현했어야 했다.

```C++
auto func = [](auto self)
{
        // ...

        return self(self);
}

// 호출 시,
func(func);
```

&nbsp;하지만 `std::function`을 이용하면 람다 함수 정의 전에 함수의 형태를 미리 선언할 수 있기 때문에 더 유연한 람다 함수를 작성할 수 있다.

```C++
std::function<void(void)> func;
func = []
{
        // ...

        return func;
}
```
<br>

(작성중) - 스마트 포인터의 작동 원리, unique_ptr의 사용 방법, unique_ptr의 소유권 이전, make_unique(C++ 14) 사용하기, shared_ptr의 작동 원리, shared_ptr의 사용 방법, make_shared(C++ 14) 사용하기(생성자로 생성 시 단점 및 주의할 점/주소값으로 생성), shared_from_this <shared_ptr의 상호 참조부터 읽어보기>
