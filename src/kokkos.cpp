#include <mpi.h>

#include "kokkos.hpp"

#include <Kokkos_DefaultNode.hpp>
#include <Kokkos_StaticCrsGraph.hpp>
#include <Kokkos_View.hpp>
#include <TpetraCore_config.h>

#include "jlcxx/jlcxx.hpp"

namespace jltrilinos
{

struct WrapDevice
{
  template<typename TypeWrapperT>
  void operator()(TypeWrapperT&&)
  {
  }
};

namespace detail
{

  template<typename OutViewT, typename ViewT, typename DT>
  OutViewT make_view(jlcxx::SingletonType<OutViewT>, const std::string& name, jlcxx::ArrayRef<DT> arr)
  {
    const std::size_t length = arr.size();
    ViewT newview(name, length);
    auto hostview = Kokkos::create_mirror_view(newview);
    for(std::size_t i = 0; i != length; ++i)
    {
      hostview(i) = arr[i];
    }
    Kokkos::deep_copy(newview, hostview);
    return newview;
  }

  template<int Rank, typename ViewT>
  struct ViewConstructor
  {
    void operator()(jlcxx::Module&)
    {
    }
  };

  template<int Rank, typename DT, typename AT1, typename AT2>
  struct ViewConstructor<Rank, Kokkos::View<const DT*, AT1, AT2>>
  {
    void operator()(jlcxx::Module&)
    {
    }
  };

  template<typename DT, typename AT1, typename AT2>
  struct ViewConstructor<1, Kokkos::View<const DT*, AT1, AT2>>
  {
    typedef Kokkos::View<DT*, AT1, AT2> ViewT;
    typedef Kokkos::View<const DT*, AT1, AT2> ConstViewT;

    void operator()(jlcxx::Module& mod)
    {
      mod.method("makeview", make_view<ConstViewT,ViewT,DT>);
    }
  };

  template<typename DT, typename AT1, typename AT2>
  struct ViewConstructor<1, Kokkos::View<DT*, AT1, AT2>>
  {
    typedef Kokkos::View<DT*, AT1, AT2> ViewT;

    void operator()(jlcxx::Module& mod)
    {
      mod.method("makeview", make_view<ViewT,ViewT,DT>);
    }
  };
}

struct WrapView
{
  template<typename TypeWrapperT>
  void operator()(TypeWrapperT&& wrapped)
  {
    typedef typename TypeWrapperT::type WrappedT;
    typedef typename std::remove_const<typename WrappedT::value_type>::type ScalarT;
    wrapped.method("dimension", &WrappedT::template dimension<int_t>);
    wrapped.method(&WrappedT::template operator()<int_t, int_t>);
    wrapped.method("ptr_on_device", &WrappedT::ptr_on_device);
    wrapped.module().method("value_type", [] (jlcxx::SingletonType<WrappedT>) { return jlcxx::SingletonType<ScalarT>(); });
    wrapped.module().method("array_layout", [] (jlcxx::SingletonType<WrappedT>) { return jlcxx::SingletonType<typename WrappedT::array_layout>(); });
    wrapped.module().method("rank", [] (jlcxx::SingletonType<WrappedT>) { return int_t(WrappedT::rank); });
    detail::ViewConstructor<WrappedT::rank, WrappedT>()(wrapped.module());
  }
};

template<size_t N>
struct ViewParameters
{
};

template<>
struct ViewParameters<3>
{
  using type = jlcxx::Parametric<jlcxx::TypeVar<1>, jlcxx::TypeVar<2>, jlcxx::TypeVar<3>>;
  ViewParameters(jlcxx::Module& mod) : wrapped_type(mod.add_type<type>("View3"))
  {
  }
  jlcxx::TypeWrapper<type> wrapped_type;
};

template<>
struct ViewParameters<4>
{
  using type = jlcxx::Parametric<jlcxx::TypeVar<1>, jlcxx::TypeVar<2>, jlcxx::TypeVar<3>, jlcxx::TypeVar<4>>;
  ViewParameters(jlcxx::Module& mod) : wrapped_type(mod.add_type<type>("View4"))
  {
  }
  jlcxx::TypeWrapper<type> wrapped_type;
};

template<size_t N>
ViewParameters<N>& view_parameters(jlcxx::Module& mod)
{
  static ViewParameters<N> m_params = ViewParameters<N>(mod);
  return m_params;
}

template<typename T>
struct NbViewParameters;

template<typename... ParamsT>
struct NbViewParameters<Kokkos::View<ParamsT...>>
{
  static constexpr size_t value = sizeof...(ParamsT);
};

template<typename T>
constexpr size_t nb_view_params = NbViewParameters<T>::value;

} // namespace jltrilinos

