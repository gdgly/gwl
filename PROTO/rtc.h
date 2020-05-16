 
#ifndef _LINUX_RTC_H_
#define _LINUX_RTC_H_






int read_time(struct rtc_time *rtc_tm);
int setup_time(struct rtc_time* rtc_tm);



#endif /* _LINUX_RTC_H_ */
