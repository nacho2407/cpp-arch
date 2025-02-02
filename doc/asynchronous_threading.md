# Asynchronous Threading

&nbsp;C++로 멀티 쓰레드 프로그램을 만들 때 일반적으로 사용되는 생산자-소비자 모델을 보면, 소비자가 작업할 내용을 생산자가 생산하기까지 소비자가 대기하는 모습을 볼 수 있다. 하지만 생산자가 작업을 마치기 전에 소비자가 다른 작업을 수행할 수 있다면 더 효율적이지 않은가? 이러한 작업을 비동기적(asynchronous) 작업이라 한다. C++은 이러한 비동기적 쓰레딩을 쉽게 사용할 수 있도록 `<future>`를 제공한다.


## std::promise, std::future

&nbsp;비동기적 쓰레딩은 기본적으로 `std::promise`와 `std::future`의 상호작용이다. `std::promise`는 어떤 값을 미래에 전달하겠다는 약속이고, `std::future`를 통해 그 값을 이용한다. 이러한 패턴은 뮤텍스와 조건 변수를 활용하여 구현할 수도 있으나 `std::promise`와 `std::future`를 통해 더 편하게 구현할 수 있다. 아래와 같이 사용한다.

```C++
#include <future>
#include <iostream>

void prom(std::promise<int>& p)
{
        // 작성중
}
```

(작성중) - std::promise와 std::future, std::future_status, std::shared_future, std::packaged_task, std::async
