add_executable(powermethod powermethod.cpp)
target_link_libraries(powermethod ${Teuchos_LIBRARIES} ${Tpetra_LIBRARIES} ${Thyra_LIBRARIES} ${Belos_LIBRARIES} ${Stratimikos_LIBRARIES})

add_executable(laplace2d laplace2d.cpp)
target_link_libraries(laplace2d ${Teuchos_LIBRARIES} ${Tpetra_LIBRARIES} ${Thyra_LIBRARIES} ${Belos_LIBRARIES} ${Stratimikos_LIBRARIES})

add_executable(bench_assembly bench_assembly.cpp)
target_link_libraries(bench_assembly ${Teuchos_LIBRARIES} ${Tpetra_LIBRARIES} ${Thyra_LIBRARIES} ${Belos_LIBRARIES} ${Stratimikos_LIBRARIES})
