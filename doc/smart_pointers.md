# Smart Pointers

&nbsp;C++ 이후의 언어들은 대부분 GC를 탑재한 채로 출시되어 더 이상 사용하지 않는 메모리 공간을 쉽게 해제시켜주고 프로그래머들의 책임을 크게 덜어주었는데, C / C++은 그런 기능이 없었다. 모던 C++에서는 **스마트 포인터**의 등장으로 프로그래머의 부담이 크게 줄었다.


&nbsp;스마트 포인터는 일종의 **래퍼(wrapper) 클래스**로, 동적으로 할당받는 메모리를 래퍼 클래스가 관리하면서, 해당 래퍼 클래스의 소멸자가 호출될 때 자동으로 자신의 포인터가 가리키고 있는 (동적으로 할당받은) 메모리 공간을 동시에 해제시켜줌으로써 메모리 힙 공간을 더 편하게 관리할 수 있도록 한다. 이러한 디자인 패턴을 **Resource Acquisition Is Initialization - RAII**라고 부른다.


## std::unique_ptr

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


&nbsp;`std::unique_ptr`은 복사는 불가능하지만 **소유권을 완전히 이전하는 이동 연산은 가능**하다. 위의 `mkdorito`에서 `std::unique_ptr<dorito> p_o = std::move(p_d)`와 같이 작성하면 `p_o`는 `p_d`가 가지던 힙 공간의 소유권을 넘겨받고, `p_d`는 `nullptr`을 가지게 된다.


&nbsp;`std::unique_ptr`의 복사가 불가능하다는 점 때문에 일반 포인터를 사용할 때보다 함수의 인자 등으로 넘겨줄 때 조금 귀찮아지는데, 이 때 선택 가능한 방법은 `std::unique_ptr`을 가리키는 포인터를 넘겨주거나(C Style), `std::unique_ptr`의 레퍼런스를 넘겨줄 수 있다. 두 방법 모두 소유권을 유일하게 가진다는 원칙을 위배할 가능성이 있기에 아예 가리키는 동적 메모리 위치 자체를 넘겨주는 방법도 있다. `std::unique_ptr`의 `get` 함수를 이용하면 해당 스마트 포인터가 가진 동적 메모리 위치를 얻어올 수 있다.


&nbsp;C++ 14부터는 **`std::unique_ptr`을 간단하게 생성할 수 있는 `std::make_unique`를 이용**할 수 있다. 그냥 생성자를 이용하면 동적 메모리 공간을 `new`를 통해 할당받고 스마트 포인터에 저장하는 작업을 거치지만, **`std::make_unique`는 사용할 생성자의 인자만 받아서 Perfect Forwarding을 통해 바로 스마트 포인터에 공간을 할당**할 수 있다. 이 장점은 아래의 `std::make_shared`에서 더 효율적인 장점을 가지게 된다. `std::make_unique`는 다음과 같이 사용한다.

```C++
auto p_d = std::make_unique<dorito>(8);
```
<br>

## std::shared_ptr

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

## std::weak_ptr

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


## 참고

### delete

&nbsp;`delete`는 동적 메모리의 해제 뿐 아니라 C++ 11부터는 원치 않는 함수를 의도적으로 삭제하는데에도 사용된다. 컴파일러는 가끔 우리가 의도하지 않는 생성자 등을 (시키지도 않았는데!) 암시적으로 생성해서 사용하는 경우가 있는데, 이러한 경우를 의도적으로 막고자 한다면 `dorito(const int& n) = delete`와 같이 `delete`를 이용하여 삭제된 함수임을 컴파일러에게 명시할 수 있다.
