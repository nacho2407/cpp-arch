# C++ Note

&nbsp;C++ 개발하면서 참고할 만한 내용 모음(주로 모던 C++ 내용). 말이 모던 C++이지 이 내용들도 이미 10년이 넘은 기능들이다. Boost라는 C++ 라이브러리에서 넘어온 기능들이 많은데, Boost 개발자들 중 많은 사람들이 C++ 표준 위원회에도 속해있어서 C++ 표준 제정 시 Boost에서 넘어온 기능들이 꽤 많다고 한다.


## 클래스 정의 시 고려해야 할 것들

- 기본 생성자
- 복사 생성자(동적 할당 고려)
- 이동 생성자
- 대입 연산자(동적 할당 고려)
- 이동 대입 연산자
- 소멸자


## Range Based for Statement

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


## Lambda Expressions & Closure

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

## Implicit Conversions

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

## Smart Pointers

&nbsp;C++ 이후의 언어들은 대부분 GC를 탑재한 채로 출시되어 더 이상 사용하지 않는 메모리 공간을 쉽게 해재시켜주고 프로그래머들의 책임을 크게 덜어주었는데, C/C++은 그런 기능이 없었다. 모던 C++에서는 **스마트 포인터**의 등장으로 프로그래머의 부담이 크게 줄었다.


&nbsp;스마트 포인터는 일종의 **래퍼(wrapper) 클래스**로, 동적으로 할당받는 메모리를 래퍼 클래스가 관리하면서, 해당 래퍼 클래스의 소멸자가 호출될 때 자동으로 자신의 포인터가 가리키고 있는 (동적으로 할당받은) 메모리 공간을 동시에 해제시켜줌으로써 메모리 힙 공간을 더 편하게 관리할 수 있도록 한다. 이러한 디자인 패턴을 **Resource Acquisition Is Initialization - RAII**라고 부른다.


&nbsp;**스마트 포인터는 크게 `<memory>`의 `std::unique_ptr`과 `std::shared_ptr`로 나눌 수 있다**. 포인터가 소멸되더라도 해당 포인터가 가리키던 힙 공간을 다른 포인터가 사용하고 있어 해당 공간을 해제하면 안되는 경우가 있는데, 이런 경우를 고려하여 해당 힙 공간을 유일하게 소유할 수 있는 `std::unique_ptr`과, 가리키고 있는 포인터의 개수를 기억하여 해제하는 `std::shared_ptr`로 나뉜 것이다. `std::unique_ptr`은 다음과 같이 사용한다.

```C++
class dorito
{
public:
        dorito();

        dorito(int n);

        // ...

        ~dorito();
private:
        int dodohan;
}

// ...

void mkdorito(void)
{
        std::unique_ptr<dorito> p_d(new dorito(8));

        std::cout << "Smells Good!" << std::endl;
}
```
<br>

&nbsp;`std::unique_ptr`은 **해당 힙 공간의 유일한 소유권을 가지기 때문**에 위의 `mkdorito`에서 `std::unique_ptr<dorito> p_o = p_d`와 같이 **동일한 힙 위치를 가리키도록 만들 수 없다**. C++ Standard Libraries에서는 이러한 경우를 막기 위해 `std::unique_ptr`의 복사 생성자 / 할당 연산자를 의도적으로 `delete` 처리해두었다.


&nbsp;참고로 `delete`는 동적 메모리의 해제 뿐 아니라 C++ 11부터는 원치 않는 함수를 의도적으로 삭제하는데에도 사용된다. 컴파일러는 가끔 우리가 의도하지 않는 생성자 등을 (시키지도 않았는데!) 암시적으로 생성해서 사용하는 경우가 있는데, 이러한 경우를 의도적으로 막고자 한다면 `dorito(const int& n) = delete`와 같이 `delete`를 이용하여 삭제된 함수임을 컴파일러에게 명시할 수 있다.


&nbsp;`std::unique_ptr`은 복사는 불가능하지만 **소유권을 완전히 이전하는 이동 연산은 가능**하다. 위의 `mkdorito`에서 `std::unique_ptr<dorito> p_o = std::move(p_d)`와 같이 작성하면 `p_o`는 `p_d`가 가지던 힙 공간의 소유권을 넘겨받고, `p_d`는 `nullptr`을 가지게 된다.


