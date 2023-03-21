#pragma once

/*
  RealSubject类，定义了Proxy所代表的真实实体.
*/

#include <iostream>
#include "subject.h"

namespace proxy {

class RealSubject : public Subject {
public:
  void request () override {
    std::cout<<"RealSubject执行请求成功."<<std::endl;
  }
};
    
} // end of namespace proxy


