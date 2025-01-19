//
// Created by liyu on 12/23/23.
//

#include "vector/TimestampColumnVector.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <chrono>
TimestampColumnVector::TimestampColumnVector(int precision, bool encoding): ColumnVector(VectorizedRowBatch::DEFAULT_SIZE, encoding) {
    TimestampColumnVector(VectorizedRowBatch::DEFAULT_SIZE, precision, encoding);
}

TimestampColumnVector::TimestampColumnVector(uint64_t len, int precision, bool encoding): ColumnVector(len, encoding) {
    this->precision = precision;
    if(encoding) {
        posix_memalign(reinterpret_cast<void **>(&this->times), 64,
                       len * sizeof(long));
    } else {
        this->times = nullptr;
    }
}


void TimestampColumnVector::close() {
    if(!closed) {
        ColumnVector::close();
        if(encoding && this->times != nullptr) {
            free(this->times);
        }
        this->times = nullptr;
    }
}

void TimestampColumnVector::print(int rowCount) {
    throw InvalidArgumentException("not support print longcolumnvector.");
//    for(int i = 0; i < rowCount; i++) {
//        std::cout<<longVector[i]<<std::endl;
//		std::cout<<intVector[i]<<std::endl;
//    }
}

TimestampColumnVector::~TimestampColumnVector() {
    if(!closed) {
        TimestampColumnVector::close();
    }
}

void * TimestampColumnVector::current() {
    if(this->times == nullptr) {
        return nullptr;
    } else {
        return this->times + readIndex;
    }
}

/**
     * Set a row from a value, which is the days from 1970-1-1 UTC.
     * We assume the entry has already been isRepeated adjusted.
     *
     * @param elementNum
     * @param days
 */
void TimestampColumnVector::set(int elementNum, long ts) {
    if(elementNum >= writeIndex) {
        writeIndex = elementNum + 1;
    }
    times[elementNum] = ts;
    // TODO: isNull
}


/*function created by dhl as lab2*/
// void TimestampColumnVector::add(long value){
// 	/*这里value的意思是自1970年一月一日以来的ms数*/
//     if (writeIndex >= length) {
//         ensureSize(writeIndex * 2, true);
//     }
//     int index = writeIndex++;
// 	times[index]=value;
//     isNull[index] = false;
// }
void TimestampColumnVector::ensureSize(uint64_t size, bool preserveData) {
    printf("length: %d, size: %d\n", length, size);
    if(times == nullptr) {
        posix_memalign(reinterpret_cast<void **>(&times), 32,size * sizeof(long));
        memoryUsage += (long) sizeof(long) * size;
        length = size;
    }
    if (length < size) {
        printf("length < size\n");
        if (1) {
            long *oldVector = times;
            posix_memalign(reinterpret_cast<void **>(&times), 32,
                           size * sizeof(long));
            printf("times: %p\n", times);
            if (preserveData) {
                std::copy(oldVector, oldVector + length, times);
            }
            delete[] oldVector;

            memoryUsage += (long) sizeof(long) * (size - length);
            length = size;
            // resize(size);
        } 
    }
}
void TimestampColumnVector::add(long value){
    
	/*这里value的意思应该是没加浮点的小数？*/
    if (writeIndex >= length || writeIndex==0) {
        if(writeIndex==0){
            printf("ensureSize(2, true)\n");
            ensureSize(4, true);
        }
        else{
            ensureSize(writeIndex * 2, true);
        }
    }
    printf("ensureSize success, times size: %d, write index: %d\n", length, writeIndex);
    int index = writeIndex++;
    printf("writeIndex++ success\n");
    printf("add value: %ld\n", value);
	times[index]=value;
    printf("add2vector success\n");
    isNull[index] = false;
}
void TimestampColumnVector::add(std::string& value) {
    int ts = dateToTimestamp(value);
    add(ts);
}

long TimestampColumnVector::dateToTimestamp(std::string& dateStr) {
    std::tm tm = {};
    std::stringstream ss(dateStr);
    
    // 解析字符串为日期和时间
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        std::cerr << "日期解析失败" << std::endl;
        return -1;
    }

    // 使用 std::chrono 获取高精度的时间戳（毫秒）
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    
    // 获取当前时间点的毫秒级时间戳
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();

    return static_cast<long>(ms)*1000;
}