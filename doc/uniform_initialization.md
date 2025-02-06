# Uniform Initialization

```C++
#include <iostream>

class A
{
public:
        A() { std::cout << "A 생성" << std::endl; };
}

int main(void)
{
        A a();

        return 0;
}
```

&nbsp;예상과는 달리 위의 코드를 실행하면 아무것도 출력되지 않는다. C++ 컴파일러는 함수의 정의처럼 보일 수 있는 것들은 모두 함수의 정의로 인식하기 때문인데, 위의 경우에는 `main` 함수 내의 생성자 호출이 `class A` 타입을 반환하고 매개변수가 없는 함수의 정의로 인식된 것이다. 심지어 `B b(A())`같은 경우에도 `class A`를 리턴하고 매개변수가 없는 함수를 인자로 받는 `class B`를 리턴하는 함수로 인식한다.


&nbsp;C++ 11에서는 균일한 초기화(Uniform Initialization)를 도입하여 이 문제를 해결했다. 간단하게 생성자에 `()` 대신 `{}`를 사용하면 된다. 매개변수가 있는 경우에도 `{}`에 값을 넘겨주면 되는데, `()`과 달리 `{}`는 데이터 손실이 발생하는 Narrowing 변환이 일어나지 못하도록 강제한다. 때문에 원치 않는 타입 캐스팅이 발생하는 경우도 `{}`를 통해 막을 수 있다.


&nbsp;함수의 반환문에서 생성자를 호출할 때, `{}`를 이용하면 객체의 타입을 굳이 명시하지 않아도 된다.

```C++
A test(void)
{
        // ...

        return { 1, 2, 3 };
}
```


## std::initializer_list&lt;T>

&nbsp;`std::vector<T>`를 초기화할 때 마치 배열을 초기화하듯이 `v = { 1, 2, 3 };`와 같이 `{}`를 이용하여 초기화할 수 있다. C++ 11부터는 이것을 사용자 정의 클래스의 객체 생성시에도 사용할 수 있다는 점인데, 아래와 같이 `std::initializer_list<T>`를 이용한다.

```C++
#include <iostream>
#include <vector>

class MyVector
{
public:
        MyVector(std::initializer_list<int> il)
        {
                for(int i: il)
                        vec.push_back(i);
        }

        void print(void)
        {
                for(int i: vec)
                        std::cout << i << std::endl;
        }
private:
        std::vector<int> vec;
}

int main(void)
{
        MyVector mv = { 1, 2, 3, 4 }; // mv{ 1, 2, 3 }도 가능

        mv.print();

        return 0;
}
```
<br>

&nbsp;주의할 점은, `std::initializer_list<T>`를 받는 생성자가 존재하는 경우 컴파일러는 `{}`로 생성 시 다른 생성자보다 `std::initializer_list<T>`로 우선 로딩하기 때문에 `std::initializer_list<T>`의 타입과 캐스팅이 가능한 생성자가 있으면 타입 캐스팅 오류가 발생한다. `std::initializer_list<T>`의 타입이 기존 생성자의 타입과 캐스팅이 불가능한 경우에는 자동으로 처리된다.


&nbsp;위의 코드처럼 `mv = { 1, 2, 3 }` 형태로 생성자를 호출할 때, 타입을 `auto`로 설정하면 `auto`는 `std::initializer_list<T>` 타입으로 추론된다.


&nbsp;`{}`를 이용한 균일한 초기화 표현식은 현대적인 C++ 코드에서 점점 더 많이 사용되고 있다. 기존 코드와의 일관성이 중요한 경우가 아니라면, 적극적으로 사용해보는 것이 좋을 것이다.
