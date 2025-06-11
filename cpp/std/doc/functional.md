# Functional

## std::function<F>

&nbsp;템플릿의 타입 등으로 함수 타입을 사용하려할 때 그냥 함수 포인터를 사용할 수도 있지만, 그런 경우에는 일반 함수만 사용할 수 있고 람다 함수 등 다른 Callable 객체를 받을 수 없다는 단점이 있다. `<functional>`의 `std::function<F>`(함수 객체)는 람다 함수, Functor 등 모든 Callable 객체를 받을 수 있는 래퍼 클래스로, 아래와 같이 사용한다.

```C++
#include <functional>
#include <iostream>

void hello()
{
        std::cout << "Hello, world!" << std::endl;
}

int main(void)
{
        std::function<void(void)> fn = hello;

        fn();

        return 0;
}
```
<br>

&nbsp;`std::function<F>`는 클래스의 멤버 함수를 저장할 때 문제점이 있는데, 멤버 함수를 저장한 `std::function<F>` 객체를 호출하려고 하면 `std::function<T>` 입장에서는 자신이 저장한 함수가 어떤 객체의 멤버인지 알 수 없기 때문이다. C++에서는 **멤버 함수의 구현 상 암묵적으로 함수의 인자로 자신의 객체를 넘겨주도록 구현**되어있다. 때문에 멤버 함수를 저장하기 위해서는 아래와 같이 함수의 **첫번째 매개변수로 자신의 객체 타입을 전달**하면 된다.

```C++
#include <functional>
#include <iostream>

class Test
{
public:
        Test(int i): i_mem(i) { }

        int mem(void);

        int c_mem(void) const;
private:
        int i_mem;
};

int Test::mem(void)
{
        return i_mem++;
}

int Test::c_mem(void) const
{
        return i_mem;
}

int main(void)
{
        Test t(8);

        std::function<int(Test&)> fn = &Test::mem;
        std::function<int(const Test&)> c_fn = &Test::c_mem;

        std::cout << fn(t) << '\n' << c_fn(t) << std::endl;

        return 0;
}
```

&nbsp;멤버 함수를 객체로 전달할 때 `&` 연산자를 사용한 이유는 C++ 문법 상 `::`를 통해서 접근하는 멤버 함수는 바로 함수 포인터로 사용되는 것이 아니기 때문이다. `const` 함수는 `const` 객체를 인자로 전달해주어야 한다.


## std::mem_fn

&nbsp;`<algorithm>`의 함수 등에서 Callable 객체를 전달해야하는 경우에도 멤버 함수를 전달하는 것은 까다로운 일이다. 가장 귀찮은 방법은 위에서 본 것처럼 `std::function<F(A&)>` 객체로 만들어서 전달하는 방법인데, C++에서는 이것을 한 번에 전달시킬 수 있도록 `std::mem_fn` 함수를 제공한다. 멤버 함수를 인자로 전달할 때 `std::mem_fn(&Test::mem)` 같은 형태로 전달하면 된다.


&nbsp;소개는 했지만 사실 `std::mem_fn`은 거의 안쓰는데, 그냥 인자로 람다 함수를 쓰면 되기 때문이다. 람다 함수를 래퍼 함수처럼 만들어서 해당 멤버 함수를 내부에서 호출하고 그 반환 값을 전달하면 된다.


## std::bind

&nbsp;`std::bind`는 **어떤 함수의 인자가 특정한 무언가로 반드시 정해져 있는 함수를 만들 필요가 있을 때, 새로운 함수를 만들 필요없이 간단하게 그러한 함수를 만들 수 있게 해준다**. 아래와 같이 사용한다.

```C++
// 인자가 a, b로 정해져있는 function 함수 반환
auto new_func = std::bind(function, a, b);
```


## std::reference_wrapper&lt;T>

&nbsp;`std::optional<T>`(C++ 17)등을 사용할 때처럼 레퍼런스 형을 그냥 `&` 기호를 통해 사용할 수 없는 경우가 있다. 이런 경우에도 레퍼런스처럼 사용할 수 있는 래퍼 클래스가 있는데 그것이 `std::reference_wrapper<T>`다. `std::reference_wrapper<T>` 객체를 생성하기 위해서는 `std::ref`를 사용하면 된다. 참고로 C++ 23부터는 `std::optional<T>`에 레퍼런스 타입도 사용이 가능하다.

```C++
#include <functional>
#include <iostream>
#include <optional>

class Nacho
{
public:
        Nacho(int i): n(i) {  }

        int get(void);
        void set(int i);
private:
        int n;
};

int Nacho::get(void)
{
        return n;
}

void Nacho::set(int i)
{
        n = i;
}

int main(void)
{
        Nacho n(8);
        std::optional<std::reference_wrapper<Nacho>> r_n = std::ref(n);

        // std::reference_wrapper<T>는 템플릿 등에서 사용될 때는 명시적으로 get을 통해 값을 이용해야 함.
        r_n->get().set(16);

        std::cout << "n.get(): " << n.get() << std::endl;

        return 0;
}
```
