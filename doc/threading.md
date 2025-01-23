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
<br>

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


## std::mutex, std::unique_lock

(작성중)
