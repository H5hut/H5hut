#!/bin/bash
#SBATCH --uenv=eurohack/24.9:rc1
#SBATCH --view=modules
#SBATCH --ntasks-per-node=16
#SBATCH --nodes=4
#SBATCH --output=out-%j.out
#SBATCH -C gpu
#SBATCH --partition=debug
#SBATCH --time=00:05:00
#
export NSYS_NVTX_PROFILER_REGISTER_ONLY=0
export CUDA_LAUNCH_BLOCKING=1
EXE="${HOME}/src/H5hut/src/examples/H5Part/write_setnparticles"
srun -n 64  "$EXE"
#nsys profile -t cuda,nvtx,mpi -o report.%p $EXE
