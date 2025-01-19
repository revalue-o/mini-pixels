//
// Created by yuly on 05.04.23.
//

#include "vector/DecimalColumnVector.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
/**
 * The decimal column vector with precision and scale.
 * The values of this column vector are the unscaled integer value
 * of the decimal. For example, the unscaled value of 3.14, which is
 * of the type decimal(3,2), is 314. While the precision and scale
 * of this decimal are 3 and 2, respectively.
 *
 * <p><b>Note: it only supports short decimals with max precision
 * and scale 18.</b></p>
 *
 * Created at: 05/03/2022
 * Author: hank
 */

DecimalColumnVector::DecimalColumnVector(int precision, int scale, bool encoding): ColumnVector(VectorizedRowBatch::DEFAULT_SIZE, encoding) {
    DecimalColumnVector(VectorizedRowBatch::DEFAULT_SIZE, precision, scale, encoding);
}

DecimalColumnVector::DecimalColumnVector(uint64_t len, int precision, int scale, bool encoding): ColumnVector(len, encoding) {
	// decimal column vector has no encoding so we don't allocate memory to this->vector
	this->vector = nullptr;
    this->precision = precision;
    this->scale = scale;
    memoryUsage += (uint64_t) sizeof(uint64_t) * len;
}

void DecimalColumnVector::close() {
    if(!closed) {
        ColumnVector::close();
		vector = nullptr;
    }
}

void DecimalColumnVector::print(int rowCount) {
//    throw InvalidArgumentException("not support print Decimalcolumnvector.");
    for(int i = 0; i < rowCount; i++) {
        std::cout<<vector[i]<<std::endl;
    }
}

DecimalColumnVector::~DecimalColumnVector() {
    if(!closed) {
        DecimalColumnVector::close();
    }
}

void * DecimalColumnVector::current() {
    if(vector == nullptr) {
        return nullptr;
    } else {
        return vector + readIndex;
    }
}

int DecimalColumnVector::getPrecision() {
	return precision;
}


int DecimalColumnVector::getScale() {
	return scale;
}

/*lab 2*/
void DecimalColumnVector::ensureSize(uint64_t size, bool preserveData) {
    printf("length: %d, size: %d\n", length, size);
    if(vector == nullptr) {
        posix_memalign(reinterpret_cast<void **>(&vector), 32,size * sizeof(long));
        memoryUsage += (long) sizeof(long) * size;
        length = size;
    }
    if (length < size) {
        printf("length < size\n");
        if (1) {
            long *oldVector = vector;
            posix_memalign(reinterpret_cast<void **>(&vector), 32,
                           size * sizeof(long));
            printf("vector: %p\n", vector);
            if (preserveData) {
                std::copy(oldVector, oldVector + length, vector);
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
void DecimalColumnVector::add(long value){
    
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
    printf("ensureSize success, vector size: %d, write index: %d\n", length, writeIndex);
    int index = writeIndex++;
    printf("writeIndex++ success\n");
    printf("add value: %ld\n", value);
	vector[index]=value;
    printf("add2vector success\n");
    isNull[index] = false;
}

void DecimalColumnVector::add(std::string &value) {
    // 打印出要添加的值
    printf("Start to add decimal value: %s\n", value.c_str());
    double decimalValue = std::stod(value);
    long long result = static_cast<long long>(std::round(decimalValue * pow(10, scale)));
    printf("Add decimal value: %lld\n", result);
    add(result);

    return;
}

// void DecimalColumnVector::add(std::string &value){
// //PixelsConsumer.cpp:120 add value: 50240095.29
//     printf("Start to add decimal value: %s\n", value.c_str());
//     long result = std::stod(value) * pow(10, scale);
//     printf("Add decimal value: %ld\n", result);
//     add(result);

// 	return;
// }