#pragma once

/*
  Proxy类，代RealSubject执行request.
*/

#include <iostream>
#include <memory>
#include "subject.h"
#include "real_subject.h"

namespace proxy {

class Proxy : public Subject {
private:
  std::shared_ptr<RealSubject> real_subject;
public:
  void request () override {
    std::cout<<"我是代理，我代RealSubject来执行request."<<std::endl;
    if (real_subject == nullptr) {
        real_subject = std::make_shared<RealSubject>();
    }
    real_subject->request();
  }
};
    
} // end of namespace proxy


