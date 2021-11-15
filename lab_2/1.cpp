#include <iostream>
#include<thread>
#include<list>
#include<chrono>
#include<mutex>
#include <stdlib.h>
#include <functional>
#include <atomic>
#include<vector>
#include <queue>
#include <condition_variable>

using namespace std;

template <class E>
class QueueItem
{

public:
    E* data;
    QueueItem* next;

    QueueItem(E* data)
    {
        this->data = data;
        next = NULL;
    }
};

template <class T>
class Queue
{
public:
    Queue()
    {
        QueueItem<T>* stub = new QueueItem<T>(NULL);

        head = stub;
        tail = stub;
    }

    bool empty()
    {
        return head == tail;
    }

    void enqueue(T value)
    {

        T* buf = new T(value);
        QueueItem<T>* item = new QueueItem<T>(buf);

        item->data = buf;
        item->next = NULL;

        tail->next = item;

        tail = item;
    }

    void enqueue(T* value)
    {
        QueueItem<T>* item = new QueueItem<T>(value);

        item->data = value;
        item->next = NULL;

        tail->next = item;

        tail = item;
    }

    T* dequeue()
    {
        if (head == tail)
            return NULL; // queue is empty

        QueueItem<T>* tmp = head;
        head = head->next;
        delete tmp;

        return head->data;
    }

private:
    QueueItem<T>* head;
    QueueItem<T>* tail;

};

class TaskIndexer
{

public:
    virtual int getIndex() = 0;
};

class MutexTaskIndexer : public TaskIndexer
{

private:
    int Index = 0;
    std::mutex testMutex;
public:
    int getIndex()
    {
        const lock_guard<mutex> lock(testMutex);
        return Index++;
    }
};

class AtomicTaskIndexer : public TaskIndexer
{

private:
    std::atomic<int> Index{0};
public:
    int getIndex()
    {
        return Index++;
    }
};

class ThreadClass
{
public:
    ThreadClass(int threadsAmount, std::function<void(void)> func)
    {
        for(int i=0; i<threadsAmount; i++)
        {
            NumThreadsList.push_back(std::thread(func));
        }
    }

    void join()
    {
        for(std::thread & th : NumThreadsList)
        {
            th.join();
        }

        joinded = true;
    }


    ~ThreadClass()
    {
        if(!joinded)
            join();
    }

private:
    std::list<std::thread> NumThreadsList;
    bool joinded = false;
};

void ArrayInit(vector<int8_t> &initArray, int arraySize, TaskIndexer &indexer)
{
    while(true)
    {
        int index = indexer.getIndex();

        if(index >= arraySize)
            return;

        initArray.at(index)++;
        this_thread::sleep_for(10ms);
    }
}


void printArray(int8_t *arr1, int length)
{
    for (int index=0; index < length; ++index)
        std::cout <<"INDEX = " << index << " Value = " << (int) arr1[index] << "\n" ;
}

void printArray(vector<int8_t> arr1, int length)
{
    for (int index=0; index < length; ++index)
        std::cout <<"INDEX = " << index << " Value = " << (int) arr1[index] << "\n" ;
}

class IQueue
{

public:
    virtual void push(uint8_t val) = 0;
    virtual bool pop(uint8_t& val) = 0;
};


void Task1(TaskIndexer *selectedIndexer)
{
    constexpr int NumTasks = 1024;
    constexpr int NumThreads = 12;

    vector<int8_t> arr(NumTasks, 0);

    TaskIndexer *indexer = selectedIndexer;
    auto processFunction = bind(ArrayInit, std::ref(arr), NumTasks, std::ref(*indexer));

    ThreadClass threadInstance(NumThreads, processFunction);

    threadInstance.join();
    printArray(arr, NumTasks);
    delete indexer;
}


int main()
{
    Task1(new MutexTaskIndexer());
    Task1(new AtomicTaskIndexer());

}