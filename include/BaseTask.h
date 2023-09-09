#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

class BaseTask
{
public:
  BaseTask(const char *name, UBaseType_t priority = 5, int stackSize = 4096, BaseType_t xCoreId = 0)
      : _name(name),
        _priority(priority),
        _stackSize(stackSize),
        _xCoreId(xCoreId)
  {
  }

  ~BaseTask()
  {
    if (task_handle == nullptr)
      return;
    vTaskDelete(task_handle);
  }

  bool Start()
  {
    xTaskCreatePinnedToCore(Handler, _name, _stackSize, this, _priority, &task_handle, _xCoreId);
    return true;
  }

  virtual void Startup() {}
  virtual void Loop() { Sleep(1000); }

protected:
  const char *_name;
  const UBaseType_t _priority;
  const uint32_t _stackSize;
  const BaseType_t _xCoreId;

  /// @brief sleep ms
  /// @param xDelay
  void Sleep(const TickType_t xDelay)
  {
    vTaskDelay(pdMS_TO_TICKS(xDelay));
  }

private:
  TaskHandle_t task_handle = nullptr;

  static void Handler(void *arg)
  {
    BaseTask *task = (BaseTask *)arg;
    task->Startup();
    while (true)
      task->Loop();
  }
};
