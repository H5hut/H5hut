#!/bin/bash
#SBATCH --uenv=eurohack/24.9:rc1
#SBATCH --view=modules
#SBATCH --ntasks-per-node=1
#SBATCH --nodes=1
#SBATCH --output=out-%j.out
#SBATCH -C gpu
#SBATCH --time=00:05:00
#SBATCH --reservation=eurohack24
#
export CUDA_LAUNCH_BLOCKING=1
#

#nsys profile -t cuda,mpi -o report.%p read_setnparticles_managed
ncu --kernel-name kernel --launch-skip 0 --launch-count 1 -o report.%p "read_setnparticles_managed"
