#include <gtest/gtest.h>
#include "Queue.h"

#define TEST_LOG "test.log"

class FriendlyQueue : public Queue
{
    public:
        FriendlyQueue() : Queue(TEST_LOG)
        {
        }

        int GetMessageCount() {
            return _message_count;
        }

        char* GetQueue()
        {
            return _queue;
        }

};

TEST(QueueTest, QueueMsgCountTest)
{
    FriendlyQueue f_queue;
    EXPECT_EQ(f_queue.GetMessageCount(), 0);
}

TEST(QueueTest, QueueVerifyCleanBuffer)
{
    FriendlyQueue f_queue;

    char* queue = f_queue.GetQueue();

    bool all_zero = true;

    for (int i = 0; i < QUEUE_LEN; ++i) {
        if (queue[i] != 0) {
            all_zero = false;
        }
    }

    EXPECT_EQ(all_zero, true);
}

TEST(QueueTest, QueueVerifyQueueAppend)
{
    FriendlyQueue f_queue;

    char msg[100] = "Hello";

    f_queue.append(msg);

    EXPECT_EQ(f_queue.GetMessageCount() - 1, strlen("Hello")); 
}

TEST(QueueTest, QueueVerifyFlush)
{
    FriendlyQueue f_queue;
    
    f_queue.append("Hello");

    EXPECT_EQ(f_queue.GetMessageCount() - 1, strlen("Hello")); 

    f_queue.flush();

    EXPECT_EQ(f_queue.GetMessageCount(), 0);
}

