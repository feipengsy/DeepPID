#ifndef DeepPID_DeepParticleID_H
#define DeepPID_DeepParticleID_H
//
// Particle identification based on deep learning
//
// Author: Teng Li    05/07/2021, created
//
//
#include <vector>

enum method{XGBoost=1, LightGBM=2, MLP=3};
class PIDModel;
class EvtRecTrack;

class DeepParticleID {

  public:

    DeepParticleID(int method);
    virtual ~DeepParticleID(){ delete m_model; }

    bool calculate(EvtRecTrack* track);
    float prob(int n);

  private:

    void reset();
    bool readData(EvtRecTrack* track);
    
    std::vector<float> m_prob;
    float m_features[57];
    PIDModel*  m_model; 
};

#endif
