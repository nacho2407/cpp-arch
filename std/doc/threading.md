# Threading

&nbsp;병렬 처리가 가능한 작업을 빠르게 수행하기 위해 우리는 멀티 쓰레딩을 한다. 기존 C / C++ 표준에는 쓰레드 관련 기능이 없어 POSIX(`<pthread.h>`)나 Windows API(`<windows.h>`) 등 프로그램이 실행될 플랫폼에 맞추어 구현하여야 했기에 플랫폼 간 상호 운용성이이 떨어졌다. 하지만 C++ 11에서 `<thread>`가 표준으로 자리 잡으면서 멀티 쓰레딩 관련 기능을 쉽게 구현할 수 있게 되었다. 아래와 같이 사용한다.

```C++
#include <iostream>
#include <thread>

void say_hi(void)
{
        std::cout << "Hello, world!\n";
}

int main(void)
{
        // 쓰레드 생성
        std::thread t1(say_hi);
        std::thread t2(say_hi);

        // 쓰레드 종료 대기
        t1.join();
        t2.join();

        return 0;
}
```


## std::thread::join, std::thread::detach

&nbsp;위의 코드에서 **`std::thread`는 `join`을 통해 실행되는 것이 아니라 객체가 생성되는 동시에 쓰레드가 실행된다**. `join`은 `std::thread`를 생성한 (메인) 쓰레드에서 생성한 쓰레드가 끝나기를 기대하는 지점에 사용한다. **`join`은 해당 쓰레드가 종료되어야 반환되기 때문에 해당 쓰레드가 종료되기 전까지 메인 쓰레드는 해당 지점에서 멈추게 된다**.


&nbsp;만약 위의 코드에서 `join`을 호출하지 않으면 메인 쓰레드가 먼저 종료되면서 `std::terminate`가 호출되어 생성한 쓰레드가 끝나기도 전에 프로그램 자체가 종료되어버린다. 만약 메인 쓰레드와 관계없이 **마치 다른 프로그램처럼 비동기적으로 동작하는 쓰레드를 만들고 싶다면 `detach` 함수를 호출**하면 된다. `detach`된 쓰레드는 메인 쓰레드가 종료되어도(해당 쓰레드를 생성한 프로그램 자체가 종료되어도) 백그라운드에서 계속 작업을 수행하게 되는데, 이 때 무한 반복 등의 문제가 발생하면 **고아 프로세스가 될 가능성이 있으므로 리소스 관리에 특히 주의를 기울여야 한다**.


## Printing in Thread

&nbsp;쓰레드에서 `std::cout`을 이용하여 값을 출력할 때 주의할 점이 있다. 아래의 코드를 보자.

```C++
#include <iostream>
#include <thread>

void say_hi(void)
{
        // ??
        std::cout << "Hello, " << "world!" << std::endl;
}

int main(void)
{
        std::thread t1(say_hi);
        std::thread t2(say_hi);
        std::thread t3(say_hi);

        t1.join();
        t2.join();
        t3.join();

        return 0;
}
```

&nbsp;얼핏 보기에는 큰 문제가 없어 보이지만, 실제로 실행해보면 출력 내용이 뒤죽박죽이 될 가능성이 높다. 쓰레드는 **CPU의 문맥 교환으로 인해 언제든지 중간에 다른 쓰레드가 실행될 수 있기 때문에 `std::cout << "Hello, "`까지만 실행되고 다른 쓰레드의 내용이 출력될 수 있다**. 때문에 **`std::string`으로 통째로 만들어 출력하거나 `std::cout`을 위한 `std::mutex`를 따로 생성하여 관리**해야 한다.


&nbsp;C-Style의 `printf`를 쓰는 방법도 있는데, 모던 C++에서 `printf`를 잘 쓰지 않기도 하거니와 동기화를 지원하는 `std::cout`과 달리 비동기식 출력이라 쓰레드 간 문맥 교환이 중간에 발생하면 한 문자열 안에서도 내용이 뒤죽박죽 출력되기 때문에 추천하지 않는다. `std::cout`은 내용이 동기식이라 한 문자열에서 내용이 중간에 끊길 우려는 없다.


## std::mutex, std::unique_lock&lt;T>

&nbsp;병렬 프로그래밍이나 멀티 쓰레딩 환경에서 동일한 리소스에 여러 프로세스(쓰레드)가 접근하려고 하면 실행 순서에 따라 결과가 의도치 않게 나올 수 있는데, 이를 경쟁 조건(Race Condition)이라 한다. 이러한 경쟁 조건을 해결할 수 있는 방법은 여러가지가 있는데, **C++에서는 뮤텍스(Mutex)라는 일종의 잠금 장치를 제공한다**. `std::mutex`는 아래와 같이 사용한다.

