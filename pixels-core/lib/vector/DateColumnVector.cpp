//
// Created by yuly on 06.04.23.
//

#include "vector/DateColumnVector.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
DateColumnVector::DateColumnVector(uint64_t len, bool encoding): ColumnVector(len, encoding) {
	if(encoding) {
        posix_memalign(reinterpret_cast<void **>(&dates), 32,
                       len * sizeof(int32_t));
	} else {
		this->dates = nullptr;
	}
	memoryUsage += (long) sizeof(int) * len;
}

void DateColumnVector::close() {
	if(!closed) {
		if(encoding && dates != nullptr) {
			free(dates);
		}
		dates = nullptr;
		ColumnVector::close();
	}
}

void DateColumnVector::print(int rowCount) {
	for(int i = 0; i < rowCount; i++) {
		std::cout<<dates[i]<<std::endl;
	}
}

DateColumnVector::~DateColumnVector() {
	if(!closed) {
		DateColumnVector::close();
	}
}

/**
     * Set a row from a value, which is the days from 1970-1-1 UTC.
     * We assume the entry has already been isRepeated adjusted.
     *
     * @param elementNum
     * @param days
 */
void DateColumnVector::set(int elementNum, int days) {
	if(elementNum >= writeIndex) {
		writeIndex = elementNum + 1;
	}
	dates[elementNum] = days;
	// TODO: isNull
}

void * DateColumnVector::current() {
    if(dates == nullptr) {
        return nullptr;
    } else {
        return dates + readIndex;
    }
}

/*function created by dhl as lab2*/
// void DateColumnVector::add(int value){
// 	/*这里value的意思是自1970年一月一日以来的天数*/
//     if (writeIndex >= length) {
//         ensureSize(writeIndex * 2, true);
//     }
//     int index = writeIndex++;
// 	dates[index]=value;
//     isNull[index] = false;
// }
void DateColumnVector::add(std::string &value){
//PixelsConsumer.cpp:120 add value: 50240095.29
    int result = dateToDates(value);
    add(result);

	return;
}
void DateColumnVector::ensureSize(uint64_t size, bool preserveData) {
    printf("length: %d, size: %d\n", length, size);
    if(dates == nullptr) {
        posix_memalign(reinterpret_cast<void **>(&dates), 32,size * sizeof(long));
        memoryUsage += (long) sizeof(long) * size;
        length = size;
    }
    if (length < size) {
        printf("length < size\n");
        if (1) {
            int *oldVector = dates;
            posix_memalign(reinterpret_cast<void **>(&dates), 32,
                           size * sizeof(long));
            printf("dates: %p\n", dates);
            if (preserveData) {
                std::copy(oldVector, oldVector + length, dates);
            }
            printf("copy success\n");
            //上面这个copy似乎会有问题，先不管了
            delete[] oldVector;
            printf("delete success\n");
            memoryUsage += (long) sizeof(long) * (size - length);
            length = size;
            // resize(size);
        } 
    }
}
void DateColumnVector::add(int value){
    
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
    printf("ensureSize success, dates size: %d, write index: %d\n", length, writeIndex);
    int index = writeIndex++;
    printf("writeIndex++ success\n");
    printf("add value: %ld\n", value);
	dates[index]=value;
    printf("add2vector success\n");
    isNull[index] = false;
}

int DateColumnVector::dateToDates(std::string& dateStr) {
    std::tm tm = {};
    std::stringstream ss(dateStr);
    
    // 解析字符串为日期
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail()) {
        std::cerr << "日期解析失败" << std::endl;
        return -1;
    }

    // 使用std::mktime将tm结构体转换为时间戳
    time_t timeSinceEpoch = std::mktime(&tm);
    if (timeSinceEpoch == -1) {
        std::cerr << "转换为时间戳失败" << std::endl;
        return -1;
    }

    return static_cast<int>(timeSinceEpoch/86400);
}