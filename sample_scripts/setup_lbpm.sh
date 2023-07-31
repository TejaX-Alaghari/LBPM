source /opt/intel/oneapi/setvars.sh
export MPI_DIR=/opt/intel/oneapi/mpi/2021.10.0

export I_MPI_OFFLOAD=2
export I_MPI_OFFLOAD_TOPOLIB=level_zero
#export I_MPI_DEBUG=120
export I_MPI_OFI_PROVIDER=shm

export LBPM_DEPS_DIR=/home/u106853/workspace/LBPM/deps/PVC_IMPI
export LBPM_ZLIB_DIR=${LBPM_DEPS_DIR}/install/zlib_1.2.13
export LBPM_HDF5_DIR=${LBPM_DEPS_DIR}/install/hdf5_1.14.1
export LBPM_SILO_DIR=${LBPM_DEPS_DIR}/install/silo_4.11

export LD_LIBRARY_PATH=${LBPM_ZLIB_DIR}/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=${LBPM_HDF5_DIR}/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=${LBPM_SILO_DIR}/lib:$LD_LIBRARY_PATH

export CPATH=${LBPM_ZLIB_DIR}/include:$CPATH
export CPATH=${LBPM_HDF5_DIR}/include:$CPATH
export CPATH=${LBPM_SILO_DIR}/include:$CPATH

#export ONEAPI_DEVICE_SELECTOR=level_zero:gpu
