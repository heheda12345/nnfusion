#!/bin/bash

source ~/miniconda3/etc/profile.d/conda.sh

conda activate grinder
./run_pytorch.sh
conda deactivate

conda activate baseline_jax
./run_jax.sh
conda deactivate

conda activate grinder
./run_sys.sh
conda deactivate