&nbsp;`std::unique_ptr`의 복사가 불가능하다는 점 때문에 일반 포인터를 사용할 때보다 함수의 인자 등으로 넘겨줄 때 조금 귀찮아지는데, 이 때 선택 가능한 방법은 `std::unique_ptr`을 가리키는 포인터를 넘겨주거나(C Style), `std::unique_ptr`의 레퍼런스를 넘겨줄 수 있다. 두 방법 모두 소유권을 유일하게 가진다는 원칙을 위배할 가능성이 있기에 아예 가리키는 동적 메모리 위치 자체를 넘겨주는 방법도 있다. `std::unique_ptr`의 `get` 함수를 이용하면 해당 스마트 포인터가 가진 동적 메모리 위치를 얻어올 수 있다.


&nbsp;C++ 14부터는 **`std::unique_ptr`을 간단하게 생성할 수 있는 `std::make_unique`를 이용**할 수 있다. 그냥 생성자를 이용하면 동적 메모리 공간을 `new`를 통해 할당받고 스마트 포인터에 저장하는 작업을 거치지만, **`std::make_unique`는 사용할 생성자의 인자만 받아서 Perfect Forwarding을 통해 바로 스마트 포인터에 공간을 할당**할 수 있다. 이 장점은 아래의 `std::make_shared`에서 더 효율적인 장점을 가지게 된다. `std::make_unique`는 다음과 같이 사용한다.

```C++
auto p_d = std::make_unique<dorito>(8);
```
<br>

&nbsp;`std::shared_ptr`는 `std::unique_ptr`과 달리 **하나의 힙 공간을 여러 포인터가 가리킬 수 있다**. **메모리의 해제 시점은 가리키는 포인터가 모두 해제된 경우**인데, 이를 추적하기 위하여 `std::shared_ptr`는 동적으로 메모리를 할당받을 때 **해당 힙 공간을 가리키는 레퍼런스 개수를 기억하는 제어 블록을 동시에 동적 할당** 받는다. `std::shared_ptr`는 소멸자가 호출될 때 제어 블록의 레퍼런스 개수가 0일 때만 힙 공간도 함께 해제한다. `std::shared_ptr`는 다음과 같이 사용한다.

```C++
class dorito
{
public:
        dorito();

        dorito(int n);

        // ...

        ~dorito();
private:
        int dodohan;
}

void mkdorito(void)
{
        std::shared_ptr<dorito> p_d(new dorito(8));
        std::shared_ptr<dorito> p_o(p_d);

        std::cout << "I Shared My Doritos!" << std::endl;
}
```
<br>

&nbsp;C++ 14부터는 생성자 대신 상기한 것처럼 **`std::make_shared`를 이용하여 `std::shared_ptr`를 더 간편하고 안전하게 생성**할 수 있다. `new`를 이용하여 `std::shared_ptr`를 생성하면 힙 공간을 할당 받으면서 제어 블록까지 공간을 받기 위해 동적 할당을 두 번 수행해야 하지만, **`std::make_shared`를 이용하면 Perfect Forwarding을 이용해 제어 블록을 합친, 필요한 크기를 한 번에 할당받아 `std::shared_ptr`를 생성하기 때문에 더 효율적이고 안전**하다. `std::make_shared`는 아래와 같이 사용한다.

```C++
auto p_d = std::make_shared<dorito>(8);
```
<br> 

&nbsp;참고로 `std::shared_ptr`를 생성자를 이용해 생성할 때 주의할 점이 있는데, **`std::shared_ptr`는 주소값을 이용해서 생성할 때 해당 힙 공간을 첫 번째로 소유하는 포인터처럼 제어 블록을 생성하게 된다는 것**이다. 그렇게 생성되는 제어 블록은 레퍼런스 개수가 1개인 것으로 생각하기 때문에 의도치 않게 힙 공간이 해제될 수도 있다. 때문에 아래와 같이 생성하는 것은 올바른 방법이 아니다.

