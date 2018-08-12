# JlTrilinos

This is the C++ component of [Trilinos.jl](https://github.com/barche/Trilinos.jl). Example build steps (from the extracted source dir):

```bash
mkdir build
cd build
CC=mpicc CXX=mpic++ cmake -DJlCxx_DIR=/path/to/libcxxwrap-prefix ../
make
```

Then set the environment variable `JLTRILINOS_DIR` to the path of the build dir, and Trilinos.jl should pick it up.

## Trilinos example configuration

Here is an example script to configure Trilinos in a way that is compatible with Trilinos.jl:

```bash
#!/bin/bash

sourcedir=$HOME/src/trilinos
instdir=$HOME/src/build/trilinos-install

CC=mpicc
CXX=mpic++
FC=mpif77

ccmake -D CMAKE_INSTALL_PREFIX:PATH=$instdir -D CMAKE_BUILD_TYPE:STRING=RELEASE \
      -DTrilinos_ENABLE_ALL_PACKAGES:BOOL=OFF \
      -DTrilinos_ENABLE_FEI=OFF \
      -DTrilinos_ENABLE_STKClassic=OFF \
      -DTrilinos_ENABLE_SEACAS=OFF     \
      -DTrilinos_ENABLE_TESTS=OFF \
      -DTrilinos_ENABLE_Panzer=OFF \
      -DTrilinos_ENABLE_OpenMP:BOOL=ON \
      -DTrilinos_ENABLE_Amesos:BOOL=ON \
      -DTrilinos_ENABLE_AztecOO:BOOL=ON \
      -DTrilinos_ENABLE_Belos:BOOL=ON \
      -DTrilinos_ENABLE_Epetra:BOOL=ON \
      -DTrilinos_ENABLE_EpetraExt:BOOL=ON \
      -DTrilinos_ENABLE_Tpetra:BOOL=ON \
      -DTrilinos_ENABLE_Ifpack:BOOL=ON \
      -DTrilinos_ENABLE_Ifpack2:BOOL=ON \
      -DTrilinos_ENABLE_MueLu:BOOL=ON \
      -DTrilinos_ENABLE_ML:BOOL=ON \
      -DTrilinos_ENABLE_RTOp:BOOL=ON \
      -DTrilinos_ENABLE_ShyLU:BOOL=ON \
      -DTrilinos_ENABLE_Teuchos:BOOL=ON \
      -DTrilinos_ENABLE_Thyra:BOOL=ON \
      -DTrilinos_ENABLE_ThyraCore:BOOL=ON \
      -DTrilinos_ENABLE_Triutils:BOOL=ON \
      -DTrilinos_ENABLE_Stratimikos:BOOL=ON \
      -DTrilinos_ENABLE_EXPLICIT_INSTANTIATION:BOOL=ON \
      -DKokkos_ENABLE_Serial:BOOL=ON \
      -DKokkosKernels_INST_ORDINAL_INT64_T:BOOL=ON \
      -DTpetra_INST_SERIAL:BOOL=ON \
      -DTpetra_INST_OPENMP:BOOL=ON \
      -DTpetra_INST_INT_LONG:BOOL=ON \
      -DTPL_ENABLE_MPI:BOOL=ON \
      -DTrilinos_ENABLE_PyTrilinos:BOOL=OFF \
      -DTPL_ENABLE_Netcdf=OFF          \
      -DTPL_ENABLE_QT=OFF              \
      -DTPL_ENABLE_GLM=OFF              \
      -DBUILD_SHARED_LIBS:BOOL=ON \
      -D TPL_BLAS_LIBRARIES:STRING=“/share/apps/openblas/gcc72/lib/libopenblas.a” \
      -D TPL_LAPACK_LIBRARIES:STRING=“/share/apps/openblas/gcc72/lib/libopenblas.a” \
  $sourcedir
```
