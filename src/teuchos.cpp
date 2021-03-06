#include <mpi.h>

#include <Teuchos_Array.hpp>
#include <Teuchos_BLAS_types.hpp>
#include <Teuchos_DefaultMpiComm.hpp>
#include <Teuchos_ParameterList.hpp>
#include <Teuchos_XMLParameterListHelpers.hpp>

#include "teuchos.hpp"

#include "jlcxx/jlcxx.hpp"

namespace jlcxx
{
  // Support MPI.CComm directly
  template<> struct static_type_mapping<MPI_Comm>
  {
    typedef MPI_Comm type;
    static jl_datatype_t* julia_type() { return (jl_datatype_t*)::jlcxx::julia_type("CComm", "MPI"); }
    template<typename T> using remove_const_ref = jlcxx::remove_const_ref<T>;
  };

  template<typename T> struct DefaultConstructible<Teuchos::Comm<T>> : std::false_type {};
  template<> struct DefaultConstructible<Teuchos::ParameterList> : std::false_type {};

  template<typename T> struct CopyConstructible<Teuchos::Comm<T>> : std::false_type {};
  //template<> struct CopyConstructible<Teuchos::ParameterList> : std::false_type {};
}

namespace jltrilinos
{

template<typename T>
struct AddSetMethod
{
  void operator()(jlcxx::Module& mod)
  {
    mod.method("set", [] (Teuchos::ParameterList& pl, const std::string& name, const T& value) { pl.set(name, value); });
  }
};

template<typename T>
struct AddSetMethod<jlcxx::StrictlyTypedNumber<T>>
{
  void operator()(jlcxx::Module& mod)
  {
    mod.method("set", [] (Teuchos::ParameterList& pl, const std::string& name, const jlcxx::StrictlyTypedNumber<T> value) { pl.set(name, value.value); });
  }
};

template<int Dummy=0> void wrap_set(jlcxx::Module&) {}

template<typename T, typename... TypesT>
void wrap_set(jlcxx::Module& mod)
{
  AddSetMethod<T>()(mod);
  wrap_set<TypesT...>(mod);
}

template<int Dummy=0> void wrap_get(jlcxx::Module&) {}

template<typename T, typename... TypesT>
void wrap_get(jlcxx::Module& mod)
{
  mod.method("get", [] (jlcxx::SingletonType<T>, Teuchos::ParameterList& pl, const std::string& name) -> T { return pl.get<T>(name); });
  wrap_get<TypesT...>(mod);
}

template<int Dummy=0> void wrap_is_type(jlcxx::Module&) {}

template<typename T, typename... TypesT>
void wrap_is_type(jlcxx::Module& mod)
{
  mod.method("isType", [] (jlcxx::SingletonType<T>, Teuchos::ParameterList& pl, const std::string& name) { return pl.isType<T>(name); });
  wrap_is_type<TypesT...>(mod);
}

template<int Dummy=0> jl_datatype_t* get_type(Teuchos::ParameterList& pl, const std::string& pname)
{
  std::cout << "warning, unknown type for parameter " << pname << " of parameterlist " << pl.name() << std::endl;
  return jlcxx::static_type_mapping<void>::julia_type();
}

template<typename T, typename... TypesT>
jl_datatype_t* get_type(Teuchos::ParameterList& pl, const std::string& pname)
{
  if(pl.isType<T>(pname))
  {
    return jlcxx::static_type_mapping<T>::julia_type();
  }
  return get_type<TypesT...>(pl, pname);
}

struct WrapArray
{
  template<typename TypeWrapperT>
  void operator()(TypeWrapperT&& wrapped)
  {
    using WrappedT = typename TypeWrapperT::type;
    using SizeT = typename WrappedT::size_type;
    using ConstRefT = typename WrappedT::const_reference;
    using ValT = typename WrappedT::value_type;
    wrapped.method("size", &WrappedT::size);
    wrapped.method("at", static_cast<ConstRefT (WrappedT::*)(SizeT) const>(&WrappedT::at));
    wrapped.method("setindex!", [] (WrappedT& w, const ValT& v, const int_t i) { w[i-1] = v; });
    wrapped.template constructor<const SizeT, const ValT&>();
  }
};

} // namespace jltrilinos

