#ifndef EMBTIME_H_
#define EMBTIME_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//! Формат представления времени, который предствялет из себя количество секунд, прошедших с 2010 01-01 00:00:00 GMT
typedef uint32_t embtime_t;

typedef enum {
	RTC_STATE_FAIL		=	0x00,
	RTC_STATE_OK		=	0x01
} RTC_STATE;

//! Составной тип представления даты и времени
typedef struct {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	// Year starting from 2000
	uint8_t year;
} emb_time_struct_t;

//! Константа, на которую различается UNIX time от embtime_t
#define UNIX_EPOCH_EMBTIME_OFFSET	(1262304000ULL)

// Number of seconds from 2010 01-01 00:00:00 GMT, working starting from 2010
/*!
 * \brief Перевод времени из составного формата в количество секунд, прошедших с 2010 01-01 00:00:00 GMT (embtime_t)
 * \param ts исходные данные в составном формате для преобразования
 * \return результат преобразования - время в формате \sa embtime_t
 */
extern embtime_t time_to_epoch(const emb_time_struct_t *ts);

/*!
 * \brief Перевод времени из формата embtime_t (аналог UNIX TIME) в составной формат
 * \param ts время в составном формате (результат преобразования)
 * \param time время в секундах начиная с 2010 01-01 00:00:00 GMT (исходные данные для преобразования)
 * \return результат преобразования
 */
extern emb_time_struct_t * epoch_to_time(emb_time_struct_t *ts, embtime_t time);

//! Формирование структуры для храниения данных в составном формате
extern emb_time_struct_t * ts_construct(emb_time_struct_t *ts, uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t month, uint8_t year);


#ifdef __cplusplus
}
#endif

#endif /* EMBTIME_H_ */
