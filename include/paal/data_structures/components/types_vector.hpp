/**
 * @file types_vector.hpp
 * @brief This is implementation of type vector taking advantage of variadic template.
 *        This implementation is NOT  c++11 adaptation of mpl.
 *        It is small set of functon needed for Components class purpose.
 *        It is also less general than mpl. The implementation is create to avoid
 *        some problems with mpl. The c++11 techniques makes it much simpler and clearer.
 *        When boost::mpl11 apears this code should be removed
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-18
 */
#ifndef TYPES_VECTOR_HPP
#define TYPES_VECTOR_HPP
namespace paal {
namespace data_structures {


//TypesVector
template <typename... Args>
struct TypesVector;

//Computes size of TypesVector
template<typename Vec>
struct size;

template<typename... Args>
struct size<TypesVector<Args...>> {
    enum { value = sizeof...(Args)};
};

//Standard fold function implementation
template <typename Vector, typename StartValue, typename Functor>
struct fold;

template <typename StartValue, typename Functor, typename Arg, typename... Args>
struct fold<TypesVector<Arg, Args...>, StartValue, Functor> {
    typedef typename fold<
        TypesVector<Args...>,
        typename Functor::template apply<StartValue, Arg>::type,
        Functor
            >::type type;
};

template <typename StartValue, typename Functor>
struct fold<TypesVector<>, StartValue, Functor> {
    typedef StartValue type;
};

//push back given val to TypesVector
template <typename Vector, typename Val>
struct push_back;

template <typename Val, typename... Args>
struct push_back<TypesVector<Args...>, Val> {
    typedef TypesVector<Args..., Val> type;
};

//gives element on id in TypesVector
template <typename Vector, typename Id>
struct at;

template <typename C, C i, typename Arg, typename... Args>
struct at<TypesVector<Arg, Args...>, std::integral_constant<C, i>> {
    typedef typename at<Args..., std::integral_constant<C, i - 1 >>::type type;
};

template <typename C, typename Arg, typename... Args>
struct at<TypesVector<Arg, Args...>, std::integral_constant<C, 0>> {
    typedef Arg type;
};

//joins to TypesVectors
template <typename V1, typename V2>
struct join;

template <typename... Args1, typename... Args2>
struct join<TypesVector<Args1...>, TypesVector<Args2...>>{
    typedef TypesVector<Args1..., Args2...> type;
};

//removes first n elements from given TypesVector
template <int n, typename V>
struct remove_n_first;

template <int n, typename Arg, typename... Args>
struct remove_n_first<n, TypesVector<Arg, Args...>> {
    typedef typename remove_n_first<n - 1, TypesVector<Args...>>::type type;
};


// two cases below cannot be one becasuse of ambiguity in instancaition
template <typename Arg, typename... Args>
struct remove_n_first<0, TypesVector<Arg, Args...>>{
    typedef TypesVector<Arg, Args...> type;
};

template <>
struct remove_n_first<0, TypesVector<>>{
    typedef TypesVector<> type;
};

//returns pos of the element in the TypesVector
template <typename Type, typename TypesVector>
struct pos;

template <typename Type, typename TypesPrefix, typename... TypesSufix>
struct pos<Type, TypesVector<TypesPrefix, TypesSufix...>> {
    enum { value =  pos<Type, TypesVector<TypesSufix...>>::value + 1 };
};

template <typename Type, typename... TypesSufix>
struct pos<Type, TypesVector<Type, TypesSufix...>> {
    enum { value = 0 };
};

//replace element at pos to NewType
template <int pos, typename NewType, typename TypesVector>
struct replace_at_pos;

template <int pos, typename NewType, typename TypesPrefix, typename... TypesSufix>
struct replace_at_pos<pos, NewType, TypesVector<TypesPrefix, TypesSufix...>> {
    typedef typename join<TypesVector<TypesPrefix>,
                typename replace_at_pos<pos - 1, NewType, TypesVector<TypesSufix...>>::type>::type type;
};

template <typename NewType, typename TypesPrefix, typename... TypesSufix>
struct replace_at_pos<0, NewType, TypesVector<TypesPrefix, TypesSufix...>> {
    typedef TypesVector<NewType, TypesSufix...> type;
};

} // data_structures
} //paal

#endif /* TYPES_VECTOR_HPP */
