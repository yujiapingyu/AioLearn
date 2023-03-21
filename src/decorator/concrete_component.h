#pragma once

#include <iostream>
#include "component.h"


namespace decorator {

class ConcreteComponent : public Component{
public:
  void operation() override {
    std::cout << "具体对象的操作" << std::endl;
  }
};

} // end of decorator
