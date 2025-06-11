# Asynchronous Threading

&nbsp;C++로 멀티 쓰레드 프로그램을 만들 때 일반적으로 사용되는 생산자-소비자 모델을 보면, 소비자가 작업할 내용을 생산자가 생산하기까지 소비자가 작업을 시작하지 않고 대기하는 모습을 볼 수 있다. 하지만 생산자가 작업을 마치기 전에 소비자가 다른 작업을 수행할 수 있다면 효율이 더 높아지지 않겠는가? 이러한 작업을 비동기적(asynchronous) 작업이라 한다. C++은 이러한 비동기적 쓰레딩을 쉽게 사용할 수 있도록 `<future>`를 제공한다.


## `std::promise<T>`, `std::future<T>`

&nbsp;비동기적 쓰레딩은 기본적으로 `std::promise<T>`와 `std::future<T>`의 상호작용이다. `std::promise<T>`를 통해 어떤 값을 미래에 전달하겠다는 약속을 잡고, `std::future<T>`를 통해 그 값을 이용한다. 이러한 패턴은 뮤텍스와 조건 변수를 활용할 수도 있으나 `std::promise<T>`와 `std::future<T>`를 통해 더 편하게 구현할 수 있다. 아래와 같이 사용한다.

```C++
#include <functional>
#include <future>
#include <iostream>
#include <thread>

void prom(std::promise<int>& p)
{
        p.set_value(8);
}

int main(void)
{
        std::promise<int> p;
        std::future<int> f = p.get_future();

        std::thread t(prom, std::ref(p));

        f.wait(); // Optional

        std::cout << f.get() << std::endl;

        t.join();

        return 0;
}
```

&nbsp;`std::promise<T>`의 템플릿을 `std::future<T>::get`이 될 타입으로 생성하고, `std::promise<T>::get_future()`를 통해 `std::future<T>`를 받아온다. 비동기적으로 작동할 쓰레드에서는 `std::future<T>::get`으로 값을 이용한다. `std::promise<T>`는 `std::promise<T>::set_value`를 통해 `std::future<T>`에 값을 설정한다. `std::promise<T>::set_exception`를 통해 예외를 설정할 수도 있다.


&nbsp;주의해야 할 점은, `std::future<T>::get`은 호출되는 순간 `std::future<T>` 값이 이동되기 때문에 호출 후 다시 값을 가져올 수 없다. 뿐만 아니라 `std::promise<T>`와 `std::future<T>`는 복사 생성이 불가능하므로 함수 인자 등으로 전달 시에는 `std::move`를 이용하거나 레퍼런스로 넘겨주어야 한다.


&nbsp;`std::future<T>::wait`은 `std::future<T>::get`의 값이 이용 가능해질 때까지 대기하는 함수인데, 사실 `std::future<T>::get`만 호출해도 값이 이용 가능할 때까지 해당 쓰레드가 대기하기 때문에 Optional이다. 그보다 더 자주 사용하는 것은 `std::future<T>::wait_for`로, `std::chrono`를 통해 정해놓은 시간까지만 값이 설정되기를 기다린다. 때문에 실제로 `std::future<T>::get`을 사용하려 할 때 값이 설정되어 있지 않은 상태일 수 있는데, `std::future<T>`의 상태는 `std::future_status`로 확인할 수 있다. 아래와 같이 사용한다.

```C++
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <thread>

void prom(std::promise<int>& p)
{
        std::this_thread::sleep_for(std::chrono::seconds(8));

        p.set_value(8);
}

int main(void)
{
        std::promise<int> p;
        std::future<int> f = p.get_future();

        std::thread t(prom, std::ref(p));

        while(true) {
                std::future_status stat = f.wait_for(std::chrono::seconds(1));
                
                if(stat == std::future_status::timeout)
                        std::cerr << ">";
                else if(stat == std::future_status::ready)
                        break;
        }

        std::cout << "\n" << f.get() << std::endl;

        t.join();

        return 0;
}
```
<br>

&nbsp;`std::future<T>`는 `std::promise<T>`에서 설정되는 값을 유일하게 소유하기 때문에 복사 생성이 불가하고 이동만 가능하다. `std::future<T>`의 값을 공유할 필요가 있을 때는 대신 `std::shared_future<T>`를 사용할 수 있다. `std::shared_future<T>`는 복사 생성이 가능하다. 사용법은 `std::future<T>`와 동일하다.


## std::packaged_task&lt;F>, std::async

&nbsp;`std::packaged_task<F>`를 이용하면 비동기 작업을 정의할 때 `std::promise<T>`로 값을 일일이 설정하지 않고 함수의 반환값으로 설정할 수 있다. 아래와 같이 사용할 수 있다. `F`로는 비동기 실행할 함수의 타입을 넣어준다.

```C++
#include <future>
#include <iostream>
#include <thread>

int prom(void)
{
        return 8;
}

int main(void)
{
        std::packaged_task<int(void)> pt(prom);
        std::future<int> f = pt.get_future();

        std::thread t(std::move(pt));

        std::cout << f.get() << std::endl;

        t.join();

        return 0;
}
```

&nbsp;`std::packaged_task<F>` 또한 비동기 작업을 유일하게 쇼유하기 위해 복사 생성이 불가능하므로, `std::thread`를 생성할 때 소유권을 이동시켜야 한다.


&nbsp;`std::packaged_task<F>`의 생성자는 함수만 받기 때문에 인자를 전달하고 싶으면 작업을 수행할 쓰레드에서 `std::packaged_task<F>`에 직접 인자를 주면서 실행하거나 생성자에서 `std::bind`를 사용하여 인자와 아예 바인딩시켜 넘겨주면 된다.


&nbsp;`std::thread`를 명시하지 않고 비동기 쓰레딩을 할 수 있는 방법도 있다, `std::async`는 `std::packaged_task<F>`처럼 함수의 반환 값으로 `std::future<T>`의 값을 설정한다. `std::thread`를 명시할 때는 해당 비동기 쓰레드가 `std::thread` 객체 생성 시점에 시작되었지만, `std::async`는 객체 생성 시점에 쓰레드를 시작할 지, 값이 필요할 때 시작할 지를 설정할 수 있다. 이 때는 `std::launch`를 추가로 명시한다. 명시하지 않으면 따로 default 값이 없이 CPU가 알아서 실행 시점을 결정하기 때문에 필요한 경우 반드시 명시하는 것이 좋다.


&nbsp;`std::packaged_task<F>`와 `std::async`는 특히 람다 표현식과 함께 사용할 때 코드가 매우 깔끔해진다. 예시는 아래와 같다.

```C++
#include <future>
#include <iostream>

int main(void)
{
        std::future<int> f = std::async(std::launch::async, []() -> int { return 8; });

        std::cout << f.get() << std::endl;

        return 0;
}
```

&nbsp;`std::async`는 `std::future<T>`와 동기화되어 `std::future<T>`가 소멸될 때 `std::async`가 끝날 때까지 대기하기 때문에 `std::thread`처럼 `join`등을 명시할 필요가 없다.
