# Range-based for loop

&nbsp;`<algorithm>`에 정의되어 있다.

## std::for_each

&nbsp;`func`의 반환값은 무시된다. `InputIterator`를 역참조한 결과는 `Type`과 동일하거나 `Type`으로 변환 가능해야 한다.

```C++
// func은 아래와 같은 형태. 반드시 constant reference일 필요는 없음.
void func(const Type& arg);

template <typename InputIterator, typename Function>
Function std::for_each(InputIterator begin, InputIterator end, Function func);
```
<br>

## std::transform

&nbsp;**`std::for_each`는 container 원소의 내용에 변화를 주지 않으나, `std::transform`는 container 원소의 내용에 변화를 줄 수 있다**.

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

## 참고

### Functor

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
                // 필요한 작업
        }
private:
        T factor; // 필요한 경우
}
```

&nbsp;`class`대신 `struct`를 사용할 수도 있고, `class`로 구현할 경우 생성자와 괄호 연산자를 클래스 외부에서 정의할 수도 있다(짧은 함수라면 그냥 클래스 정의 내에서 함수 정의 추천). Functor 내 각 매개변수는 필요에 따라 레퍼런스 타입으로 선언한다.
