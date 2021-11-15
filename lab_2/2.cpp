#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <ctime>
#include <atomic>
#include <iomanip>

constexpr int TaskNum = 4 * 1042 * 1024; // 4194304

class Queue
{
protected:
    std::queue<uint8_t> _q;
    int	prod_count	= 0;
    bool prod_finished = false;
    std::mutex mutex;
public:
    virtual void push(uint8_t val) = 0;
    virtual bool pop(uint8_t& val) = 0;

    void add_producer() {
        std::lock_guard<std::mutex> locker(mutex);
        this->prod_count++;
    }

    void delete_producer() {
        std::lock_guard<std::mutex> locker(mutex);
        this->prod_count--;
        if (prod_count == 0)
            prod_finished = true;
    }
};

class FixedQueue : public Queue {
private:
    std::mutex lock;
    std::condition_variable check;
    std::size_t	q_size;

public:
    FixedQueue(std::size_t queue_size) {
        this->q_size = queue_size;
    }

    void push(uint8_t val) override {
        std::unique_lock<std::mutex> locker(mutex);
        check.wait(locker, [this] {return _q.size() < q_size; });
        this->_q.push(val);
        check.notify_all();
    }

    bool pop(uint8_t& val) override {
        std::unique_lock<std::mutex> locker(mutex);
        while (!prod_finished || _q.size() > 0) {
            if (check.wait_for(locker, std::chrono::microseconds(1), [this] {return _q.size() > 0; }))
            {
                val = _q.front();
                _q.pop();
                check.notify_all();
                return true;
            }
            check.notify_one();
        }
        check.notify_all();
        return false;
    }

};

class DynamicQueue : public Queue {
private:
public:

    void push(uint8_t val) override {
        std::lock_guard<std::mutex> locker(mutex);
        this->_q.push(val);
    }

    bool pop(uint8_t& val) override {
        mutex.lock();
        while (_q.empty()) {
            mutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            mutex.lock();
            if (prod_finished && _q.empty()) {
                mutex.unlock();
                return false;
            }
        }
        val = _q.front();
        _q.pop();
        mutex.unlock();
        return true;
    }
};

void consumer(Queue & q, std::atomic<int>& counter) {
    uint8_t val;
    int _counter = 0;
    while (true) {
        if (!q.pop(val)) {
            counter.fetch_add(_counter);
            break;
        }
        _counter += val;
    }

}

void producer(Queue& q) {
    int i = 0;
    q.add_producer();
    for (i = 0; i < TaskNum; i++) {
        q.push(1);
    }
    q.delete_producer();
}

unsigned int thread_func(int producerNum, int consumerNum, Queue& q, std::atomic<int> & counter) {
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < producerNum; i++)
        producers.push_back(std::thread(producer, std::ref(q)));
    for (int i = 0; i < consumerNum; i++)
        consumers.push_back(std::thread(consumer, std::ref(q), ref(counter)));

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < producerNum; i++) {
        producers[i].join();
        consumers[i].join();
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
}

int main()
{
    int ProducerNum[] = { 1, 2, 4 };
    int ConsumerNum[] = { 1, 2, 4 };
    size_t QueueSize[] = { 1, 4, 16 };

    std::cout << std::setw(10) << "Dynamic Queue: \n";
    for (int i = 0; i < 3; i++) {
        DynamicQueue dynamicQ;
        std::atomic<int> counter;
        std::cout << std::setw(8) << thread_func(ProducerNum[i], ConsumerNum[i], dynamicQ, counter);
        std::cout << std::setw(8) << "\t" << (counter == ProducerNum[i] * TaskNum ? "Unsuccess" : "Success") << std::endl;
    }

    std::cout << "\nFixed Queue: \n";
    for (int i = 0; i < 3; i++) {
        FixedQueue fixedQ(QueueSize[i]);
        std::atomic<int> counter;
        std::cout << std::setw(8) << thread_func(ProducerNum[i], ConsumerNum[i], fixedQ, counter);
        std::cout << std::setw(8) << "\t" << (counter == ProducerNum[i] * TaskNum ? "Unsuccess" : "Success") << std::endl;
    }
}