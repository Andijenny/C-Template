#pragma once
#include <type_traits>


namespace tmp {

    //sequential 
    template<typename T>
    struct RemoveRefConst {
    private:
        using inner_type = typename std::remove_reference<T>::type;
    public:
        using type = typename std::remove_const<inner_type>::type;
    };

    template<typename T>
    using RemoveRefConst_t = typename RemoveRefConst<T>::type;

    /*------------------------------------------------------------*/
    /**********************branch if-else *************************/
    /*------------------------------------------------------------*/
    struct A; struct B;
    /*specialiation of class template */
    template<typename T>
    struct Branch {
        constexpr static size_t value = 0;
    };

    template<>
    struct Branch<A> {
        constexpr static size_t value = 1;
    };

    template<>
    struct Branch<B> {
        constexpr static size_t value = 2;
    };

    /*template variables*/
    template<typename T>
    constexpr size_t Branch1 = 0;

    template<>
    constexpr size_t Branch1<A> = 1;

    template<>
    constexpr size_t Branch1<B> = 2;

    /*partial specialization of template function*/

    template<typename T>
    struct Wrapper{
        template<typename U, typename DummyT = void>
        struct Branch2 {
            constexpr static size_t value = 0;
        };

        template<typename DummyT>
        struct Branch2<int, DummyT> {
            constexpr static size_t value = 1;
        };
    };

    /*enable_if, enable_if_t*/
    template<bool IsFeedBackOut, typename T, std::enable_if_t<IsFeedBackOut>* = nullptr>
    auto FeedBackOut(T&&) // true
    {
        return (int)0;
    }
    template<bool IsFeedBackOut, typename T, std::enable_if_t<!IsFeedBackOut>* = nullptr>
    auto FeedBackOut(T&&) // false
    {
        return (double)0;
    }

    /*if constexpr*/
    template<bool Check>
    auto FeedBackOut2()
    {
        if constexpr(Check) {
            return (int)0;
        }
        else {
            return (double)0;
        }
    }
    /*------------------------------------------------------------*/
    /********************** loop *********************************/
    /*------------------------------------------------------------*/

    /*count ones */
    template<size_t T>
    constexpr size_t Ones = (T%2) + Ones<T/2>;

    template<>
    constexpr size_t Ones<0> = 0;

    /*accumulate*/
    template<size_t ... T>
    constexpr size_t Accu = 0;

    template<size_t CurInput, size_t ... Inputs>
    constexpr size_t Accu<CurInput, Inputs...>
    = CurInput + Accu<Inputs...>;

    /*fold expression*/
    template<size_t ... Values>
    constexpr size_t Accu2()
    {
        return (0+...+Values);
    }
}
