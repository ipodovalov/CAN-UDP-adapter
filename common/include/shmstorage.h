#ifndef SHMSTORAGE_H
#define SHMSTORAGE_H

#include <string>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <atomic>

#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/sem.h>

#include <time.h>

#include <embtime.h>
#include <elogger.h>

//! Шаблонный класс-хранилище данных в разделяемой памяти (SHM).
/*!
   Класс-хранилище данных в разделяемой памяти. Обеспечивает атомарный доступ к данным.
 */
template <typename T>
class SHMStorage
{
public:
    typedef struct {
        T data;
        sem_t shm_sem;
        std::atomic<uint32_t> update_counter;
        std::atomic<uint32_t> last_update_timestamp;
    } shared_data_t;

    /*!
     * \brief Конструктор класса
     * \param id идентификатор разделяемого ресурса
     */
    SHMStorage(const std::string &id):
        shared_data(NULL), shm_id(id)
    {
        bool isFirst = false;

        if ((shm_fd = shm_open(id.c_str(), O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR)) > 0) {
            isFirst = true;
        } else {
            shm_fd = shm_open(id.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        }

        if (shm_fd == -1) {
            ELOG(ELogger::INFO_SYSTEM, ELogger::LEVEL_ERROR) << "Failed to open SHM region" << id;
            return;
        }

        if (ftruncate(shm_fd, sizeof(shared_data_t)) == -1) {
            ELOG(ELogger::INFO_SYSTEM, ELogger::LEVEL_ERROR) << "Failed to set size of SHM region" << id;
            return;
        }

        shared_data = reinterpret_cast<shared_data_t *>(mmap(NULL, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));

        if (shared_data == MAP_FAILED) {
            ELOG(ELogger::INFO_SYSTEM, ELogger::LEVEL_ERROR) << "Failed to mmap() SHM region" << id;
            return;
        }

        if (isFirst) {
            bzero(shared_data, sizeof(shared_data_t));

            shared_data->update_counter = 0;
            shared_data->last_update_timestamp = 0;

            if (sem_init(&(shared_data->shm_sem), 1, 1) == -1) {
                ELOG(ELogger::INFO_SYSTEM, ELogger::LEVEL_ERROR) << "Failed to init SHM semaphore" << id;
                return;
            }
        }

        ELOG(ELogger::INFO_SYSTEM, ELogger::LEVEL_INFO) << "SHM region" << id << "is ready.";
    }

    ~SHMStorage() {
        if ((shared_data != nullptr) && (shared_data != MAP_FAILED)) {
            munmap(shared_data, sizeof(shared_data_t));
        }

        //sem_destroy(&(shared_data->shm_sem));
        //shm_unlink(shm_id.c_str());
    }

    //! Функция установки нового значения для хранимых данных
    void set(T value) {
        time_t timestamp = time(NULL);

        if (timestamp > UNIX_EPOCH_EMBTIME_OFFSET) {
            timestamp -= UNIX_EPOCH_EMBTIME_OFFSET;
        } else {
            timestamp = 0;
        }

        if (shared_data == NULL) {
            return;
        }

        sem_wait(&(shared_data->shm_sem));
        shared_data->data = value;
        shared_data->update_counter++;
        shared_data->last_update_timestamp = timestamp;
        sem_post(&(shared_data->shm_sem));
    }

    //! Функция получения текущего значения хранимых данных
    T get() {
        T temp;

        if (shared_data != NULL) {
            sem_wait(&(shared_data->shm_sem));
            temp = shared_data->data;
            sem_post(&(shared_data->shm_sem));
        }

        return temp;
    }

    T getDataExt(uint32_t &updateCounter, uint32_t &lastUpdateTimestamp) {
        T temp;

        if (shared_data != NULL) {
            sem_wait(&(shared_data->shm_sem));
            temp = shared_data->data;
            updateCounter = shared_data->update_counter;
            lastUpdateTimestamp = shared_data->last_update_timestamp;
            sem_post(&(shared_data->shm_sem));
        }

        return temp;
    }

    /*!
     * \brief Функция получения значения счётчика обновлений хранимых данных
     * \return текущее значение счётчика обновлений хранимых данных
     */
    uint32_t getUpdateCounter() {
        return shared_data->update_counter;
    }

    /*!
      * \brief Функция получения временной отметки последнего обновления данных
      * \return временная отметка последнего обновления данных
      */
    uint32_t getLastUpdateTimestamp() {
        return shared_data->last_update_timestamp;
    }

private:
    shared_data_t *shared_data;
    int shm_fd;
    std::string shm_id;


};

#endif // SHMSTORAGE_H
