// NOLINTBEGIN
#include "gtest/gtest.h"
#include <thread>
// NOLINTEND
#include "AsyncQueue.h"

class AsyncQueueTest : public::testing::Test {
public:
	AsyncQueue<int> queue{};
protected:
	void SetUp() override {}
	void TearDown() override {}
};

TEST_F(AsyncQueueTest, Emplace) {
	const int input = 10;
	queue.Emplace(input);
	int ret = queue.Pop();
	EXPECT_EQ(input, ret);
}

TEST_F(AsyncQueueTest, LeftValueReferencePush) {
	const int input = 10;
	queue.Push(input);
	int ret = queue.Pop();
	EXPECT_EQ(input, ret);
}

TEST_F(AsyncQueueTest, RightValueReferencePush) {
	const int input = 10;
	queue.Push(std::move(input));
	int ret = queue.Pop();
	EXPECT_EQ(input, ret);
}

TEST_F(AsyncQueueTest, TimedOutPop) {
	int val = 0;
	EXPECT_EQ(false, queue.TimedPop(&val, std::chrono::milliseconds(1000)));
}

TEST_F(AsyncQueueTest, UseThread1) {
	const int input = 10;
	int ret = 0;
	bool timeout = false;
	std::thread t1{ [&] {
		timeout = queue.TimedPop(&ret, std::chrono::milliseconds(1000));
	} };
	std::thread t2{ [&] {
		queue.Push(input);
	} };
	t1.join();
	t2.join();
	ASSERT_EQ(true, timeout);
	EXPECT_EQ(input, ret);
}

TEST_F(AsyncQueueTest, UseThread2) {
	const int input = 0;
	int val = 0;
	bool ret = false;
	std::thread t1{ [&] {
		queue.Push(input);
	} };
	std::thread t2{ [&] {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		ret = queue.TimedPop(&val, std::chrono::seconds(1));
	} };
	t1.join();
	t2.join();
	ASSERT_EQ(true, ret);
	EXPECT_EQ(input, val);
}