```C++
dorito* d = new dorito(8);
std::shared_ptr<dorito> p_d(d);
std::shared_ptr<dorito> p_o(d); // d에 대한 제어 블록을 각각 가짐
```

&nbsp;이 문제는 `std::make_shared`를 이용하더라도 한 번은 짚고 넘어가야하는 부분인데, 왜냐하면 **클래스 내부에서 자신의 `std::shared_ptr`를 반환하는 함수를 구현**하려 할 때, `return std::shared_ptr<dorito>(this)`와 같이 구현하면 의도치 않게 해당 힙 공간이 해제되어버릴 수도 있기 때문이다.


&nbsp;클래스 내부에서 **자기 자신의 `std::shared_ptr`를 안전하게 전달하기 위한 방법으로 `std::enable_shared_from_this`를 이용하는 방법**이 있다. 아래와 같이 사용한다.

```C++
class dorito: public std::enable_shared_from_this<dorito>
{
public:
        dorito();

        dorito(int n);

        // ...

        std::shared_ptr<dorito> get_my_dorito();

        ~dorito();
private:
        int dodohan;
}

// ...

std::shared_ptr<dorito> dorito::get_my_dorito()
{
        return shared_from_this(); // 자신의 std::shared_ptr를 반환
}
```

&nbsp;단, `shared_from_this`는 자신의 힙 공간을 가리키는 제어 블록(`std::shared_ptr`)이 미리 정의되어 있어야 정상 작동하기 때문에 아래와 같이 그냥 사용할 수는 없다.

```C++
dorito* d = new dorito(8);
std::shared_ptr<dorito> p_d = d->get_my_dorito(); // 에러 발생
```
<br>

&nbsp;`std::shared_ptr`는 **상호 참조 시에 메모리 해제가 불가능하다는 단점**이 있다. `a`라는 객체의 멤버 `std::shared_ptr`가 `b`라는 객체를 가리키고, 동시에 `b` 객체의 멤버 `std::shared_ptr`가 `a` 객체를 가리키게 되면 `a`와 `b`의 멤버가 서로를 해제할 수 없게 되는 것이다. **이 문제를 해결하기 위해 `std::weak_ptr`가 존재**한다. `std::weak_ptr`는 `std::shared_ptr`와 일반 포인터의 중간 쯤 성격으로, `std::shared_ptr`와 **동일한 힙 공간을 가리키되, 제어 블록의 레퍼런스 개수를 증가시키지는 않는다**. 때문에 `std::weak_ptr`이 가리키고 있더라도 해당 힙 공간을 가리키는 `std::shared_ptr`가 모두 소멸되었다면 해당 힙 공간은 이미 해제된 상태다. 자신을 가리키는 또 다른 객체가 존재할 수도 있는 상황에서 아래와 같이 사용한다.

```C++
class dorito
{
public:
        dorito();

        dorito(int n);

        // ...

        void set_parent_dorito(std::weak_ptr<dorito> p_d);
        std::shared_ptr<dorito> get_parent_dorito();

        ~dorito();
private:
        int dodohan;
        std::shared_ptr<dorito> child_dorito;
        std::weak_ptr<dorito> parent_dorito;
}

// ...

void dorito::set_parent_dorito(std::weak_ptr<dorito> p_d)
{
        parent_dorito = p_d;
}

std::shared_ptr<dorito> dorito::get_parent_dorito()
{
        std::shared_ptr<dorito> p = parent_dorito.lock();

        if(p)
                return p;
        else
                std::cout << "Already Deleted!" << std::endl;
}
```

&nbsp;`class dorito`를 Node처럼 Tree를 구성한다고 가정하면, 위의 코드처럼 자식 Node는 `std::shared_ptr`로 가지고, 부모 Node는 `std::weak_ptr`로 가지게 할 수 있다. 둘 모두 `std::shared_ptr`로 정의하면 상호 참조 관계가 되어 메모리에서 해제가 불가능해진다.


