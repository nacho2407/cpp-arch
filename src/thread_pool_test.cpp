#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <type_traits>
#include <vector>

namespace nacho
{
        class ThreadPool
        {
        public:
                ThreadPool(std::size_t s);

                ~ThreadPool(void);

                /**
                 * @brief 수행할 job을 jobs에 추가
                 */
                template <typename F, typename... Args>
                std::future<std::invoke_result_t<F, Args...>> enqueue_job(F&& job, Args&&... args);
        private:
                std::size_t pool_size;

                std::vector<std::thread> workers;

                // 임계 구역
                std::queue<std::function<void(void)>> jobs;

                std::mutex m;
                std::condition_variable cv;

                // ThreadPool이 정지되었는지 여부
                bool is_disabled;

                /**
                 * @brief std::thread에서 실행될 worker
                 */
                void worker(void);
        };

        ThreadPool::ThreadPool(std::size_t s): pool_size(s), is_disabled(false)
        {
                // workers 공간 확보
                workers.reserve(s);

                // std::vector<T>::emplace_back을 통해 람다 함수로 std::thread 생성 후 workers에 추가
                for(std::size_t i = 0; i < s; i++) {
                        workers.emplace_back([this]() {
                                this->worker();
                        });
                }
        }

        ThreadPool::~ThreadPool(void)
        {
                // ThreadPool 사용 정지(모든 worker 정지)
                is_disabled = true;
                cv.notify_all();

                // (참고) std::thread는 복사 생성 불가능
                for(auto& w : workers)
                        w.join();
        }

        template <typename F, typename... Args>
        std::future<std::invoke_result_t<F, Args...>> ThreadPool::enqueue_job(F&& job, Args&&... args)
        {
                // 완벽한 전달 패턴을 위해 보편적 레퍼런스 사용
                
                // ThreadPool이 사용 정지된 후에 작업을 추가하려하면 예외 발생
                if(is_disabled)
                        throw std::runtime_error("ThreadPool has been disabled.");

                // 모든 타입의 함수를 처리하기 위해 템플릿을 사용하여 job을 받고, std::future<T>를 통해 반환값을 전달하는 std::packaged_task<F>로 래핑
                // worker에서 수행될 작업을 해제하지 않으면서 효율을 높히기 위해 std::shared_ptr<T>를 사용(그냥 사용하면 함수 반환 시 수행될 작업이 해제됨)
                // std::forward<T>를 통해 완벽한 전달 패턴 구현
                using return_type = typename std::invoke_result_t<F, Args...>;
                auto job_ptr = std::make_shared<std::packaged_task<return_type(void)>>(std::bind(std::forward<F>(job), std::forward<Args>(args)...));
                std::future<return_type> job_result = job_ptr->get_future();

                // 뮤텍스를 잠근 후 수행할 std::packaged_task<F>를 void(void) 타입의 람다 함수로 래핑하여 jobs에 추가
                std::unique_lock<std::mutex> lk(m);
                jobs.push([job_ptr]() {
                        (*job_ptr)();
                });
                
                lk.unlock();
                cv.notify_one();

                // job의 반환값을 전달할 std::future<T> 반환
                return job_result;
        }

        void ThreadPool::worker(void)
        {
                while(true) {
                        // 뮤텍스를 잠근 후 수행할 작업이 추가될 때까지 대기
                        std::unique_lock<std::mutex> lk(m);
                        cv.wait(lk, [this]() {
                                return (!this->jobs.empty() || this->is_disabled);
                        });

                        // 남은 작업이 없고 ThreadPool이 사용 정지된 상태이면 쓰레드 종료
                        if(this->jobs.empty() && is_disabled)
                                return;

                        // jobs에서 수행할 작업 꺼내기
                        std::function<void(void)> job = jobs.front();
                        jobs.pop();

                        // 잠금 해제 후 작업 시작
                        lk.unlock();

                        job();
                }
        }
}

/* Test Code */

const std::size_t TEST_SIZE = 10;

void run(void);

int test_job(std::size_t t, int id);

int main(void)
{
        run();

        return 0;
}

void run(void)
{
        nacho::ThreadPool pool(4);

        std::vector<std::future<int>> futures;
        for(std::size_t i = 0; i < TEST_SIZE; i++)
                futures.push_back(pool.enqueue_job(test_job, i % 3 + 1, i));

        std::ostringstream oss;

        for(std::size_t i = 0; i < TEST_SIZE; i++) {
                oss.str("");
                oss.clear();

                // 아래와 같이 <format>의 std::format을 사용할 수도 있으나, 아직 C++ 20이 널리 채택되지 않은 관계로(2025.02) 이 코드에서는 <sstream> 이용
                // std::cout << std::format("job({}) returns value({}).\n", i, futures[i].get()) << std::flush;
                oss << "job(" << i << ") returns value(" << futures[i].get() << ").\n";

                std::cout << oss.str() << std::flush;
        }
}

int test_job(std::size_t t, int id)
{
        std::ostringstream oss;
        oss << "job(" << id << ") is started.\n";

        std::cout << oss.str() << std::flush;

        std::this_thread::sleep_for(std::chrono::seconds(t));

        oss.str("");
        oss.clear();

        oss << "job(" << id << ") is finished after " << t << " seconds.\n";

        std::cout << oss.str() << std::flush;

        return t + id;
}
