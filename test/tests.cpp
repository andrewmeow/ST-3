// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

// Фикстура тестов для ST3
class ST3 : public ::testing::Test {
 protected:
  TimedDoor *door;
  MockTimerClient *mockClient;

  void SetUp() override {
    door = new TimedDoor(100); // Таймаут 100 мс
    mockClient = new MockTimerClient();
  }

  void TearDown() override {
    delete door;
    delete mockClient;
  }
};

// Проверка начального состояния двери (она должна быть закрыта)
TEST_F(ST3, InitialState) { EXPECT_FALSE(door->isDoorOpened()); }

// Проверка открытия двери
TEST_F(ST3, OpenDoor) {
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

// Проверка закрытия двери
TEST_F(ST3, CloseDoor) {
  door->unlock();
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}

// Проверка, что выбрасывается исключение при оставленной открытой двери
TEST_F(ST3, ExceptionOnTimeout) {
  door->unlock();
  std::this_thread::sleep_for(
      std::chrono::milliseconds(150)); // Ждем больше таймаута
  EXPECT_THROW(door->throwState(), std::runtime_error);
}

// Проверка, что нет исключения, если дверь была закрыта вовремя
TEST_F(ST3, NoExceptionWhenClosed) {
  door->unlock();
  door->lock();
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  EXPECT_NO_THROW(door->throwState());
}

// Проверка таймаута с мок-объектом
TEST_F(ST3, TimerTriggersTimeout) {
  Timer timer;
  EXPECT_CALL(*mockClient, Timeout()).Times(1);
  timer.tregister(100, mockClient);
}

// Проверка работы DoorTimerAdapter
TEST_F(ST3, AdapterTriggersTimeout) {
  DoorTimerAdapter adapter(*door);
  EXPECT_THROW(adapter.Timeout(), std::runtime_error);
}

// Проверка метода getTimeOut
TEST_F(ST3, GetTimeoutValue) { EXPECT_EQ(door->getTimeOut(), 100); }

// Проверка повторного открытия двери после закрытия
TEST_F(ST3, ReopenAfterClose) {
  door->unlock();
  door->lock();
  door->unlock();
  EXPECT_TRUE(door->isDoorOpened());
}

// Проверка повторного закрытия двери (должна оставаться закрытой)
TEST_F(ST3, RecloseDoor) {
  door->lock();
  EXPECT_FALSE(door->isDoorOpened());
}
