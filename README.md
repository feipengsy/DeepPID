# DeepPID

## Installation

1. Setup the BOSS (tested with 7.0.5) environment
2. Download the code and compile using CMT

```bash
git clone https://github.com/feipengsy/DeepPID.git
source DeepPID/setup.sh 
cd DeepPID/DeepParticleID/DeepParticleID-00-00-01/cmt
cmt config
make
source setup.sh
```

3. Edit your requirements file of your analysis package, add `use DeepParticleID DeepParticleID-*`
4. This is an example code of using the DeepParticleID code:

```c++
#include "DeepParticleID/DeepParticleID.h"

StatusCode Analysis::execute() {

  //...
  
  DeepParticleID* pid = new DeepParticleID(XGBoost);
  pid->calculate(evtRecTrack);
  float prob_pi = pid->prob(2);
  float prob_K = pid->prob(3);

  //...
  
}
```