```C++
#include <iostream>
#include <mutex>
#include <thread>

void up_one(int* i, std::mutex* mtx)
{
        mtx->lock();

        (*i)++;

        mtx->unlock();
}

void up_two(int* i, std::mutex* mtx)
{
        // std::lock_guard를 사용하는 방법
        std::lock_guard lk(*mtx);

        *i += 2;
}

int main(void)
{
        static int i = 0;
        
        std::mutex mtx;

        std::thread t1(up_one, &i, &mtx);
        std::thread t2(up_two, &i, &mtx);

        t1.join();
        t2.join();

        std::cout << i << std::endl;

        return 0;
}
```
<br>

&nbsp;`std::mutex`로 락을 걸게 되면 다른 쓰레드들은 해당 락이 해제될 때까지 무한정 기다리게 되는데, 락을 취득한 쓰레드가 락을 해제하지 않은 채 종료되면 프로그램이 더 이상 진행되지 않는 데드락 상태에 빠질 수 있다. `std::lock_guard`는 스마트 포인터와 같은 래퍼 클래스로, `std::lock_guard` 객체가 해제될 때 자신이 소유하고 있던 `std::mutex`의 락도 함께 해제해준다.


&nbsp;`std::unique_lock<T>`은 `std::lock_guard`와 사용법은 조금 다르지만 거의 유사하다. `std::lock_guard`는 명시적인 락 해제가 불가능하지만, `std::unique_lock<T>`은 명시적인 잠금과 해제가 가능하여 더 유연하다. 아래와 같이 사용한다.

```C++
#include <iostream>
#include <mutex>
#include <thread>

void up_one(int* i, std::mutex* mtx)
{
        std::unique_lock<std::mutex> lk(*mtx);

        (*i)++;

        lk.unlock();

        // 이후 다시 lock 가능
}

void up_two(int* i, std::mutex* mtx)
{
        std::unique_lock<std::mutex> lk(*mtx);

        *i += 2;

        lk.unlock();
}

int main(void)
{
        static int i = 0;
        
        std::mutex mtx;

        std::thread t1(up_one, &i, &mtx);
        std::thread t2(up_two, &i, &mtx);

        t1.join();
        t2.join();

        std::cout << i << std::endl;

        return 0;
}
```


## Producer-Consumer Pattern, std::condition_variable

&nbsp;생산자-소비자 패턴은 멀티 쓰레드 프로그래밍에서 자주 사용되는 디자인 패턴으로, 하나 이상의 생산자 쓰레드와 하나 이상의 소비자 쓰레드 간의 작업을 동기화하여 데이터의 생산-소비 과정을 효율적으로 관리하는 구조를 제공한다.


&nbsp;데이터를 생산하는 생산자 쓰레드는 공유 자원(임계 구역)인 버퍼에 데이터를 저장하고, 소비자 쓰레드는 버퍼에서 데이터를 소비(처리)한다. 생산자 쓰레드가 데이터를 저장하는 동안에는 소비자 쓰레드가 할 일이 없으므로 버퍼에 값이 들어올 때까지 계속 확인해야 하는데, 이러한 방식을 폴링(Polling) 방식이라하며 매우 비효율적이다.


&nbsp;생산자-소비자 패턴에서 소비자 쓰레드가 할 일이 있는지를 확인하는 등의 상황에서 효율적으로 사용할 수 있는 것이 조건 변수(`std::condition_variable`)다. 조건 변수는 조건 함수를 인자로 받아 해당 함수의 반환값이 `false`이면 누군가가 자기를 깨울 때까지 해당 쓰레드를 대기시킨다. 생산자-소비자 패턴에서 생산자 쓰레드가 데이터 저장을 마치면 소비자 쓰레드를 깨우기 위해 조건 변수에게 신호를 주면 된다. `std::condition_variable`을 이용한 생산자-소비자 패턴의 [예제 코드](../src/producer-consumer_pattern_test.cpp)는 `../src/producer-consumer_test.cpp`에서 확인할 수 있다.


## Thread Pool

&nbsp;서버 프로그램을 제작할 때, 접속 폭주로 클라이언트를 처리할 쓰레드 수가 너무 많아지면 프로그램의 효율이 급속히 떨어지거나 다운되어 버릴 수 있다. 이런 경우엔 보통 쓰레드 풀(Thread Pool)을 이용하여 실행할 쓰레드를 미리 정해진 수 만큼 만들어두고 작업이 필요할 때마다 쓰레드를 하나 꺼내어 작업을 시킨다. 서버-클라이언트 시스템에서는 쓰레드 풀에 대기 상태의 쓰레드가 없는 경우 클라이언트를 적절히 대기시킴으로써 자원을 효율적으로 분배할 수 있다. 그 외에도 UI 응답성 향상이나 불필요한 쓰레드의 과생성 억제 등 쓰레드 풀을 이용하는 장점이 많다. 쓰레드 풀의 [예제 코드](../src/thread_pool_test.cpp)는 `../src/thread_pool_test.cpp`에서 확인할 수 있다.
