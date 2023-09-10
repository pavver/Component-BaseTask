#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

class BaseTask
{
public:
  /**
   * @brief Create a Task context
   *
   * @param name A descriptive name for the task. This is mainly used to facilitate debugging. Max length defined by configMAX_TASK_NAME_LEN - default is 16.
   * @param priority The priority at which the task should run. mode by setting bit portPRIVILEGE_BIT of the priority parameter. For example,
   *  to create a privileged task at priority 2 the uxPriority parameter should be set to ( 2 | portPRIVILEGE_BIT ).
   * @param stackSize The size of the task stack specified as the number of bytes. Note that this differs from vanilla FreeRTOS.
   * @param xCoreId If the value is tskNO_AFFINITY, the created task is not pinned to any CPU, and the scheduler can run it on any core available.
   * Values 0 or 1 indicate the index number of the CPU which the task should be pinned to.
   * Specifying values larger than (portNUM_PROCESSORS - 1) will cause the to fail.
   */
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

  /**
   * @brief Create/Start system Task. Invoke Startup and Loop
   *
   * @return true the task was successfully created and added to a ready list
   * @return false
   */
  bool Start()
  {
    BaseType_t err = xTaskCreatePinnedToCore(Handler, _name, _stackSize, this, _priority, &task_handle, _xCoreId);
    return err == pdPASS;
  }

  /**
   * @brief The function is called on startup. Use it to initialize variables, bind routines, initialize libraries, and more. The function will run only once
   */
  virtual void Startup() {}

  /**
   * @brief After creating a startup function that initializes and sets the initial values, the function does
   * exactly what its name suggests and executes a sequential loop, allowing your program to change and react.
   * Be sure to call the Sleep function to avoid triggering the task watchdog.
   */
  virtual void Loop() { Sleep(1000); }

protected:
  const char *_name;
  const UBaseType_t _priority;
  const uint32_t _stackSize;
  const BaseType_t _xCoreId;

  /**
   * @brief Pauses the program for the amount of time (in milliseconds) specified as parameter. (There are 1000 milliseconds in a second.)
   * @param xDelay the number of milliseconds to pause.
   */
  void Sleep(const TickType_t xDelay = 10)
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
