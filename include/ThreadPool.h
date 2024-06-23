#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <functional>
class ThreadPool{
public:
    static ThreadPool& GetThreadPool(size_t num);
    static ThreadPool* tp;
    static std::once_flag once;
    void stop();
    template<class F,class ...Args>
    void add_task(F &&f,Args &&...args){
        std::function<void()> func=std::bind(std::forward<F>(f),std::forward<Args>(args)...);
        {
            std::lock_guard<std::mutex> lock(mtx);
            task_queue.emplace(func);
        }
        cond_.notify_one();
    }
    ~ThreadPool();
private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> task_queue;
    std::mutex mtx;
    std::condition_variable cond_;
    ThreadPool(size_t thread_num);
    ThreadPool(const ThreadPool &t) = delete;
    ThreadPool& operator = (const ThreadPool &t) = delete;
    std::atomic<bool> stop_;
    class PtrDel{
        public:
            PtrDel(){};
            ~PtrDel(){
                if(ThreadPool::tp){
                    delete tp;
                    ThreadPool::tp=nullptr;
                }
            }
    };
};