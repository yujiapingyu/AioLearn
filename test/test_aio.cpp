#include <stdlib.h>
#include <string.h>
#include <libaio.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>

#define FILEPATH "./aio.txt"

int main()
{
  io_context_t context;
  iocb io[1];
  iocb *p[1] = {
    &io[0]
  };
  io_event e[1];
  unsigned nr_events = 10;
  timespec timeout;
  char *wbuf;
  int wbuflen =  50 * 1024 * 1024;  // 50MB
  int ret;

  posix_memalign((void **)&wbuf, 512, wbuflen);

  memset(wbuf, '@', wbuflen);
  memset(&context, 0, sizeof(io_context_t));

  timeout.tv_sec = 0;
  timeout.tv_nsec = 10000000;

  // 1. 打开要进行异步IO的文件
  int fd = open(FILEPATH, O_CREAT | O_RDWR | O_DIRECT, 0644);
  if (fd < 0) {
    std::cout << "open file error: " << strerror(errno) << std::endl;
    return -1;
  }

  // 2. 创建一个异步IO上下文
  if (0 != io_setup(nr_events, &context)) {
    std::cout << "iosetup error: " << strerror(errno) << std::endl;
    return 0;
  }

  // 3. 创建一个异步IO任务
  io_prep_pwrite(&io[0], fd, wbuf, wbuflen, 0);

  // 4. 提交异步IO任务
  if ((ret = io_submit(context, 1, p)) != 1) { 
    std::cout << "io submit error" << ret << std::endl;
    io_destroy(context);
    return -1;
  }

  // 5. 获取异步IO的结果
  while (1) {
    ret = io_getevents(context, 1, 1, e, &timeout);
    if (ret < 0) {
      std::cout << "io getevents error" << ret << std::endl;
      break;
    }

    if (ret > 0) {
      std::cout << "result, res2:" << e[0].res2 << ", res: " << e[0].res << std::endl;
      break;
    }
  }

  // 6. 销毁异步IO上下文
  io_destroy(context);

  // 7. 关闭文件
  close(fd);

  return 0;
}