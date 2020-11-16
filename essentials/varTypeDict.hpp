#pragma once
#include <memory>
#include <utility>

namespace NSParameter
{
    //placeholder
    struct NullParameter;

    // ...T is empty in the first call of create_
    template<size_t N, template<typename...> class TCont, typename ...T>
    struct create_ {
        using type = typename create_<N-1, TCont, NullParameter, T...>::type;
    };

    // will return TCont<NullParameter, ... NullParameter>
    template<template<typename...>class TCont, typename ...T>
    struct create_<0, TCont, T...>{
        using type = TCont<T...>;
    };

    template<typename T1, typename T2, typename... TParameter>
    struct Order_{
        constexpr static size_t value = Order_<T1, TParameter...>::value;
    };
    template<typename T, typename... TParameter>
    struct Order_<T, T, TParameter...> {
        constexpr static size_t value = sizeof...(TParameter)+1;
    };

    //return the postion of type T in the template argument lists <...TParameter>
    template<typename T, typename... TParameter>
    constexpr size_t tag2Id = (sizeof...(TParameter)) - Order_<T, TParameter...>::value;

    template<typename TRealType, size_t N, size_t M, typename ...T>
    struct NewType_;

    template<typename TRealType, size_t N, size_t M,
            template<typename...>class TCont,
            typename ...TModifiedTypes,
            typename TCurType,
            typename ...TRemainTypes>
    struct NewType_<TRealType, N, M, TCont<TModifiedTypes...>, TCurType, TRemainTypes...> {
        using type = typename NewType_<TRealType, N, M+1, TCont<TModifiedTypes..., TCurType>,
                            TRemainTypes...>::type;
    };

    template<typename TRealType, size_t N,
            template<typename...>class TCont,
            typename ...TModifiedTypes,
            typename TCurType,
            typename ...TRemainTypes>
    struct NewType_<TRealType, N, N, TCont<TModifiedTypes...>, TCurType, TRemainTypes...> {
        using type = TCont<TModifiedTypes..., TRealType, TRemainTypes...>;
    };

    // replace the placeholder  at position N with the realType
    template<typename TRealType, size_t N, typename TCont, typename...T>
    struct NewType{
        using type = typename NewType_<TRealType, N, 0, TCont, T...>::type;
    };

    template<size_t N, size_t M, typename ...TRemainTypes>
    struct AtHelper_;

    template<size_t N, size_t M, typename TCur, typename ...TRemainTypes>
    struct AtHelper_<N, M, TCur, TRemainTypes...>{
        using type = typename AtHelper_<N, M+1, TRemainTypes...>::type;
    };

    template<size_t N, typename TCur, typename ...TRemainTypes>
    struct AtHelper_<N, N, TCur, TRemainTypes...>{
        using type = TCur;
    };


    template<size_t N, size_t M, typename TCont>
    struct At_;

    template<size_t N, size_t M, template<typename...>typename TCont, typename ...TParam>
    struct At_<N, M, TCont<TParam...>>{
        using type = typename AtHelper_<N, M, TParam...>::type;
    };

    // return the N-th type in the parameter list of class template Tcont 
    template<typename TCont, size_t N>
    using At = typename At_<N, 0, TCont>::type;
}


namespace NSdataType {
    template<typename ... TParameters >
    class VarTypeDict{
    public:
        template<typename... TType>
        struct Value{
        public:
            Value() = default;
            Value(std::shared_ptr<void>(&&input)[sizeof...(TType)])
            {
                for(size_t i = 0; i < sizeof...(TType); ++i)
                    m_inputs[i] = std::move(input[i]);
            }

            template<typename TTag, typename TVal>
            auto set(TVal&& val)&&
            {
                using namespace NSParameter;
                constexpr size_t pos = tag2Id<TTag, TParameters...>;
                using RawType = std::decay_t<TVal>;
                RawType* tmp = new RawType(std::forward<TVal>(val));
                m_inputs[pos] = std::shared_ptr<void>(tmp,
                    [](void* ptr) {
                         RawType* rptr  = static_cast<RawType*>(ptr);
                         delete rptr;
                     });

                using new_type = typename NewType<RawType, pos, Value<>, TType...>::type;
                return new_type(std::move(m_inputs));
            }

            template<typename TTag>
            const auto& get() const
            {
                constexpr size_t pos = NSParameter::tag2Id<TTag, TParameters...>;
                using type = NSParameter::At<Value, pos>;
                return *(static_cast<type*>(m_inputs[pos].get()));
            }
        private:
            std::shared_ptr<void>      m_inputs[sizeof...(TParameters)];
        };

        static auto create()
        {
            using namespace NSParameter;
            using type = typename create_<sizeof...(TParameters), Value>::type;
            return type{};
        }
    };
}
