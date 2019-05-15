#include <embtime.h>
#include <time.h>
#include <string.h>

#ifdef __GNUC__
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif


//! Количество дней года на начало месяца
const uint16_t mon_days [] =      { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

embtime_t time_to_epoch(const emb_time_struct_t *ts) {
	uint8_t tyears, leaps;
	uint16_t tdays;

    if (unlikely(ts->year < 10)) {
		return 0;
	}

	tyears = ts->year - 10; // tm->tm_year is from 2010.
	leaps = (tyears + 2) / 4; // no of next two lines until year 2100.
	//i = (tyears - 100) / 100;
	//leaps -= ( (i/4)*3 + i%4 );

    if (unlikely(ts->month == 0)) {
		tdays = mon_days[0];
	} else {
		tdays = mon_days[ts->month - 1];
	}

    if (likely(ts->day != 0)) {
		tdays += ts->day - 1; // days of month passed.
	}

	tdays = tdays + (tyears * 365) + leaps;

	return (tdays * 86400) + (ts->hour * 3600) + (ts->min * 60) + ts->sec;
}

emb_time_struct_t * epoch_to_time(emb_time_struct_t *ts, embtime_t time) {
	time_t unix_time = time+UNIX_EPOCH_EMBTIME_OFFSET;
	struct tm * local ;

	ts->sec = time % 60;
	time -= ts->sec;
	ts->min = (time / 60) % 60;
	time -= ts->min * 60;
	ts->hour = (time / 3600) % 24;
	time -= ts->hour * 3600;

	local = localtime(&unix_time);
	ts->year = ((local->tm_year)+1900)-2000;
	ts->month = (local->tm_mon)+1;
	ts->day = local->tm_mday;

	return ts;
}

emb_time_struct_t * ts_construct(emb_time_struct_t *ts, uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t month, uint8_t year) {
	bzero(ts, sizeof(emb_time_struct_t));

    if (unlikely(sec >= 60)) {
		ts->sec = 59;
	} else {
		ts->sec = sec;
	}

    if (unlikely(min >= 60)) {
		ts->min = 59;
	} else {
		ts->min = min;
	}

	if (unlikely(hour >= 24)) {
		ts->hour = 23;
	} else {
		ts->hour = hour;
	}

	if (unlikely(day == 0)) {
		ts->day = 1;
	} else {
		ts->day = day;
	}

	if (unlikely(month == 0)) {
		ts->month = 1;
	} else {
		ts->month = month;
	}

	ts->year = year;

	return ts;
}


