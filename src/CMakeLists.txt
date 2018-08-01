add_library(jltrilinos SHARED
  belos.cpp
  benchmark.cpp
  ifpack2.cpp
  kokkos.cpp
  muelu.cpp
  testing.cpp
  teuchos.cpp
  tpetra.cpp
  thyra.cpp
)
target_link_libraries(jltrilinos ${Teuchos_LIBRARIES} ${Tpetra_LIBRARIES} ${Stratimikos_LIBRARIES} ${Thyra_LIBRARIES} ${Ifpack2_LIBRARIES} ${MueLu_LIBRARIES} JlCxx::cxxwrap_julia)

install(TARGETS
  jltrilinos
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
RUNTIME DESTINATION lib)
