# Atomic & Memory Order

&nbsp;우리는 코드를 통해서 컴퓨터에 명령을 내릴 때 우리가 명령한 순서대로 작동할 것이라고 생각하지만, 실제로 우리의 컴퓨터(컴파일러와 CPU)는 우리가 무엇을 명령하는지만 생각하고 순서는 자기가 더 빠르게 작동할 수 있는 순서대로 마음대로 조직하여 명령을 수행한다. 순서가 어떻게 되던 간에 우리가 의도한대로 결과만 제대로 나오면 사실 컴퓨터가 더 빠르게 연산을 수행하는 것이 문제가 되지 않지만, 문제는 이것이 하나의 쓰레드 기준으로만 결과를 올바르게 보장한다는 것이다.


&nbsp;이러한 특성은 멀티 쓰레드 프로그래밍에서는 큰 장애물이 된다. 더 빠르게 연산을 수행하는 것은 분명 큰 장점이지만 결과가 예상대로 나오지 않으면 아무 소용이 없다. C++에서는 이러한 문제를 해결하기 위해 `<atomic>`을 통해 명령어의 원자성(Atomicity)을 보장하는 방법들을 제공한다. 원자적 연산이란, 더이상 쪼개질 수 없는 하나의 단위로 실행되는 연산으로, 다른 연산이 중간에 끼어들거나 중단되지 않음을 보장하는 것이다.


## std::atomic&lt;T>

```C++
#include <functional>
#include <iostream>
#include <thread>

void up(int& i)
{
        i++;
}

int main(void)
{
        static int i = 0;

        std::thread t1(up, std::ref(i));
        std::thread t2(up, std::ref(i));

        t1.join();
        t2.join();
        
        std::cout << i << std::endl;

        return 0;
}
```

&nbsp;위의 코드에서 대표적으로 발생할 수 있는 경쟁 조건은 데이터의 불일치(Inconsistency)다. `t1`이 `i + 1`을 계산 후 결과를 `i`에 저장하기 전에 `t2`가 현재의 값을 읽어서 연산을 수행하면, 연산의 수행 결과는 `2`가 아니라 `1`이 될 수 있는 것이다. 이러한 문제를 해결하기 위해서는 `i`의 값을 읽고, `i + 1`을 수행한 다음, 이를 다시 `i`에 저장하는 일련의 연산의 중간에 다른 연산이 끼지 못하도록 원자성을 보장하는 것이다. `std::atomic<T>`을 이용하여 구현하면 아래와 같다.

```C++
#include <atomic>
#include <functional>
#include <iostream>
#include <thread>

void up(std::atomic<int>& i)
{
        i++;
}

int main(void)
{
        static std::atomic<int> i = 0;

        std::thread t1(up, std::ref(i));
        std::thread t2(up, std::ref(i));

        t1.join();
        t2.join();

        std::cout << i << std::endl;

        return 0;
}
```

&nbsp;`i++`은 자동으로 `i.fetch_add(1, std::memory_order_seq_cst)`로 확장되어 연산된다. `std::atomic<T>`가 지원하는 함수는 여러가지가 있는데, 주로 `std::atomic<T>::store`, `std::atomic<T>::load`, `std::atomic<T>::fetch_add` 등이 사용된다. 사용자 정의 클래스와 함께 사용할 수도 있기는 하나, 복사/대입 연산자의 비명시, Trivially Copyable 등 조건이 꽤나 까다롭다.


&nbsp;`std::atomic<T>`과 관련한 연산은 내부적으로 연산될 때 CPU의 원자적 명령어들을 이용하여 연산된다. 이 때 CPU의 명령어 지원이 될 때만 원자적으로 연산되고, 지원이 안되는(매우 오래된 시스템 등) 경우 내부적으로 그냥 뮤텍스를 이용하여 연산된다. 이 경우에는 시간적 복잡도가 오히려 높아질 수도 있기 때문에 `std::atomic<T>::is_lock_free` 함수를 통해 CPU가 원자적 명령어를 지원하는지 확인할 수도 있다.


## std::memory_order

&nbsp;원자적 연산은 어셈블리어 수준까지 지정하는 연산이라 메모리에 어떤 방식으로 접근하는지까지 지정할 수 있다. `<atomic>`은 이를 위해 `std::memory_order`라는 열거형 타입을 지원한다.

- `std::momory_order_relaxed`: **가장 느슨한 조건**으로, 주위의 다른 메모리 접근 명령어들과 순서가 바뀔 수 있다.

- `std::memory_order_acquire`: 해당 명령을 기준으로 **이전에 위치한 메모리 접근 명령어들이 해당 명령 이후로 재배치되는 것을 제한**한다.

- `std::memory_order_release`: 해당 명령을 기준으로 **이후에 위치한 메모리 접근 명령어들이 해당 명령 이전으로 재배치되는 것을 제한**한다.

- `std::memory_order_acq_rel`: 이름과 같이 `std::memory_order_acquire`와 `std::memory_order_release`를 모두 보장한다.

- `std::memory_order_seq_cst`: **`std::atomic<T>`의 default이자 가장 엄격한 조건**으로, **메모리 접근 명령의 순차적 일관성(sequential consistency)를 보장**한다. 즉, 메모리 접근 명령어의 재배치가 일어날 수 없어서 실제 실행 시 속도는 가장 느리다.


&nbsp;아래와 같이 사용할 수 있다.

```C++
#include <array>
#include <atomic>
#include <cstddef>
#include <functional>
#include <iostream>
#include <thread>

const std::size_t NUM_DATA = 3;

void producer(std::array<int, NUM_DATA>& data, std::atomic<bool>& is_ready)
{
        for(int i = 0; i < data.size(); i++)
                data[i] = i;

        is_ready.store(true, std::memory_order_release);
}

void consumer(const std::array<int, NUM_DATA>& data, const std::atomic<bool>& is_ready)
{
        while(!is_ready.load(std::memory_order_acquire));

        for(int i = 0; i < data.size(); i++)
                std::cout << data[i];
}

int main(void)
{
        static std::array<int, NUM_DATA> data;
        static std::atomic<bool> is_ready(false);

        std::thread t_p(producer, std::ref(data), std::ref(is_ready));
        std::thread t_c(consumer, std::cref(data), std::cref(is_ready));

        t_p.join();
        t_c.join();

        return 0;
}
```

&nbsp;위의 코드에서는 release-acquire 동기화를 이용한다. 위처럼 간단한 경우에는 release-acquire 동기화를 통해 프로그램의 성능을 높힐 수 있으나, 쓰레드가 많은 복잡한 상황에서는 순서 보장이 부족해서 예상과 다르게 작동할 수 있다. 이러한 경우에는 `std::memory_order_seq_cst`를 이용할 수 밖에 없는데, 상기하였듯 `std::memory_order_seq_cst`는 비용이 높은 연산이라 정말 필요한 경우가 아니면 사용을 자제하는 것이 좋다. 같은 이유로, `std::memory_order_seq_cst`를 default로 사용하는 `std::atomic<T>`보다는 대부분의 경우 `std::mutex`와 `std::condition_variable`을 주로 사용한다.
