#pragma once

class ProducerInterface {
public:
  virtual ~ProducerInterface() = default;

  virtual void produce() = 0;
};
