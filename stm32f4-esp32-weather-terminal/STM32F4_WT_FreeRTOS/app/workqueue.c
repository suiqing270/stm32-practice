#include "workqueue.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdint.h>

//工作消息结构体
typedef struct 
{
    work_t work;
    void* param;
} work_message_t;
//所有要异步执行的函数 都会先放进这个队列排队
static QueueHandle_t work_msg_queue;

//队列有消息 =>取出并调用函数指针执行任务

static void work_func(void* param)
{
    work_message_t msg;
    while(1)
    {
        //1.阻塞等待队列有消息 没消息就休眠 不占用CPU
        xQueueReceive(work_msg_queue, &msg, portMAX_DELAY);
        //2.取出消息 执行消息中的函数 并将消息中的参数传入要执行的函数
        msg.work(msg.param);
    }
}

//系统初始化时调用一次 完成队列创建+工作线程启动
void workqueue_init(void)
{
    //1.创建队列 长16 每个元素大小= 工作消息结构体大小
    work_msg_queue = xQueueCreate(16, sizeof(work_message_t));
    configASSERT(work_msg_queue);
    //2.创建工作线程 绑定work_func
    xTaskCreate(work_func, "work_queue", 1024, NULL, 5, NULL);
}

/*  对外接口
    调用这个函数 把任意函数丢进工作队列异步执行
    所有任务在同一个线程执行 无竞争 无需加锁*/
void workqueue_run(work_t work,void *param)
{
    configASSERT(work_msg_queue);
    work_message_t msg = {work, param};
    xQueueSend(work_msg_queue, &msg, portMAX_DELAY);
}
