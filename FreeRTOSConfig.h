#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define configUSE_PREEMPTION                        1
#define configUSE_TICKLESS_IDLE                     0
#define configCPU_CLOCK_HZ                          ( F_CPU )
#define configSYSTICK_CLOCK_HZ                      ( 100000UL )
#define configTICK_RATE_HZ                          ( (TickType_t) 1000 )
#define configUSE_PORT_OPTIMISED_TASK_SELECTION     1
#define configMAX_PRIORITIES                        ( 10 )
#define configMINIMAL_STACK_SIZE                    ( ( unsigned short ) 128 )
#define configMAX_TASK_NAME_LEN                     ( 10 )
#define configUSE_16_BIT_TICKS                      0
#define configIDLE_SHOULD_YIELD                     1
#define configUSE_TASK_NOTIFICATIONS                1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES       4
#define configUSE_MUTEXES                           1
#define configUSE_RECURSIVE_MUTEXES                 1
#define configUSE_COUNTING_SEMAPHORES               1
#define configQUEUE_REGISTRY_SIZE                   0
#define configUSE_QUEUE_SETS                        0
#define configUSE_TIME_SLICING                      0
#define configUSE_NEWLIB_REENTRANT                  1
#define configENABLE_BACKWARD_COMPATIBILITY         0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS     4
#define configUSE_APPLICATION_TASK_TAG              0

#define configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H   1

#define configSUPPORT_STATIC_ALLOCATION             1
#define configSUPPORT_DYNAMIC_ALLOCATION            1
#define configAPPLICATION_ALLOCATED_HEAP            1   //힙 메모리를 유저가 직접관리 "1"
#define configTOTAL_HEAP_SIZE                       ( ( size_t ) ( 256 * 1024 ) )

#define configUSE_IDLE_HOOK                         0
#define configUSE_TICK_HOOK                         0
#define configCHECK_FOR_STACK_OVERFLOW              2
#define configUSE_MALLOC_FAILED_HOOK                1
#define configUSE_DAEMON_TASK_STARTUP_HOOK          0

#define configGENERATE_RUN_TIME_STATS               1
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE()            freertos_get_us()
#define configUSE_TRACE_FACILITY                    1
#define configUSE_STATS_FORMATTING_FUNCTIONS        0

#define configRECORD_STACK_HIGH_ADDRESS             1

#define configUSE_CO_ROUTINES                       0
#define configMAX_CO_ROUTINE_PRIORITIES             ( 2 )

#define configUSE_TIMERS                            1
#define configTIMER_TASK_PRIORITY                   ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH                    10
#ifndef configTIMER_TASK_STACK_DEPTH
#define configTIMER_TASK_STACK_DEPTH                ( 1536U / 4U )
#endif
#define configIDLE_TASK_NAME                        "IDLE"

#ifdef NDEBUG
#define configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES   0
#define configASSERT(condition) ((void) 0)
#define putchar_debug(...)
#define printf_debug(...)
#define ASSERT_LOG(...)
#else
#define configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES   1
#ifdef __cplusplus
extern "C" {
#endif
void assert_blink(const char*, int, const char*, const char*) __attribute__((noreturn));
#ifdef __cplusplus
}
#define ASSERT_LOG(_msg) assert_blink("", __LINE__, __PRETTY_FUNCTION__, #_msg);
#else
#if defined ARDUINO_TEENSY40 || defined ARDUINO_TEENSY41
#define PROGMEM_FREERTOS __attribute__((section(".progmem")))
#else
#define PROGMEM_FREERTOS
#endif
#define ASSERT_LOG(_msg)                                                            \
    {                                                                               \
        static const char _file_[] PROGMEM_FREERTOS = __FILE__;                     \
        assert_blink((const char*) _file_, __LINE__, __PRETTY_FUNCTION__, #_msg);   \
    }
#endif // __cplusplus
#define configASSERT(_e)               \
    if (__builtin_expect(!!(_e), 1)) { \
        (void) 0;                      \
    } else {                           \
        ASSERT_LOG(_e);                \
    }
#ifdef PRINT_DEBUG_STUFF
void putchar_debug(char);
void printf_debug(const char*, ...);
#else
#define putchar_debug(...)
#define printf_debug(...)
#endif // PRINT_DEBUG_STUFF
#endif // NDEBUG

#if configGENERATE_RUN_TIME_STATS == 1
uint64_t freertos_get_us(void);
#endif

#define INCLUDE_vTaskPrioritySet                    1
#define INCLUDE_uxTaskPriorityGet                   1
#define INCLUDE_vTaskDelete                         1
#define INCLUDE_vTaskCleanUpResources               1
#define INCLUDE_vTaskSuspend                        1
#define INCLUDE_xTaskDelayUntil                     1
#define INCLUDE_vTaskDelay                          1
#define INCLUDE_eTaskGetState                       1
#define INCLUDE_xTimerPendFunctionCall              1
#define INCLUDE_xSemaphoreGetMutexHolder            0
#define INCLUDE_xTaskGetSchedulerState              1
#define INCLUDE_xTaskGetCurrentTaskHandle           1
#define INCLUDE_uxTaskGetStackHighWaterMark         1
#define INCLUDE_xTaskGetIdleTaskHandle              1
#define INCLUDE_eTaskGetState                       1
#define INCLUDE_xTaskAbortDelay                     1
#define INCLUDE_xTaskGetHandle                      1
#define INCLUDE_xTaskResumeFromISR                  1

#ifdef __NVIC_PRIO_BITS
	#define configPRIO_BITS                         __NVIC_PRIO_BITS
#else
	#define configPRIO_BITS                         4
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY     ( ( 1U << ( configPRIO_BITS ) ) - 1 )
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    2

#define configKERNEL_INTERRUPT_PRIORITY             ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY        ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )

#define configUSE_GCC_BUILTIN_ATOMICS               1

#ifdef __cplusplus
}
#endif

#if defined(__has_include) && __has_include("freertos_config_override.h")
#include "freertos_config_override.h"
#endif

#endif /* FREERTOS_CONFIG_H */