# C++ Rvalue Reference & Move Semantics

*Thomas Becker(현재 원문 확인 불가) 님이 작성하신 글을 번역한 모두의 코드[[1]](#references) 님의 글을 바탕으로 공부한 내용을 정리한 글입니다.*


## Rvalue Reference & Move Semantics

&nbsp;C++로 우리만의 벡터 `MyVector`를 구현한다고 가정하자. `MyVector`는 아래와 같이 원소들을 동적 메모리 상에 저장하고 해당 위치를 포인터로 소유하고 있을 것이다.

```C++
template <typename T>
class MyVector
{
public:
	// ...
private:
	// ...
	T* element;
}
```
<br>

&nbsp;이제 `MyVector`에 깊은 복사를 수행하는 대입 연산자를 추가한다. 대입 연산자는 아래와 같은 형태를 가지고 있을 것이다.

```C++
template <typename T>
MyVector<T>& MyVector<T>::operator =(const MyVector<T>& new_vec)
{
	// ...

	// this의 element를 해제
	// this의 element에 새 동적 메모리 공간 대입
	// new_vec의 element가 가리키는 내용들을 this의 element가 가리키는 위치로 복사

	// ...

	return *this;
}
```
<br>

&nbsp;`MyVector` 객체인 `v`와 `u` 사이의 대입 연산 `v = u`는 우리의 의도대로 깊은 복사로 작동할 것이다. 그렇다면 아래와 같이 함수의 반환값을 대입하는 연산을 생각해보자.

```C++
MyVector<int> foo();

MyVector<int> v;

v = foo();
```
<br>

&nbsp;`foo()`의 반환으로 생성되는 `MyVector` 객체는 임시 객체이기 때문에 `v = foo()`가 수행되고나서 자동으로 사라질 것이다.


&nbsp;그런데 생각해보면, 곧 사라질 임시 객체의 내용을 복사해오기 위해 대입 연산자에서 수행하는 복사가 비효율적이지 않은가? 그냥 `this`의 `element`가 가리키는 위치를 해제하고 임시 객체의 `element`를 대입하면 훨씬 효율적이지 않은가? 알다시피 **메모리를 할당하고 해제하는 작업은 프로그래밍에서 매우 비용이 높은 작업**이다. `MyVector`와 같이 깊은 복사를 수행해야하는 작업에서는 그 비용 차이가 더 두드러질 것이다.


&nbsp;더 효율적인 대입 연산을 위해 **대입 연산자를 오버로딩**해보자. 우리는 일반적인 `MyVector` 객체를 대입하는 경우와 `foo()`의 반환값과 같은 임시 객체를 대입하는 경우를 구분해야 한다. 이 둘은 **Lvalue**를 대입하는 경우와 **Rvalue**를 대입하는 경우로 구분할 수 있다. `v`와 `u`는 Lvalue이고, `foo()`의 반환으로 생성되는 임시 객체는 Pure Rvalue다(Lvalues, Rvalues에 대한 내용은 여기서는 생략한다).


&nbsp;이 때 사용되는 것이 **Rvalue Reference**(우측값 참조)다. C++ 11에서 추가된 문법으로, `MyVector`의 Rvalue Reference는 `MyVector&&`로 정의된다. **Lvalue는 Lvalue Reference 매개 변수로, Rvalue는 Rvalue Reference 매개 변수로 로드되는 것을 선호**하기 때문에, 우리는 함수 오버로딩을 이용하여 Rvalue 매개 변수를 더 효율적으로 대입할 수 있다.


&nbsp;Rvalue Reference를 이용하여 대입 연산자를 오버로드한 함수는 아래와 같다.

```C++
template <typename T>
MyVector<T>& MyVector<T>::operator =(MyVector<T>&& new_vec)
{
	// ...

	T* temp = element;
	this->element = new_vec.element;
	new_vec.element = temp;

	// ...

	return *this;
}
```
&nbsp;소멸자에서 element를 적절하게 해제하도록 구현했다면 대입 연산 후 임시 객체가 소멸할 때 임시 객체의 element가 가리키는 공간 또한 적절하게 해제될 것이므로, 위처럼 단순히 element에 swap 연산을 통해 간단하고 효율적으로 구현할 수 있다.


&nbsp;이러한 연산을 **Move Semantics**(이동 문법/연산)라 부른다. C++ 11 이전에도 TMP를 통해 이동 연산을 구현할 수 있었으나, Rvalue Reference를 통해 함수 오버로딩으로 구현할 수 있게 되었다[[1]](#references).


&nbsp;Rvalue는 일반적인 (Lvalue) Reference를 이용하는 대입 연산으로 로드될 수 있지만, Lvalue는 Rvalue Reference를 이용하는 이동 대입 연산으로 로드될 수 없기 때문에, 이동 대입 연산자를 구현하지는 않더라도 대입 연산자 자체는 필요한 경우 반드시 구현하여야 한다.


## std::move

&nbsp;효율적인 **이동 연산을 Lvalue에서도 이용**할 수 있을까? 만약 Lvalue인 `source`와 `target`에서 `target = source`를 수행한 후 source가 더 이상 사용되지 않는다면, 임시 객체에서의 경우와 같이 이동 연산으로 처리하는 것이 더 효율적일 것이다. C++에서는 이러한 경우를 위해 `std::move` 함수를 제공한다. `std::move`는 아래와 같이 사용한다.

```C++
target = std::move(source);
```
<br>

&nbsp;`std::move`는 Lvalue를 Rvalue처럼 취급하도록(이동 연산이 가능하도록) 표시하는 역할을 한다. 위와 같은 경우, `source`, `target`의 클래스에 이동 연산이 정의되어 있지 않다면 기존의 (Lvalue) Reference를 이용한 대입 연산을 수행하겠지만, 이동 대입 연산이 정의되어 있다면 이동 연산을 통해 효율적으로 대입을 수행할 수 있다. 참고로, `std::move`를 이용하거나 임시 객체를 대입하는 경우와 같이 **자신의 내부 내용에 대한 소유권을 포기한 상태의 값을 개념적으로 Xvalue**라고 부른다. Xvalue 또한 큰 범주 상에서 Rvalue다.


&nbsp;`std::move`는 사실 `static_cast<typename std::remove_reference<T>::type&&>(t)`를 반환하는 간단한 함수이고, 실제로 `std::move`대신 `static_cast<T&&>`를 써도 (안정성은 좀 떨어질지언정) Rvalue Reference로 변환이 가능하다. 하지만 `std::move`를 사용하는 것이 더 안정적이고 가독성도 좋기 때문에 `std::move` 사용을 권장한다[[1]](#references).


&nbsp;Move Semantics 단에서 살펴본 바와 같이 이동 연산은 일반적으로 swap 연산을 통해 구현되는데, 이 때 주의해야할 것은 `source`가 스코프를 벗어나기 전까지 내용이 유지되기 때문에 **사용자가 언제 객체가 소멸될지를 쉽게 알아차리기 어렵다**는 것이다. 때문에 임시 객체를 대입하는 경우와 같이 **이동 연산 후 즉시 객체가 소멸되기를 기대하는 연산(쓰레드 락 해제 등)들은 반드시 이동 연산 함수 내부에서 수행**되어야 한다[[1]](#references).


&nbsp;이동 연산과 `std::move`를 이용하는 것은 많은 표준 알고리즘의 비약적인 속도 향상을 가져온다. 특히 swap 연산이 빈번하게 일어나는 정렬 등의 알고리즘에서 그 속도 향상이 크다. 기존의 Standard Libraries의 알고리즘들은 타입의 복사 가능성(Copyablility)를 요구하였으나, 조사 결과 이동 가능성(moveability)으로도 충분함을 밝혀냈으며, 복사는 불가능하나 이동만 가능한 타입에 대해서도 Standard Libraries를 이용할 수 있게 되었다[[1]](#references).


## Rvalue Reference는 Rvalue인가?

&nbsp;주의해야할 점이 하나 더 있다. Move Semantics 단에서는 Rvalue Reference를 Lvalue와 Rvalue의 로딩을 구분하는 용도로 사용했었는데, 실제로 해당 값을 사용하려 할 때 **Rvalue Reference는 Rvalue로 취급되는가?** 아래의 예를 살펴보자.

```C++
template <typename T>
void bar(MyVector<T>&& new_vec)
{
	MyVector<T> temp = new_vec;

	// ...
}
```
<br>

&nbsp;Rvalue Reference로 받은 값이니 `new_vec` 또한 Rvalue일 것이라고 지레짐작할 수 있지만, 예상과는 달리 실제로 `new_vec`은 위의 상황에서 Lvalue다. **Rvalue Reference 또한 상황에 따라 Lvalue, Rvalue로 취급될 수 있다**. 이를 구분하는 것은 대입 연산자가 호출될 지, 이동 대입 연산자가 호출될 지를 결정하기 때문에 반드시 생각해보아야 하는 내용인데, 어떤 상황에서 Rvalue Reference가 Lvalue가 될 지, Rvalue가 될 지를 구분하는 가장 쉬운 방법은 **이름**이 있는지를 확인하는 것이다.


&nbsp;**이름이 있다**는 뜻은 '**변수나 레퍼런스를 통해 접근할 수 있다**', 즉 '**메모리 상의 위치를 파악할 수 있다**'는 뜻으로 이해해도 좋다. 이름이 없는 객체는 임시 객체와 같은 경우로, 변수나 레퍼런스 등으로 메모리 상 위치를 파악할 수 없기 때문에 임의로 접근할 수가 없다. 참고로 **함수의 반환값이더라도 static 변수를 레퍼런스로 반환하는 함수의 반환값은 메모리 상 위치를 파악할 수 있으므로 Lvalue**다!


&nbsp;위 코드의 경우에는 `new_vec`이 Rvlaue Reference를 통해서 넘어온 Xvalue이긴 하지만, 적어도 함수 내에서는 `new_vec`이라는 엄연한 이름을 가지고 메모리 상 위치에 접근할 수 있는 값이기 때문에 Lvalue로 취급할 수 있는 것이다. 함수 내에서 Rvalue로 취급하기 위해서는 Lvalue를 Xvalue로 만들기 위해 `std::move`를 사용해야 한다.

```C++
MyVector<T> temp = std::move(new_vec);
```
<br>

&nbsp;상속 과정에서 **이동 복사 생성자**를 작성하는 경우에도 동일하다. 부모 클래스의 이동 생성자를 올바르게 호출하려면 아래와 같이 Xvalue로 인자를 넘겨주어야 한다.

```C++
template <typename T>
MyNewVector<T>::MyNewVector(MyNewVector<T>&& new_vec) : MyVector<T>(std::move(new_vec))
{
	// ...
}
```
<br>

&nbsp;이런 식으로 구현되어 있는 이유는 **값이 자신의 소유권을 확실히 포기한 경우에만 이동 연산을 허용하기 위함**이다. 개발자가 이동 연산이 수행될 것이라 예측하지도 못한 상황에서 이동이 완료된 값을 다시 사용하려하면 프로그래밍 상 재앙적인 상황이 발생할 수 있기 때문에 일종의 안전장치를 걸어둔 셈이다[[1]](#references).


## noexcept

&nbsp;`noexcept`는 해당 함수가 예외를 발생시키지 않는다는 것을 컴파일러에게 명시적으로 알려주는 키워드다.


&nbsp;**이동 연산을 구현하는 오버로드 함수는 예외를 발생시키지 않도록 정의하고 이를 `noexcept` 키워드를 활용하여 이를 컴파일러에게 명시**하는 것이 좋은데, 왜냐하면 **컴파일러는 연산의 효율 상승보다 예외에 대한 안정성을 확보하는 것을 더 중요하게 여기기 때문**이다. 실제로 `noexcept` 키워드 없이 정의한 오버로드 함수는 때때로 예상과 달리 이동 연산이 수행되지 않는 경우를 볼 수 있다. 특히 Standard Libraries의 `std::vector`는 오버로드 함수가 `noexcept`로 선언되어 있지 않으면 이동 연산이 수행되지 않는다[[1]](#references).


## References
[1] 씹어먹는 C++ 토막글 ① - Rvalue(우측값) 레퍼런스에 관해, 모두의 코드([https://modoocode.com/189](https://modoocode.com/189))
