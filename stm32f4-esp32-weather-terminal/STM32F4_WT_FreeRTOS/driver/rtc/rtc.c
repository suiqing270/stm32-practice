//初始化 RTC、把网络 SNTP 时间写入 RTC、从 RTC 可靠读取时间
#include <string.h>
#include "stm32f4xx.h"
#include "rtc.h"

void rtc_init(void)
{
    RTC_InitTypeDef RTC_InitStruct;
    RTC_StructInit(&RTC_InitStruct);
    RTC_Init(&RTC_InitStruct);
    
    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForSynchro();
}
//把 SNTP 得到的时间，写入 RTC（一次，不做校验）
static void _rtc_set_time_once(const rtc_date_time_t *date_time)
{
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
    
    RTC_DateStructInit(&date);
    RTC_TimeStructInit(&time);
    //把date_time中的信息传递给date和time
    date.RTC_Year = date_time->year - 2000;
    date.RTC_Month = date_time->month;
    date.RTC_Date = date_time->day;
    date.RTC_WeekDay = date_time->weekday;
    time.RTC_Hours = date_time->hour;
    time.RTC_Minutes = date_time->minute;
    time.RTC_Seconds = date_time->second;
    //把date和time中的信息传递给RTC
    // 执行硬件写入：把时间写入 RTC 寄存器
    RTC_SetDate(RTC_Format_BIN, &date);
    RTC_SetTime(RTC_Format_BIN, &time);
    
}
//从 RTC 读一次时间，原样塞进 date_time
static void _rtc_get_time_once(rtc_date_time_t *date_time)
{
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
    
    RTC_DateStructInit(&date);
    RTC_TimeStructInit(&time);
    //获取当前的RTC时间，并存入date和time
    RTC_GetDate(RTC_Format_BIN, &date);
    RTC_GetTime(RTC_Format_BIN, &time);
    //将获取到的RTC时间存入传入的date_time
    date_time->year = 2000 + date.RTC_Year;
    date_time->month = date.RTC_Month;
    date_time->day = date.RTC_Date;
    date_time->weekday = date.RTC_WeekDay;
    date_time->hour = time.RTC_Hours;
    date_time->minute = time.RTC_Minutes;
    date_time->second = time.RTC_Seconds;
}
//date_time:通过esp32获取到的stnp时间
//将获取到的sntp时间传递给RTC
void rtc_set_time(const rtc_date_time_t *date_time)
{
    rtc_date_time_t rtime;
    do {
        //通过esp32获取到的stnp时间设置为RTC时间
        _rtc_set_time_once(date_time);
        //获取刚才设置的RTC时间，并存入rtime
        _rtc_get_time_once(&rtime);
    } while (date_time->second != rtime.second);
    //确保 RTC 的“秒”至少和 SNTP 时间对齐一次
}
//连续读两次 RTC
//只有两次完全一致，才认为这次读是可靠的
/*RTC 是异步时钟，和 CPU 时钟不同步；
读取时，RTC 可能刚好进位（比如 59 秒 → 00 秒），导致读到一半旧时间、一半新时间（数据错乱）；
连续读两次，完全一致才说明读取时没有发生进位，数据有效。*/

void rtc_get_time(rtc_date_time_t *date_time)
{
    rtc_date_time_t time1, time2;
    do {
        _rtc_get_time_once(&time1);
        _rtc_get_time_once(&time2);
    } while (memcmp(&time1, &time2, sizeof(rtc_date_time_t)) != 0);
    
    memcpy(date_time, &time1, sizeof(rtc_date_time_t));
}