namespace jlcxx
{
  template<unsigned Val>
  struct BuildParameterList<Kokkos::MemoryTraits<Val>>
  {
    typedef ParameterList<std::integral_constant<unsigned, Val>> type;
  };
} // namespace jlcxx

JLCXX_MODULE register_kokkos(jlcxx::Module& mod)
{
  using namespace jlcxx;
  using namespace jltrilinos;

  auto device_type = mod.add_type<Parametric<TypeVar<1>,TypeVar<2>>>("Device");
  mod.add_type<Kokkos::HostSpace>("HostSpace");
#ifdef HAVE_TPETRA_INST_SERIAL
  mod.add_type<Kokkos::Compat::KokkosSerialWrapperNode>("KokkosSerialWrapperNode");
  mod.add_type<Kokkos::Serial>("Serial");
  device_type.apply<Kokkos::Device<Kokkos::Serial, Kokkos::HostSpace>>(WrapDevice());
#endif
#ifdef HAVE_TPETRA_INST_OPENMP
  mod.add_type<Kokkos::Compat::KokkosOpenMPWrapperNode>("KokkosOpenMPWrapperNode");
  mod.add_type<Kokkos::OpenMP>("OpenMP");
  mod.method("initialize", [](Kokkos::OpenMP, int num_threads) { Kokkos::OpenMP::initialize(num_threads); });
  device_type.apply<Kokkos::Device<Kokkos::OpenMP, Kokkos::HostSpace>>(WrapDevice());
#endif

  mod.method("default_node_type", [] () { return (jl_datatype_t*)jlcxx::julia_type<KokkosClassic::DefaultNode::DefaultNodeType>(); });

  mod.add_type<Kokkos::LayoutLeft>("LayoutLeft");
  mod.add_type<Kokkos::LayoutRight>("LayoutRight");

  typedef ParameterList<Kokkos::LayoutLeft, Kokkos::LayoutRight> layouts_t;

  view_parameters<3>(mod).wrapped_type.apply_combination<Kokkos::View, arrays_t, layouts_t, kokkos_devices_t>(WrapView());
  view_parameters<4>(mod).wrapped_type.apply_combination<Kokkos::View, arrays_t, layouts_t, kokkos_devices_t, ParameterList<void>>(WrapView());

  mod.add_type<Parametric<TypeVar<1>>>("MemoryTraits")
    .apply<Kokkos::MemoryTraits<0u>>([](auto) {});

  mod.add_type<Parametric<TypeVar<1>, TypeVar<2>, TypeVar<3>>>("StaticCrsGraph_cpp")
    .apply_combination<Kokkos::StaticCrsGraph, local_ordinals_t, layouts_t, kokkos_devices_t>([&](auto wrapped)
    {
      typedef typename decltype(wrapped)::type WrappedT;
      typedef typename WrappedT::entries_type entries_type;
      typedef typename WrappedT::row_map_type row_map_type;

      if(!jlcxx::static_type_mapping<entries_type>::has_julia_type())
        view_parameters<nb_view_params<entries_type>>(mod).wrapped_type.template apply<entries_type>(WrapView());
      if(!jlcxx::static_type_mapping<row_map_type>::has_julia_type())
        view_parameters<nb_view_params<row_map_type>>(mod).wrapped_type.template apply<row_map_type>(WrapView());

      wrapped.template constructor<const entries_type&, const row_map_type>();
      wrapped.module().method("entries_type", [] (jlcxx::SingletonType<WrappedT>) { return jlcxx::SingletonType<entries_type>(); });
      wrapped.module().method("row_map_type", [] (jlcxx::SingletonType<WrappedT>) { return jlcxx::SingletonType<row_map_type>(); });
    });
}
