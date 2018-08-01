#include <mpi.h>

#include "teuchos.hpp"
#include "tpetra.hpp"

#include <Teuchos_ArrayView.hpp>

#include "jlcxx/jlcxx.hpp"

namespace jltrilinos
{

template<typename T>
int arrayview_sum(const Teuchos::ArrayView<T>& av)
{
  T result = 0;
  for(T x : av)
  {
    result += x;
  }
  return result;
}

} // namespace jltrilinos

// Some methods to test types that would otherwise be difficult to test directly
JLCXX_MODULE register_testing(jlcxx::Module& mod)
{
  using namespace jlcxx;
  using namespace jltrilinos;

  mod.method("arrayview_sum", arrayview_sum<double>);
}
