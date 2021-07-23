#ifndef DeepPID_ModelXGBoost_H
#define DeepPID_ModelXGBoost_H

#include "PIDModel.h"
#include <string>
#include <math.h>
#include <xgboost/c_api.h>

class ModelXGBoost: public PIDModel {

  public:

    ModelXGBoost(std::string const& path, uint64_t nlabels):
        _modelPath(path),
        _nlabels(nlabels)
    {
       
        if (XGBoosterCreate(NULL, 0, &_booster) == 0 &&  XGBoosterLoadModel(_booster, _modelPath.c_str()) == 0){
            //LOG HERE
        }else{
            //LOG HERE
            _booster = NULL;
        }        
    }

    bool predict(float* features, std::vector<float>& result)
    {
        if (NULL == _booster) {
            return false;
        }
        DMatrixHandle X;
        const float* data = features;
        int const nrow = 1;
        int const ncol = 57;

        XGDMatrixCreateFromMat(data, nrow, ncol, NAN, &X);
        
        const float* out;
        auto ret = XGBoosterPredict(_booster, X, 0, 0, 0, &out);
        if (ret < 0){
            // LOG HERE
            return false;
        }

        XGDMatrixFree(X);
        for (int i=0; i<5; ++i) {
          result.push_back(*(out+i));
        }

        return true;
    }

    virtual ~ModelXGBoost(){
        XGBoosterFree(_booster);
    }
    
  private:
    std::string const _modelPath;
    BoosterHandle _booster;
    uint64_t const _nlabels;
};

#endif
