#include <iostream>
#include "decorator/concrete_component.h"
#include "decorator/concrete_decorator_A.h"
#include "decorator/concrete_decorator_B.h"
#include "proxy/proxy.h"

int main() {

  /*
    装饰器模式：
    1. 实例化实体组件C;
    2. 用装饰器A来包装组件C;
    3. 用装饰器B来包装组件A;
    4. 调用装饰器B的operation方法
  */
  auto component = std::make_shared<decorator::ConcreteComponent>();
  auto da = std::make_shared<decorator::ConcreteDecoratorA>();
  auto db = std::make_shared<decorator::ConcreteDecoratorB>();

  da->set_component(component);
  db->set_component(da);
  db->operation();

  /*
    代理模式：
    创建代理，然后使用代理进行request
  */

  proxy::Proxy proxy;
  proxy.request();

  return 0;
}