#ifndef DeepPID_PIDModel_H
#define DeepPID_PIDModel_H

#include <vector>

class PIDModel {
  public:
    PIDModel() {}
    virtual ~PIDModel() {}
    virtual bool predict(float* features, std::vector<float>& result) = 0;
};

#endif
