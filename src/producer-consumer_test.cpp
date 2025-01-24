#include <chrono>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

const std::size_t NUM_PAGES = 5;
const std::size_t NUM_THREADS = 4;

void run(void);

/**
 * @brief producer: page 다운로드를 시뮬레이션
 */
void downloader(std::deque<std::string>* downloaded, std::size_t ti, std::mutex* mtx, std::condition_variable* cv);

/**
 * @brief consumer: page 처리를 시뮬레이션
 */
void processor(std::deque<std::string>* downloaded, std::size_t* num_processed, std::mutex* mtx, std::condition_variable* cv);

int main(void)
{
        run();

        return 0;
}

void run(void)
{
        // 임계 구역
        static std::deque<std::string> downloaded;
        static std::size_t num_procesed = 0;
        
        static std::mutex mtx;
        static std::condition_variable cv;

        // downloader 쓰레드 시작
        std::vector<std::thread> downloaders;
        for(std::size_t i = 0; i < NUM_THREADS; i++)
                downloaders.push_back(std::thread(downloader, &downloaded, i, &mtx, &cv));
        
        // processor 쓰레드 시작
        std::vector<std::thread> processors;
        for(std::size_t i = 0; i < NUM_THREADS; i++)
                processors.push_back(std::thread(processor, &downloaded, &num_procesed, &mtx, &cv));

        // page 다운로드 대기
        for(std::size_t i = 0; i < NUM_THREADS; i++)
                downloaders[i].join();

        // 대기 중인 processor 전체에 시작 신호(종료를 위해)
        cv.notify_all();

        // page 처리 대기
        for(std::size_t i = 0; i < NUM_THREADS; i++)
                processors[i].join();
}

void downloader(std::deque<std::string>* downloaded, std::size_t ti, std::mutex* mtx, std::condition_variable* cv)
{
        for(std::size_t i = 0; i < NUM_PAGES; i++) {
                // page 다운로드
                std::string page = "page(" + std::to_string(i) + ") from downloader(" + std::to_string(ti) + ")";

                std::this_thread::sleep_for(std::chrono::milliseconds(128 * ti));

                {
                        // mutex를 잠근 후 임계 구역 진입
                        std::unique_lock<std::mutex> lk(*mtx);
                        downloaded->push_back(page);
                }

                // processor 하나에 시작 신호
                cv->notify_one();
        }
}

void processor(std::deque<std::string>* downloaded, std::size_t* num_processed, std::mutex* mtx, std::condition_variable* cv)
{
        while(*num_processed < NUM_PAGES * NUM_THREADS) {
                // mutex를 잠근 후 처리할 page가 다운로드되기 전까지 대기
                std::unique_lock<std::mutex> lk(*mtx);

                cv->wait(lk, [&] -> bool {
                        return !downloaded->empty() || *num_processed == NUM_PAGES * NUM_THREADS;
                });

                // 대기 종료 후 이미 모든 page 처리가 끝난 상태이면 쓰레드 종료
                if(*num_processed == NUM_PAGES * NUM_THREADS)
                        return;

                // 처리할 page 가져오기
                std::string page = std::move(downloaded->front());
                downloaded->pop_front();
                page.append(" processed.\n");

                // 처리한 page 수를 올리며 mutex 잠금 해제
                (*num_processed)++;
                lk.unlock();

                // page 처리
                std::this_thread::sleep_for(std::chrono::milliseconds(64));

                std::cout << page;
        }
}
