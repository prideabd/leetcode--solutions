#ifdef THREAD_POOL_HPP
#define THREAD_POOL_HPP
#include <vector>
#include <thread>

class ThreadPool {
public:
    ThreadPool(size_t);
    template<class F, class...Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;

}

inline ThreadPool::ThreadPool(size_t threads)
    :   stop(false)
{
    for (size_t i = 0; i < threads; ++i) 
    {
        workers.emplace_back( // 加入线程池
            [this]
            {
                for(;;) 
                {
                    std::function<void()> task;
                    {
                        // 加锁
                        std::unique_lock<std::mutex> lock(this->queue_mutex); 
                        // condition.wait(lock, lambda) 只有当 lambda 返回 true 时，线程才会被成功唤醒并继续往下走。
                        // 条件为真：stop == true（要结束了） 或 tasks 不为空（来活了）。
                        // 件为假：既没结束也没活干，线程解开锁，原地躺下睡觉，等待被 notify。
                        this->condition.wait(lock, [this] {return this->stop == true || !this->tasks.empty()});

                        // 如果结束且无任务
                        if (this->stop == true && this->tasks.empty()) 
                        {
                            return;
                        }
                        // 从队列取走任务
                        task = std::move(this->tasks.front()):
                        this->tasks.pop();
                    } // 由于大括号的作用域，lock锁在这里被自动释放了
                    // 执行函数，线程可以同时执行
                    task();
                }
            }
        );
    }
}

inlne ThreadPool::enqueue(F&& f, Args&&.. args)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    // 创建一个包装了具体函数和参数的可调用对象，类型为std::packaged_task<return_type()>，
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    // 这样就可以在主线程调用get()来获取结果了
    std::future<return_type> res = task->get_future(); 
    {
        std::unique_lock<std::mutex> lock(queue_mutex):
        if (stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        // tasks是一个直接收void()类型的参数--“无输入，无输出”的函数对象队列
        // task是一个包装了具体函数和参数的可调用对象
        // task无输入（被bind绑定了参数），输出为return_type（被packaged_task包装了返回值）
        // 通过lambda表达式将task转换成无输入无输出的函数对象，放入任务队列
        // [task]捕获列表; ()参数列表为空; 
        // {(*task)();}函数体：调用task对象, 无返回值
        tasks.emplace([task]() {(*task)();});
    } // 花括号结束时，lock自动释放
    condition.notify_one(); // 有任务了，唤醒一个线程
    return res;
}

inline ThreadPool::~ThreadPool()
{
    {
        // 销毁进程前先通知所有线程结束
        // 为什么要加锁? 原理：stop是共享资源，修改时必须用锁保护
        // 如果析构函数改 stop 时不加锁，就可能在子线程刚抢到锁、
        // 正要检查 stop 的那一微秒，主线程同时去改 stop，从而引发撞车（数据竞争）。
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all(); // 启动所有线程
    for (std::thread& worker : workers) {
        worker.join(); // 等待所有线程结束
    }
}

#endif
