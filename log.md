在pixels_DB/pixels/mini-pixels/pixels-core/include/exception/PixelsFileVersionInvalidException.h
和pixels_DB/pixels/mini-pixels/pixels-common/include/physical/Request.h
加入#include <cstdint>
不然会编译报错


在ColumnVector.cpp中的resize和ensuresize似乎自相矛盾了，一个是要size大于this->length，一个是小于？

这个ensuresize似乎没啥问题？先不管看看

出现
```
re@revalue:/mnt/c/learning/DB/pixels_DB/pixels/mini-pixels$ ./build/release/examples/pixels-example/pixels-example
PIXELS_SRC is /mnt/c/learning/DB/pixels_DB/pixels/mini-pixels
PIXELS_HOME is /mnt/c/learning/DB/pixels_DB/pixels/mini-pixels
pixels properties file is /mnt/c/learning/DB/pixels_DB/pixels/mini-pixels/pixels-cxx.properties
File not found at: /mnt/c/learning/DB/pixels_DB/pixels/mini-pixels/cpp/tests/data/example.pxl
Trying alternative path...
ConfigFactory: The key is not boolean type.
Invalid Error: std::exception
```
的问题，下载了原仓库的cxx配置解决了，，，