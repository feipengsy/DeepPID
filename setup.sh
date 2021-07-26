#!/bin/bash
  
__conda_setup="$('/afs/ihep.ac.cn/users/t/tengli/junofs/soft/anaconda/bin/conda' 'shell.bash' 'hook' 2> /dev/null)"
if [ $? -eq 0 ]; then
    eval "$__conda_setup"
else
    if [ -f "/afs/ihep.ac.cn/users/t/tengli/junofs/soft/anaconda/etc/profile.d/conda.sh" ]; then
        . "/afs/ihep.ac.cn/users/t/tengli/junofs/soft/anaconda/etc/profile.d/conda.sh"
    else
        export PATH="/afs/ihep.ac.cn/users/t/tengli/junofs/soft/anaconda/bin:$PATH"
    fi
fi
unset __conda_setup

conda activate xg-gpu