&nbsp;`std::weak_ptr`는 **그 자체로 힙 공간에 접근할 수는 없다**. 위의 코드처럼 `std::weak_ptr`의 **`lock`함수를 이용하여 `std::shared_ptr`로 변경시킨 다음 힙 공간에 접근**할 수 있는데, 이 때 `std::weak_ptr`이 남아 있더라도 원본 `std::shared_ptr`이 해제되어 힙 공간도 해제된 상태일 수 있으므로 안전하게 이용하려면 `get_parent_dorito` 함수처럼 **해당 공간이 `nullptr`가 아닌지 확인하여 접근**해야 한다. **`lock` 함수는 원본 `std::shared_ptr`의 제어 블록을 참고하여 레퍼런스 개수가 0이면 `nullptr`을 가지는 `std::shared_ptr`를 반환**한다.


&nbsp;참고로 `std::weak_ptr`이 아직 존재하지만 원본 `std::shared_ptr`이 해제되어 힙 공간이 해제된 상태라면, 제어 블록 또한 해제되어 `lock` 함수가 무엇을 반환해야하는지 모르게 될 수 있는데(제어 블록의 메모리가 다른 내용으로 덮어써진 상태일 수 있음), 이런 경우를 방지하기 위하여 **제어 블록은 레퍼런스 개수 외에도 약한 레퍼런스 개수(weak reference)도 함께 가진다**. 자신을 가리키는 `std::weak_ptr`의 개수도 0이 될 때 제어 블록이 해제된다.


## Variadic Templates

&nbsp;C에서는 `<stdarg.h>`와 파라미터 팩(`...`)을 이용하여 가변 길이 인자를 사용할 수 있었다. C++에서도 C-Style을 동일하게 사용할 수 있으나, C++ 11 이후로는 **가변 길이 템플릿과 파라미터 팩을 이용하여 타입에 더 강한 가변 길이 인자를 구현할 수 있다**. 가변 길이 템플릿은 아래와 같이 사용한다.

```C++
template <typename T>
void print(T arg)
{
        std::cout << arg << std::endl;
}

template <typename T, typename... Types>
void print(T arg, Types... args)
{
        std::cout << arg << std::endl;

        print(args);
}
```
<br>

&nbsp;**가변 길이 템플릿은 일반적으로 위 코드의 하나의 인자를 가진 `print`처럼 베이스 케이스 함수와 함께 사용한다**. 파라미터 팩이 없는 함수가 더 우선 순위가 높기 때문에 오버로드된다. 또한 C++은 현재 위치의 앞에 있는 내용만 정의되기 때문에 **베이스 템플릿 함수는 가변 길이 템플릿 함수보다 앞서 정의되어야 한다**. 아래에서 볼 Fold 표현을 이용하여 더 간단하게 구현할 수도 있다.

### sizeof...

&nbsp;`sizeof` 연산자는 인자의 크기를 반환한다. **파라미터 팩에 `sizeof...` 연산자를 사용하면 해당 파라미터 팩이 포함하고 있는 인자의 개수를 반환한다**.

### Fold Expressions

&nbsp;가변 길이 템플릿은 일반적으로 재귀적인 형태로 이용하는데, C++ 17 이후로는 **Fold 표현을 이용하여 더 간단하게 구현할 수 있다**. Fold 표현은 아래와 같이 사용한다. **Fold 표현은 반드시 `()` 안에서 사용해야 한다**.

```C++
template <typename... Types>
int sum(Types... arg)
{
        return (arg + ...);
}
```

&nbsp;위 코드는 입력이 `a, b, c`일 때 `((a + b) + c)`와 같이 동작한다. 파라미터 팩의 위치에 따라 연산되는 순서가 다르다. 가능한 형태는 다음과 같다.

- 단항 좌측: `... op E` => `(((e[1] op e[2]) op e[3]) op ...)`
- 단항 우측: `E op ...` => `(... op (e[n - 2] op (e[n - 1] op e[n])))`
- 이항 좌측: `I op ... op E` => `(((I op e[1]) op e[2]) op ...)`
- 이항 우측: `E op ... op I` => `(... op (e[n - 1] op (e[n] op I)))`


&nbsp;`,`를 통해 각 인자에 대한 작업을 각각 수행할 수도 있다. 아래와 같이 사용한다.

```C++
template <typename... Types>
void func(Types... arg)
{
        (new_func(arg), ...);
}
```

