#include <chrono>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

const std::size_t TEST_SIZE = 1048576; // 2^20

int main(void)
{
        std::random_device rd;
        
        std::mt19937 mt(rd());

        // 균등 분포
        std::uniform_int_distribution<int> dist(0, TEST_SIZE);
        std::vector<int> vec;

        vec.reserve(TEST_SIZE);

        // 현재 시간을 기록한 후 std::random_device를 이용한 난수 생성 수행
        auto pt = std::chrono::high_resolution_clock::now();
        for(std::size_t i = 0; i < TEST_SIZE; i++)
                vec[i] = dist(rd);

        // 생성 후 시간 차이를 마이크로초로 변환하여 출력
        auto dur = std::chrono::high_resolution_clock::now() - pt;
        auto micro = std::chrono::duration_cast<std::chrono::microseconds>(dur);

        std::cout << "Time taken to generate " << TEST_SIZE << " random values with std::random_device >> " << micro.count() << " us" << std::endl;

        // 동일 작업을 std::mt19937을 이용하여 수행
        pt = std::chrono::high_resolution_clock::now();
        for(std::size_t i = 0; i < TEST_SIZE; i++)
                vec[i] = dist(mt);

        dur = std::chrono::high_resolution_clock::now() - pt;
        micro = std::chrono::duration_cast<std::chrono::microseconds>(dur);

        std::cout << "Time taken to generate " << TEST_SIZE << " random values with std::mt19937 >> " << micro.count() << " us" << std::endl;

        return 0;
}
