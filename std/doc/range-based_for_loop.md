# Range-based for loop

&nbsp;`<algorithm>`에 정의되어 있다. C++ 11부터는 for문 내에서 `:`를 이용한 가독성 좋은 범위 기반 루프도 가능하지만 `<algorithm>`의 `std::for_each`는 조금 더 다양하게 활용할 수 있기 때문에 상황에 따라 더 적절한 방법을 선택해서 사용하자.


## std::for_each

&nbsp;`func`의 반환값은 무시된다. `InputIterator`를 역참조한 결과는 `Type`과 동일하거나 `Type`으로 변환 가능해야 한다.

```C++
// func은 아래와 같은 형태. 반드시 constant reference일 필요는 없음.
void func(const Type& arg);

template <typename InputIterator, typename Function>
Function std::for_each(InputIterator begin, InputIterator end, Function func);
```
<br>

&nbsp;C++ 17부터는 놀랍게도 `std::for_each`를 이용하여 병렬 처리까지 가능하다! **첫번째 인자로 `std::excution::par`을 주면 해당 `std::for_each`는 병렬 실행**된다.


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
