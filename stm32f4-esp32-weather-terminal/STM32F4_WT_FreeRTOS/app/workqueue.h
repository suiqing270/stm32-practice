#ifndef __WORKQUEUE_H__
#define __WORKQUEUE_H__

//定义函数指针类型 work_t
//统一规范所有要放入队列执行的函数格式
typedef void (*work_t)(void *param);

void workqueue_init(void);

void workqueue_run(work_t work, void *param);

#endif // __WORKQUEUE_H__
