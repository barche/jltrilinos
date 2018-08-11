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
-D Kokkos_ENABLE_OpenMP:BOOL=ON \
-D Trilinos_ENABLE_DEFAULT_PACKAGES:BOOL=OFF \
-D Trilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=ON \
-D Trilinos_ENABLE_TESTS:BOOL=OFF \
-D Trilinos_ENABLE_Amesos:BOOL=ON \
-D Trilinos_ENABLE_AztecOO:BOOL=ON \
-D Trilinos_ENABLE_Belos:BOOL=ON \
-D Trilinos_ENABLE_Didasko:BOOL=OFF \
-D Didasko_ENABLE_TESTS:BOOL=OFF \
-D Didasko_ENABLE_EXAMPLES:BOOL=OFF \
-D Trilinos_ENABLE_Epetra:BOOL=ON \
-D Trilinos_ENABLE_EpetraExt:BOOL=ON \
-D Trilinos_ENABLE_ShyLU:BOOL=ON \
-D Trilinos_ENABLE_Tpetra:BOOL=ON \
-D Trilinos_ENABLE_Teko:BOOL=ON \
-D Trilinos_ENABLE_TpetraExt:BOOL=ON \
-D Trilinos_ENABLE_Ifpack:BOOL=ON \
-D Trilinos_ENABLE_Meros:BOOL=ON \
-D Trilinos_ENABLE_ML:BOOL=ON \
-D Trilinos_ENABLE_RTOp:BOOL=ON \
-D Trilinos_ENABLE_Teuchos:BOOL=ON \
-D Trilinos_ENABLE_Thyra:BOOL=ON \
-D Trilinos_ENABLE_ThyraCore:BOOL=ON \
-D Trilinos_ENABLE_Triutils:BOOL=ON \
-D Trilinos_ENABLE_Stratimikos:BOOL=ON \
-D Trilinos_ENABLE_Zoltan:BOOL=ON \
-D Zoltan_ENABLE_EXAMPLES:BOOL=ON \
-D Zoltan_ENABLE_TESTS:BOOL=ON \
-D Zoltan_ENABLE_ULLONG_IDS:Bool=ON \
-D TPL_ENABLE_BLAS:BOOL=ON \
-D TPL_ENABLE_LAPACK:BOOL=ON \
-D TPL_ENABLE_MPI:BOOL=ON \
-D BUILD_SHARED_LIBS:BOOL=ON \
-D Trilinos_VERBOSE_CONFIGURE:BOOL=FALSE \
-D TPL_BLAS_LIBRARIES:STRING=“/share/apps/openblas/gcc72/lib/libopenblas.a” \
-D TPL_LAPACK_LIBRARIES:STRING=“/share/apps/openblas/gcc72/lib/libopenblas.a” \
  $sourcedir
```
