#pragma once

#include <iostream>
#include "decorator.h"

namespace decorator {

class ConcreteDecoratorB : public Decorator{
public:
  void operation() override {
    component->operation();
    std::cout << "装饰器B执行操作." << std::endl;
  }
};

} // end of decorator

