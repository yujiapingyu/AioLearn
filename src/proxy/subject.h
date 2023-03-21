#pragma once

/*
  Subject类，定义了RealSubject和Proxy的公共接口, 
  这样就在任何使用RealSubject的地方都可以使用Proxy.
*/

namespace proxy {

class Subject {
public:
  virtual void request() = 0;
};
    
} // end of namespace proxy


