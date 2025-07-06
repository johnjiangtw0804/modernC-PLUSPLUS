#include <thread>
#include <vector>
#include <iostream>
#include <functional>
#include <mutex>
#include <condition_variable>


using namespace std;
void test(int X) {
    cout << "Hello from thread!" << endl;
    cout << "Argument passed in: " << X << endl;
}

static int sharedValue = 0;
mutex gMyLock;
void incrementSharedValue()
{
    gMyLock.lock();
        sharedValue += 1;
    gMyLock.unlock();
}


static int sharedValue2 = 0;
mutex gMyLock2;
void incrementSharedValue_uniqueLock()
{
    unique_lock<mutex> unique_lock(gMyLock2);
        sharedValue2 += 1;
}

static atomic<int> sharedValue3 = 0;
void incrementSharedValue_atomic()
{
    sharedValue3++; // be careful, some operator is not overloaded
}

static queue<int> sharedQueue;
static int FULLSIZE = 5;
static condition_variable isFull;
static condition_variable isEmpty;
mutex gM;

int main()
{
    // https://en.cppreference.com/w/cpp/thread/thread/thread.html
    auto lambda = [](int X)
    {
        cout << "Hello from thread!" << endl;
        cout << "Argument passed in: " << X << endl;
    };

    thread myThread(lambda, 100);
    thread myThread2(test, 100);
    myThread.join();
    myThread2.join();

    cout << "----------------------multi-threaded------------------------" << endl;

    vector<thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.push_back(thread([](int X){cout << "Hello from threadID: " << (this_thread::get_id()) << endl;
                                           cout << "Argument passed in: " << X << endl;}, i));
    }
    for (thread &t : threads) {
        t.join();
    }

    cout << "------------------jThread----------------------------" << endl;
    // need to be carful because this will automatically when it has to
    vector<jthread> jThreads;
    for (int i = 0; i < 10; i++)
    {
        jThreads.push_back(jthread([](int X)
                                   {cout << "Hello from jthreadID: " << (this_thread::get_id()) << endl;
                                           cout << "Argument passed in: " << X << endl; }, i));
    }

    cout << "------------------Shared Resource----------------------------" << endl;
    // force jthread out of scope
    {
        vector<jthread> jthreads2;
        for (int i = 0; i < 100; i++)
        {
            jthreads2.push_back(jthread(incrementSharedValue));
        }
    }


    cout << "Hello from my main thread" << endl;
    cout << "shared value: " << sharedValue << endl;

    cout << "------------------Lock Guard (RAII)----------------------------" << endl;
    // 它的用途是：一旦建立，就馬上幫你 lock()，出了 scope 就自動 unlock()。
    {
        vector<jthread> jthreads2;
        for (int i = 0; i < 100; i++)
        {
            jthreads2.push_back(jthread(incrementSharedValue_uniqueLock));
        }
    }
    cout << "shared value2: " << sharedValue2 << endl;

    cout << "------------------Atomic----------------------------" << endl;
    {
        vector<jthread> jthreads3;
        for (int i = 0; i < 100; i++)
        {
            jthreads3.push_back(jthread(incrementSharedValue_atomic));
        }
    }

    cout << "------------------conditional variable----------------------------" << endl;
    // we have a pub-sub
    function<void()> producer = [&](){
        // produce if not full
        for (int i = 0; i < 5; i++) {
            {
                unique_lock lock(gM);
                isFull.wait(lock, [](){
                    return sharedQueue.size() < FULLSIZE;
                });
                int newData = rand() % 5;
                cout << "adding new data " << newData << endl;
                sharedQueue.push(newData);
                isEmpty.notify_one();
            }
        }
    };
    function<void()> consumer = [&](){
        for (int i = 0; i < 5; i++) {
            {
                unique_lock lock(gM);
                isEmpty.wait(lock, []() {
                    return sharedQueue.size() > 0;
                });
                int consumedData = sharedQueue.front(); sharedQueue.pop();
                cout << "removing new data " << consumedData << endl;

                // remember to notify
                isFull.notify_one();
            }
        }
    };

    vector<jthread> producers;
    for (int i = 0; i < 5; i ++) {
        producers.emplace_back(producer);
    }
    vector<jthread> consumers;
    for (int i = 0; i < 5; i++)
    {
        consumers.emplace_back(consumer);
    }

    return 0;
}
