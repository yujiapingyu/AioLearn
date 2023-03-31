// Code written by Daniel Ehrenberg, released into the public domain

#include <fcntl.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <libaio.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "aio/aio_request.h"

DEFINE_string(path, "/tmp/testfile", "Path to the file to manipulate");
DEFINE_int32(file_size, 10, "Length of file in 4k blocks");
DEFINE_int32(concurrent_requests, 100, "Number of concurrent requests");
DEFINE_int32(min_nr, 1, "min_nr");
DEFINE_int32(max_nr, 1, "max_nr");

class Adder {
 public:
  virtual void Add(int amount) = 0;

  virtual ~Adder() { };
};

class AIOReadRequest : public AIORequest {
 private:
  Adder* adder_;

 public:
  AIOReadRequest(Adder* adder) : AIORequest(), adder_(adder) { }

  virtual void Complete(int res) {
    CHECK_EQ(res, kPageSize) << "Read incomplete or error " << res;
    int value = buffer_[0];
    LOG(INFO) << "Read of " << value << " completed";
    adder_->Add(value);
  }
};

class AIOWriteRequest : public AIORequest {
 private:
  int value_;

 public:
  AIOWriteRequest(int value) : AIORequest(), value_(value) {
    buffer_[0] = value;
  }

  virtual void Complete(int res) {
    CHECK_EQ(res, kPageSize) << "Write incomplete or error " << res;
    LOG(INFO) << "Write of " << value_ << " completed";
  }
};

class AIOAdder : public Adder {
 public:
  int fd_;
  io_context_t ioctx_;
  int counter_;
  int reap_counter_;
  int sum_;
  int length_;

  AIOAdder(int length)
      : ioctx_(0), counter_(0), reap_counter_(0), sum_(0), length_(length) { }

  void Init() {
    LOG(INFO) << "Opening file";
    fd_ = open(FLAGS_path.c_str(), O_RDWR | O_DIRECT | O_CREAT, 0644);
    PCHECK(fd_ >= 0) << "Error opening file";
    LOG(INFO) << "Allocating enough space for the sum";
    PCHECK(fallocate(fd_, 0, 0, kPageSize * length_) >= 0) << "Error in fallocate";
    LOG(INFO) << "Setting up the io context";
    PCHECK(io_setup(100, &ioctx_) >= 0) << "Error in io_setup";
  }

  virtual void Add(int amount) {
    sum_ += amount;
    LOG(INFO) << "Adding " << amount << " for a total of " << sum_;
  }

  void SubmitWrite() {
    LOG(INFO) << "Submitting a write to " << counter_;
    struct iocb iocb;
    struct iocb* iocbs = &iocb;
    AIORequest *req = new AIOWriteRequest(counter_);
    io_prep_pwrite(&iocb, fd_, req->buffer_, kPageSize, counter_ * kPageSize);
    iocb.data = req;
    int res = io_submit(ioctx_, 1, &iocbs);
    CHECK_EQ(res, 1);
  }

  void WriteFile() {
    reap_counter_ = 0;
    for (counter_ = 0; counter_ < length_; counter_++) {
      SubmitWrite();
      Reap();   // 这个操作其实是边提交边获取结果，也可以把这行注释掉
    }
    ReapRemaining();
  }

  void SubmitRead() {
    LOG(INFO) << "Submitting a read from " << counter_;
    struct iocb iocb;
    struct iocb* iocbs = &iocb;
    AIORequest *req = new AIOReadRequest(this);
    io_prep_pread(&iocb, fd_, req->buffer_, kPageSize, counter_ * kPageSize);
    iocb.data = req;
    int res = io_submit(ioctx_, 1, &iocbs);
    CHECK_EQ(res, 1);
  }

  void ReadFile() {
    reap_counter_ = 0;
    for (counter_ = 0; counter_ < length_; counter_++) {
        SubmitRead();
        Reap(); // 这个操作其实是边提交边获取结果，也可以把这行注释掉
    }
    ReapRemaining();
  }

  int DoReap(int min_nr) {
    LOG(INFO) << "Reaping between " << min_nr << " and "
              << FLAGS_max_nr << " io_events";
    struct io_event* events = new io_event[FLAGS_max_nr];
    struct timespec timeout;
    timeout.tv_sec = 0;
    timeout.tv_nsec = 100000000;
    int num_events;
    LOG(INFO) << "Calling io_getevents";
    num_events = io_getevents(ioctx_, min_nr, FLAGS_max_nr, events,
                              &timeout);
    LOG(INFO) << "Calling completion function on results";
    for (int i = 0; i < num_events; i++) {
      struct io_event event = events[i];
      AIORequest* req = static_cast<AIORequest*>(event.data);
      req->Complete(event.res);
      delete req;
    }
    delete events;
    
    LOG(INFO) << "Reaped " << num_events << " io_events";
    reap_counter_ += num_events;
    return num_events;
  }

  void Reap() {
    if (counter_ >= FLAGS_min_nr) {
      DoReap(FLAGS_min_nr);
    }
  }

  void ReapRemaining() {
    while (reap_counter_ < length_) {
      DoReap(1);
    }
  }

  ~AIOAdder() {
    LOG(INFO) << "Closing AIO context and file";
    io_destroy(ioctx_);
    close(fd_);
  }

  int Sum() {
    LOG(INFO) << "Writing consecutive integers to file";
    WriteFile();
    LOG(INFO) << "Reading consecutive integers from file";
    ReadFile();
    return sum_;
  }
};

int main(int argc, char* argv[]) {
  /*
    这段代码是一个使用Linux异步IO的示例，它计算了从0到FLAGS_file_size-1的整数之和。
    它使用了libaio库，该库提供了一种异步IO的方式，可以在IO操作完成之前继续执行其他操作。
    在这个例子中，程序首先写入FLAGS_file_size个整数到文件中，然后读取这些整数并计算它们的总和。
    在写入和读取文件时，程序使用了异步IO，这样可以在等待IO操作完成时继续执行其他操作。
    程序使用了io_setup()函数来初始化异步IO环境，使用io_submit()函数来提交IO请求，使用io_getevents()函数来等待IO操作完成并处理结果。
    程序使用了AIORequest类来表示IO请求，它有两个子类：AIOWriteRequest和AIOReadRequest，分别表示写入和读取请求。
    程序还使用了Adder类来表示一个可以添加整数的对象，它有一个子类AIOAdder，它使用AIORequest和Adder来实现异步IO的计算。
  */
  google::ParseCommandLineFlags(&argc, &argv, true);
  AIOAdder adder(FLAGS_file_size);
  adder.Init();
  int sum = adder.Sum();
  int expected = (FLAGS_file_size * (FLAGS_file_size - 1)) / 2;
  LOG(INFO) << "AIO is complete";
  CHECK_EQ(sum, expected) << "Expected " << expected << " Got " << sum;
  printf("Successfully calculated that the sum of integers from 0"
         " to %d is %d\n", FLAGS_file_size - 1, sum);
  return 0;
}