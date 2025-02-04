#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
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
                workers.reserve(s);

                for(int i = 0; i < s; i++) {
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

        // 완벽한 전달 패턴을 위해 보편적 레퍼런스 사용
        template <typename F, typename... Args>
        std::future<std::invoke_result_t<F, Args...>> ThreadPool::enqueue_job(F&& job, Args&&... args)
        {
                // ThreadPool이 사용 정지된 후에 작업을 추가하려하면 예외 발생
                if(is_disabled)
                        throw std::runtime_error("ThreadPool has been disabled.");

                // 모든 타입의 함수를 처리하기 위해 템플릿을 사용하여 job을 받고, std::future<T>를 통해 반환값을 전달하는 작업으로 래핑
                // worker에서 수행될 작업을 해제하지 않으면서 효율을 높히기 위해 std::shared_ptr<T>를 이용(그냥 사용하면 함수 반환 시 수행될 작업이 해제됨)
                // std::forward<T>를 통해 완벽한 전달 패턴 구현
                using return_type = typename std::invoke_result_t<F, Args...>;
                auto job_ptr = std::make_shared<std::packaged_task<return_type>(void)>(std::bind(std::forward<F>(job), std::forward<Args>(args)...));
                std::future<return_type> job_result = job_ptr->get_future();

                {
                        // 뮤텍스를 잠근 후 void(void) 타입의 람다 함수로 수행할 작업을 래핑하여 jobs에 추가
                        std::unique_lock<std::mutex> lk(m);
                        jobs.push([job_ptr]() {
                                (*job_ptr)();
                        });
                }
                cv.notify_one();

                // std::future<T>를 통해 job의 반환값 전달
                return job_result;
        }

        void ThreadPool::worker(void)
        {
                // 작성중
        }
}