&nbsp;위와 같은 코드는 범위 기반 for-loop처럼 모든 인자에 대해 `new_func`를 수행한다.


## Standard Libraries - &lt;algorithm>

### Sorting

- `std::sort`

&nbsp;기본 정렬 알고리즘. 퀵 정렬, 힙 정렬, 삽입 정렬을 복합적으로 사용하는 인트로 정렬을 이용하여 구현되었다. `RandomAccessIterator` 타입을 만족하는 반복자를 반환하는 자료구조에 사용할 수 있다. 기본적으로 오름차순 정렬을 수행한다.

```C++
std::vector<int> vec;

vec.push_back(5);
vec.push_back(1);
vec.push_back(3);
vec.push_back(4);
vec.push_back(2);

std::sort(vec.begin(), vec.end());
```
<br>

&nbsp;정렬 순서를 변경하고 싶다면 세 번째 인자로 Functor를 넘겨주거나 아래와 같이 람다 함수를 넘겨주면 된다.

```C++
std::vector<int> vec;

vec.push_back(5);
vec.push_back(1);
vec.push_back(3);
vec.push_back(4);
vec.push_back(2);

std::sort(vec.begin(), vec.end(), [](int first, int second) {
        return first > second; // int 내림차순 정렬
});
```

&nbsp;`<functional>`의 `std::greater<int>` 등을 활용하여 더 간단하게 구현할 수도 있다. `std::greater<T>`, `std::less<T>` 등은 `T`에 `operator <`가 정의되어 있어야 한다.


- `std::partial_sort`

&nbsp;정렬하고자 하는 컨테이너의 일부분만 정렬하는 알고리즘. `std::sort`와 달리 start, middle, end 세 개의 인자가 필요하다. start부터 middle 전 까지의 원소들이 정렬되며, middle부터 end 전까지의 원소들도 의도치 않게 순서가 뒤바뀔 수 있기 때문에 정말 [start, middle) 사이의 원소만 정렬하고 나머지의 순서는 중요하지 않을 때만 사용하는 것이 좋다. 이런 경우에는 `std::sort`를 사용하는 것보다 시간 효율이 좋다.

```C++
std::vector<int> vec;

vec.push_back(5);
vec.push_back(1);
vec.push_back(3);
vec.push_back(4);
vec.push_back(2);

std::partial_sort(vec.begin(), vec.begin() + 3, vec.end());
```
<br>

&nbsp;정렬 순서를 바꾸고 싶을 때는 `std::sort`처럼 마지막 인자로 비교 함수 객체(람다 함수)를 넘겨준다.


- `std::stable_sort`

&nbsp;정렬 알고리즘에서 동일한 수준의 값들은 정렬 후에도 기존의 순서를 유지하는 것을 안정성(Stability)라 하는데, `std::sort`는 안정성을 만족하지 않는 퀵 정렬과 힙 정렬 등으로 구성되어 있다. 만약 안정성이 반드시 필요한 경우(pair 정렬 등)에는 `std::stable_sort`를 이용하여 정렬할 수 있다. `std::stable_sort`는 안정성을 만족하는 합병 정렬 및 삽입 정렬로 구현되었다. 사용법은 `std::sort`와 동일하다.

```C++
std::vector<std::pair<int, char>> vec;

vec.push_back(std::make_pair(5, 'a'));
vec.push_back(std::make_pair(1, 'b'));
vec.push_back(std::make_pair(3, 'c'));
vec.push_back(std::make_pair(5, 'd'));
vec.push_back(std::make_pair(2, 'e'));

std::stable_sort(vec.begin(), vec.end());
```
<br>

### Container Control - Erasing, Finding

- `std::remove`, `std::remove_if`

&nbsp;정수 벡터 내에서 3이라는 숫자를 모두 제거하고 싶을 때 반복자와 `erase` 함수를 이용하여 구현할 수도 있지만, `<vector>`와 같이 `RandomAccessIterator`를 이용하는 컨테이너들은 원소가 삭제되면 현재 반복자를 계속 사용할 수 없기 때문에 계속 반복자를 다시 생성해야하는 비효율적인 면이 있다. (작성중)
