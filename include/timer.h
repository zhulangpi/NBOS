#if !defined(_TIMER_H)
#define  _TIMER_H 

extern void timer_init(void);
extern void timer_handler(void);

extern unsigned long long jiffies;

#endif  /* _TIMER_H  */
