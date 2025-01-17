/*
 * Copyright 2024 PixelsDB.
 *
 * This file is part of Pixels.
 *
 * Pixels is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Pixels is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 *
 * You should have received a copy of the Affero GNU General Public
 * License along with Pixels.  If not, see
 * <https://www.gnu.org/licenses/>.
 */


#include <utils/ConfigFactory.h>
#include "utils/BitUtils.h"
#include "writer/TimestampColumnWriter.h"
TimestampColumnWriter::TimestampColumnWriter(std::shared_ptr<TypeDescription> type, std::shared_ptr<PixelsWriterOption> writerOption)
    : ColumnWriter(type, writerOption),
      runlengthEncoding(false),
      encoder(std::make_unique<RunLenIntEncoder>()),
      curPixelVector() {

    // 可以在这里做一些初始化操作，例如设置运行长度编码标志
    // runlengthEncoding = writerOption->isRunLengthEncoding();
}

int TimestampColumnWriter::write(std::shared_ptr<ColumnVector> vector, int length) {
    auto columnVector = std::static_pointer_cast<TimestampColumnVector>(vector);
    long* values = columnVector->times;
    bool littleEndian = this->byteOrder == ByteOrder::LITTLE_ENDIAN;
    
    for (int i = 0; i < length; i++) {
        isNull[curPixelIsNullIndex++] = columnVector->isNull[i];
        curPixelEleIndex++;

        if (columnVector->isNull[i]) {
            hasNull = true;
            pixelStatRecorder.increment();

            if (nullsPadding) {
                // For nulls, we pad with zeros
                EncodingUtils::writeLongLE(outputStream, 0L);
            }
        } else {
            if (littleEndian) {
                EncodingUtils::writeLongLE(outputStream, values[i]);
            } else {
                EncodingUtils::writeLongBE(outputStream, values[i]);
            }
            pixelStatRecorder.updateInteger128(values[i], 0, 1);
        }

        // If current pixel size satisfies the pixel stride, end the current pixel and start a new one
        if (curPixelEleIndex >= pixelStride) {
            newPixel();
        }
    }

    return outputStream.size();
}

void TimestampColumnWriter::close() {
    // 关闭操作，例如释放资源
    encoder.reset();
}

void TimestampColumnWriter::newPixel() {
    // 处理新的像素
    curPixelVector.clear();
    curPixelEleIndex = 0;
}

void TimestampColumnWriter::writeCurPartTime(std::shared_ptr<ColumnVector> columnVector, long* values, int curPartLength, int curPartOffset) {
    // 处理当前时间的写入
    for (int i = 0; i < curPartLength; i++) {
        // 在这里添加逻辑来处理特定的部分时间写入
    }
}

bool TimestampColumnWriter::decideNullsPadding(std::shared_ptr<PixelsWriterOption> writerOption) {
    return writerOption->isNullsPadding();
}

pixels::proto::ColumnEncoding TimestampColumnWriter::getColumnChunkEncoding() const {
    // 返回列的编码方式
    return runlengthEncoding ? pixels::proto::ColumnEncoding::RLE : pixels::proto::ColumnEncoding::PLAIN;
}