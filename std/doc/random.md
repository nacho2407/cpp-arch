# Random

&nbsp;기존 C에서 사용하던 난수 생성 방식은, 현재 시간을 통해 시드를 생성하고, 해당 시드를 통해 난수를 받아오는 방식이였는데, 이런 방식의 문제점은 우선 초 단위로 새 시드를 생성할 수 밖에 없기 때문에 서버 시스템에서 동일한 시간에 접속하면 동일한 난수열을 생성할 가능성이 있다는 것이고, 또 다른 문제는 애초에 `rand()` 함수의 난수 생성 방식인 선형 합동 생성기가 난수 간 상관 관계가 상당히 높아 성능이 썩 좋지 않다는 것이다. C++에서는 이러한 문제를 해결한 `<random>`이라는 라이브러리를 제공한다.


## &lt;random>

&nbsp;C++에서 난수를 생성하기 위해서, 우선 시드값을 얻기 위한 `std::random_device`를 이용한다. 시간을 시드로 사용하던 C의 `srand()`와 달리 **`std::random_device`는 운영체제에서 하드웨어의 노이즈값 등을 기반으로 생성한 진짜 난수를 이용**한다. 이 값 자체를 난수로 써도 되지 않겠나 생각할 수도 있으나, 이런 값은 의사 난수와 달리 생성하는데에 매우 높은 시간이 소요되기 때문에 **초기 시드 생성에만 사용하는 것이 적합**하다.


&nbsp;이후 난수를 얻어올 난수 생성 엔진을 초기화한다. C++ 난수 생성 엔진으로 가장 많이 사용되는 것은 `std::mt19937`로, 생성된 난수 간 상관 관계가 매우 낮은 메르센 트위스터 알고리즘을 사용한다.


&nbsp;참고로 메르센 트위스터 알고리즘은 난수 품질은 매우 좋지만 어느 정도 크기의 난수를 뽑고 나면 해당 내용으로 현재 생성기의 상태를 알아낼 수 있기 때문에 암호학적으로 그리 안전한 의사난수 생성기는 아니다. 암호학적으로 안전하게 설계된 BBS(Bloom Bloom Shub)라는 알고리즘도 있으나 자료가 부족하고 수학적 지식이 꽤 필요한지라... 이 이상 조사는 그만두었다.


&nbsp;`<random>`에는 `std::mt19937` 외에도 여러 난수 생성 엔진이 정의되어있다. `std::md19937`은 난수 품질은 좋지만 객체 크기가 2KB 이상으로 꽤 커서 메모리가 제한적인 환경에서는 오히려 기존 C의 `rand()`와 같은 선형 합동 생성기 방식을 쓰는 `std::minst_rand`를 사용하기도 한다.


&nbsp;난수 생성 엔진 만으로 난수를 생성할 수 있는 것은 아니다. C++ 난수의 가장 특징적인 부분으로, **생성할 난수의 분포를 결정**해야 한다. 가장 많이 사용하는 **균등 분포의 경우 `std::uniform_int_distributin<T>`(정수 버전) / `std::uniform_real_distribution<T>`(실수 버전)을 사용하고, 정규 분포의 경우 `std::normal_distribution<T>`을 사용**한다. `std::uniform_int_distribution<T>`의 경우 최솟값과 최댓값을 생성자로 받고, `std::normal_distribution<T>`의 경우 평균과 표준 분포를 생성자로 받는다.


```C++
#include <cstddef>
#include <iostream>
#include <random>

const std::size_t TEST_SIZE = 128;

int main(void)
{
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 99);

        for(std::size_t i = 0; i < TEST_SIZE; i++)
                std::cout << "Random generated >> " << dist(gen) << "\n";

        std::cout << std::flush;

        return 0;
}
```
