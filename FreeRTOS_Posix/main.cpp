
/* System headers. */

extern "C" {

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <unistd.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "partest.h"
#include "semphr.h"

/* Demo file headers. */
#include "BlockQ.h"
#include "PollQ.h"
#include "death.h"
#include "crflash.h"
#include "flop.h"
#include "print.h"
#include "fileIO.h"
#include "semtest.h"
#include "integer.h"
#include "dynamic.h"
#include "mevents.h"
#include "crhook.h"
#include "blocktim.h"
#include "GenQTest.h"
#include "QPeek.h"
#include "countsem.h"
#include "recmutex.h"

#include "AsyncIO/AsyncIO.h"
#include "AsyncIO/AsyncIOSocket.h"
#include "AsyncIO/PosixMessageQueueIPC.h"
#include "AsyncIO/AsyncIOSerial.h"


void vApplicationIdleHook( void );
}

#include <string>
#include <cstring>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <array>
#include <limits>
#include <atomic>
#include <cstdint>


using namespace std;


static unsigned long uxQueueSendPassedCount = 0;
void vMainQueueSendPassed( void )
{
    /* This is just an example implementation of the "queue send" trace hook. */
    uxQueueSendPassedCount++;
}


void vApplicationIdleHook( void )
{
    vCoRoutineSchedule();   /* Comment this out if not using Co-routines. */
}


class SynchroObjectDummy {

    SynchroObjectDummy() {};

public:

    static inline void get() {
    }

    static inline void release() {
    }

};// class SynchroObjectDummy

template<typename Mutex> class Lock {

public:
    inline Lock() {
        Mutex::get();
    }

    inline ~Lock() {
        Mutex::release();
    }
};// class Lock

/**
 * Instantiate a new type - lock which does nothing
 */
typedef Lock<SynchroObjectDummy> LockDummy;

class StackBase {

public:
    inline bool isEmpty() {
        bool res = (this->top == 0);
        return res;
    }

    inline bool isFull() {
        bool res = (this->top == size);
        return res;
    }

protected:
    StackBase(size_t size) {
        this->size = size;
        this->top = 0;
    }

    void errorOverflow() {
    }

    void errorUnderflow() {
    }

    size_t top;
    size_t size;

};// StackBase

template<typename ObjectType, typename Lock, std::size_t Size>
class Stack: public StackBase {
public:

    Stack() :
        StackBase(Size) {
    }

    ~Stack() {
    }

    inline bool push(ObjectType* object);
    inline bool pop(ObjectType** object);

private:

    ObjectType* data[Size + 1];
};// class Stack

template<typename ObjectType, typename Lock, std::size_t Size>
inline bool Stack<ObjectType, Lock, Size>::push(ObjectType* object) {
    Lock();
    if (!isFull()) {
        data[this->top] = object;
        this->top++;
        return true;
    } else {
        errorOverflow();
        return false;
    }

}

template<typename ObjectType, typename Lock, std::size_t Size>
inline bool Stack<ObjectType, Lock, Size>::pop(ObjectType** object) {
    Lock();
    if (!isEmpty()) {
        this->top--;
        *object = (data[this->top]);
        return true;
    } else {
        errorUnderflow();
        return false;
    }
}

template<typename Lock, typename ObjectType, size_t Size> class MemoryPool {

public:

    MemoryPool() {
        for (int i = 0;i < Size;i++) {
            pool.push(&objects[i]);
        }
    }

    ~MemoryPool() {}

    inline bool allocate(ObjectType **obj) {
        bool res;
        Lock();
        res = pool.pop(obj);
        return res;
    }

    inline bool free(ObjectType *obj) {
        bool res;
        Lock();
        res = pool.push(obj);
        return res;
    }

protected:
    Stack<ObjectType, LockDummy,  Size> pool;
    ObjectType objects[Size];
};


class JobThread {
public:

    JobThread();

    typedef void (*jobPtr)(void);
    void start(jobPtr job);

protected:

    jobPtr job;
    xTaskHandle pxCreatedTask;
    xQueueHandle signal;

    static void mainLoop(JobThread *jobThread);
};

JobThread::JobThread() : job(nullptr) {
    static const char *name = "a job";
    vSemaphoreCreateBinary(this->signal);
    portBASE_TYPE res = xTaskCreate((pdTASK_CODE)mainLoop, (const signed char *)name, 300, this, 1, &this->pxCreatedTask);
    if (res != pdPASS) {
        cout << "Failed to create a task" << endl;
    }
}

void JobThread::start(jobPtr job) {
    this->job = job;
    xSemaphoreGive(this->signal);
}

void JobThread::mainLoop(JobThread *jobThread) {
    while (true) {
        xSemaphoreTake(jobThread->signal, portMAX_DELAY);
        if (jobThread->job != nullptr) {
            jobThread->job();
        }
        jobThread->job = nullptr;
    }
}



MemoryPool<LockDummy, JobThread, 3> jobThreads;
void myPrintJob() {
    cout << "Print job is running" << endl;
}

int main( void )
{

    JobThread *jobThread;
    jobThreads.allocate(&jobThread);
    jobThread->start(myPrintJob);

    vTaskStartScheduler();

	return 1;
}
