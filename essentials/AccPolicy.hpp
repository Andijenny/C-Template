#pragma once

#include <cmath>

#define TypePolicyObj(PolicyName, Ma, Mi, Val) \
struct PolicyName : virtual public Ma \
{ \
    using MajorClass = Ma; \
    using MinorClass = Ma::Mi##TypeCate; \
    using Mi = Ma::Mi##TypeCate::Val; \
};

#define TypePolicyType(PolicyName, Ma, Mi, Type) \
struct PolicyName : virtual public Ma \
{ \
    using MajorClass = Ma; \
    using MinorClass = Ma::Mi##TypeCate; \
    using Mi = Type; \
};

#define TypePolicyVal(PolicyName, Ma, Mi, Val) \
struct PolicyName : virtual public Ma \
{ \
    using MajorClass = Ma; \
    using MinorClass = Ma::Mi##ValueCate; \
    static constexpr decltype(Val) Mi() { return Val;} \
};



struct AccPolicy{
    using MajorClass = AccPolicy;

    struct ValueTypeCate;
    using Value = float;

    struct AccuTypeCate
    {
        struct Add {
            static constexpr Value Zero = 0;
            static Value Op(const auto& first, const auto& second)
            {
                return first + second;
            }
            static auto ave(Value total, Value n)
            {
                return total / n;
            }

        };

        struct Mul {
            static constexpr Value Zero = 1;
            static Value Op(const auto& first, const auto& second)
            {
                return first * second;
            }
            static Value ave(Value total, Value n)
            {
                return std::pow(total, 1.0 / n);
            }
        };
    };
    using Accu = AccuTypeCate::Add;

    struct IsAveValueCate;
    static constexpr bool IsAve(){return false;}
};

namespace util
{
    template<bool U, bool V>
    struct AndValue_{
        static constexpr bool value = (U && V);
    };

    template<typename T>
    struct IsEmptyArray_;


    template<template<typename...>class TPoliCont, typename...T>
    struct IsEmptyArray_<TPoliCont<T...>>{
        static constexpr bool value = TPoliCont<T...>::size == 0;
    };

    template<typename TPoliCont>
    static constexpr bool IsEmptyArray = IsEmptyArray_<TPoliCont>::value;
}

template<typename ...TPolicies>
struct PolicyContainer{
    static constexpr size_t size = sizeof...(TPolicies);
};

namespace internal
{
    template<bool U, bool V>
    static constexpr bool AndValue = util::AndValue_<U, V>::value;
    // insert types in TP.. that have same MajorClass as TMajor to container TPoliCont
    template<typename TPoliCont, typename TMajor, typename ...TP>
    struct MajorFilter_
    {
        using type = TPoliCont;
    };
    template<template<typename...>class TPoliCont, typename ...TSameMajorTypes, typename TMajor, typename TCur, typename ...TRemainTypes>
    struct MajorFilter_<TPoliCont<TSameMajorTypes...>, TMajor, TCur, TRemainTypes...> {
        using type = std::conditional_t<std::is_same<typename TMajor::MajorClass, typename TCur::MajorClass>::value,
                typename MajorFilter_<TPoliCont<TSameMajorTypes..., TCur>, TMajor, TRemainTypes...>::type,
                typename MajorFilter_<TPoliCont<TSameMajorTypes...>, TMajor, TRemainTypes...>::type>;
    };
    template<template<typename...>class TPoliCont, typename ... TSameMajorTypes, typename TMajor>
    struct MajorFilter_<TPoliCont<TSameMajorTypes...>, TMajor>
    {
        using type = TPoliCont<TSameMajorTypes...>;
    };

    template<typename T, typename TPolicyCont>
    struct MinorCheckInner_ ;

    template<typename TCur>
    struct MinorCheckInner_<TCur, PolicyContainer<>>{
        static constexpr bool value = true;
    };
    template<typename TCur, typename TN, typename ...TP>
    struct MinorCheckInner_<TCur, PolicyContainer<TN, TP...>> {
        static constexpr bool cur_check = !std::is_same<TCur, typename TN::MinorClass>::value;
        static constexpr bool value = AndValue<cur_check, MinorCheckInner_<TCur, PolicyContainer<TP...>>::value>;
    };

    template<typename TPolicyCont>
    struct MinorCheck_ {
        static constexpr bool value = true;
    };
    template<typename TCur, typename ...T>
    struct MinorCheck_<PolicyContainer<TCur, T...>> {
        static constexpr bool cur_check = MinorCheckInner_<typename TCur::MinorClass, PolicyContainer<T...>>::value;
        static constexpr bool value = AndValue<cur_check, MinorCheck_<PolicyContainer<T...>>::value>;
    };

    template<typename T>
    struct PolicySelRes;
    template<typename TPolicy>
    struct PolicySelRes<PolicyContainer<TPolicy>> : public TPolicy {};
    template<typename TCur, typename ...TP>
    struct PolicySelRes<PolicyContainer<TCur, TP...>>
            : public TCur,
              public PolicySelRes<PolicyContainer<TP...>>{};

    template<typename TPolicy, typename TPoliContainer>
    struct PolicySelect_;
    template<typename TMajorClass, typename ... TPolicies>
    struct PolicySelect_<TMajorClass, PolicyContainer<TPolicies...>>
    {
        using MajorTypeV = typename MajorFilter_<PolicyContainer<>, TMajorClass, TPolicies...>::type;

        static_assert(MinorCheck_<MajorTypeV>::value, "Minor class set conflict.");

        using type = std::conditional_t<util::IsEmptyArray<MajorTypeV>,
                                        TMajorClass,
                                        PolicySelRes<MajorTypeV>>;
    };
}

template<typename TPolicy, typename TPoliContainer>
using PolicySelect = typename internal::PolicySelect_<TPolicy, TPoliContainer>::type;

template<typename...TPolicies>
struct Accumulator
{
    using TPoliCont = PolicyContainer<TPolicies...>;
    using TPolicyRes = PolicySelect<AccPolicy, TPoliCont>;

    using ValueType = typename TPolicyRes::Value;
    static constexpr bool is_ave = TPolicyRes::IsAve();
    using AccuType = typename TPolicyRes::Accu;

public:
    template<typename TIn>
    static auto eval(const TIn& in)
    {
        ValueType count = 0;
        ValueType res = AccuType::Zero;
        for(const auto& i : in) {
            res = AccuType::Op(res, i);
            count += 1;
        }

        if constexpr(is_ave)
            return AccuType::ave(res, count);
        else
            return res;
    }
};