JLCXX_MODULE register_teuchos(jlcxx::Module& mod)
{
  using namespace jlcxx;
  using namespace jltrilinos;

  // Comm
  mod.add_type<Teuchos::Comm<int>>("Comm")
    .method("getRank", &Teuchos::Comm<int>::getRank)
    .method("getSize", &Teuchos::Comm<int>::getSize);
  mod.add_type<Teuchos::MpiComm<int>>("MpiComm", julia_type<Teuchos::Comm<int>>())
    .method("getRawMpiComm", [](const Teuchos::MpiComm<int> &comm) {
      return (*comm.getRawMpiComm())();
    });
  mod.method("MpiComm", [](MPI_Comm comm)
  {
    Teuchos::RCP<const Teuchos::MpiComm<int>> teuchos_comm(new Teuchos::MpiComm<int>(comm));
    return teuchos_comm;
  });

  mod.add_bits<Teuchos::ENull>("ENull", jlcxx::julia_type("CppEnum"));
  mod.set_const("null", Teuchos::null);

  mod.add_bits<Teuchos::ETransp>("ETransp", jlcxx::julia_type("CppEnum"));
  mod.set_const("NO_TRANS", Teuchos::NO_TRANS);
  mod.set_const("TRANS", Teuchos::TRANS);
  mod.set_const("CONJ_TRANS", Teuchos::CONJ_TRANS);

  mod.add_bits<Teuchos::EVerbosityLevel>("EVerbosityLevel", jlcxx::julia_type("CppEnum"));
  mod.set_const("VERB_DEFAULT", Teuchos::VERB_DEFAULT);
  mod.set_const("VERB_NONE", Teuchos::VERB_NONE);
  mod.set_const("VERB_LOW", Teuchos::VERB_LOW);
  mod.set_const("VERB_MEDIUM", Teuchos::VERB_MEDIUM);
  mod.set_const("VERB_HIGH", Teuchos::VERB_HIGH);
  mod.set_const("VERB_EXTREME", Teuchos::VERB_EXTREME);

  mod.add_type<Teuchos::ParameterList>("ParameterList", jlcxx::julia_type("PLAssociative", "Trilinos"))
    .method("numParams", &Teuchos::ParameterList::numParams)
    .method("setName", &Teuchos::ParameterList::setName)
    .method("name", static_cast<const std::string& (Teuchos::ParameterList::*)() const>(&Teuchos::ParameterList::name))
    .method("print", static_cast<void (Teuchos::ParameterList::*)() const>(&Teuchos::ParameterList::print))
    .method("isParameter", &Teuchos::ParameterList::isParameter)
    .method("isSublist", &Teuchos::ParameterList::isSublist);
  mod.method("ParameterList", [] () { return Teuchos::rcp(new Teuchos::ParameterList()); });
  mod.method("ParameterList", [] (const Teuchos::ParameterList& pl) { return Teuchos::rcp(new Teuchos::ParameterList(pl)); });
  mod.method("ParameterList", [] (const std::string& name) { return Teuchos::rcp(new Teuchos::ParameterList(name)); });
  mod.method("writeParameterListToXmlFile", [] (const Teuchos::ParameterList& pl, const std::string& filename) { Teuchos::writeParameterListToXmlFile(pl, filename); });
  wrap_set<StrictlyTypedNumber<int32_t>, StrictlyTypedNumber<int64_t>, StrictlyTypedNumber<double>, std::string, bool, Teuchos::ParameterList>(mod);
  wrap_get<int32_t, int64_t, double, std::string, bool, Teuchos::ParameterList>(mod);
  wrap_is_type<int32_t, int64_t, double, std::string, bool>(mod);
  mod.method("get_type", get_type<int32_t, int64_t, double, std::string, bool>);
  mod.method("sublist", [] (Teuchos::ParameterList& pl, const std::string& name) -> Teuchos::ParameterList& { return pl.sublist(name); });
  mod.method("keys", [] (const Teuchos::ParameterList& pl)
  {
    jlcxx::Array<std::string> keys;
    JL_GC_PUSH1(keys.gc_pointer());
    for(const auto& elem : pl)
    {
      keys.push_back(elem.first);
    }
    JL_GC_POP();
    return (jl_value_t*)keys.wrapped();
  });

  mod.add_type<Parametric<TypeVar<1>>>("Array", jlcxx::julia_type("AbstractVector"))
    .apply<Teuchos::Array<std::string>>(WrapArray());
}
