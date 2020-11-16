#include "gtest/gtest.h"
#include "../sequential.hpp"
#include "../varTypeDict.hpp"
#include "../AccPolicy.hpp"
#include <sstream>

namespace tmp
{
    TEST(Sequential, removeRefConst)
    {
        RemoveRefConst_t<const int&> h = 3;
        EXPECT_TRUE((std::is_same<decltype(h), int>::value));
    }

    TEST(VarTypeDict, tag2Id)
    {
        using namespace NSParameter;
        struct A; struct B; struct C; struct D;
        constexpr auto pos = tag2Id<C, A, B, C, D>;
        EXPECT_EQ(pos, 2);
    }

    TEST(VarTypeDict, set)
    {
        using type = NSdataType::VarTypeDict<struct A, struct B, struct C, struct D>;
        auto value = type::create().set<B>(0.2f).set<A>(true).set<D>(1);
        EXPECT_TRUE((value.get<A>()));
        EXPECT_EQ(value.get<D>(), 1);
        EXPECT_NEAR(value.get<B>(), 0.2, 0.001);
    }

    TEST(AccPolicy, Add)
    {
        TypePolicyObj(Add, AccPolicy, Accu, Add);
        TypePolicyType(Dbl, AccPolicy, Value, double);
        TypePolicyVal(Ave, AccPolicy, IsAve, true);
        auto res = Accumulator<Add, Dbl, Ave>::eval(std::vector<double>({1,2,3,4}));
        EXPECT_NEAR(res, 2.5, 0.001);
    }

    TEST(AccPolicy, Mul)
    {
        TypePolicyObj(Mul, AccPolicy, Accu, Mul);
        TypePolicyType(Int, AccPolicy, Value, int);
        TypePolicyVal(Ave, AccPolicy, IsAve, true);
        auto res = Accumulator<Mul, Int, Ave>::eval(std::vector<double>({1,2,3,4}));
        EXPECT_NEAR(res, 2.213, 0.001);
    }
}
