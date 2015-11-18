/*
    Lightmetrica - A modern, research-oriented renderer

    Copyright (c) 2015 Hisanari Otsu

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include <pch_test.h>
#include <lightmetrica/exception.h>
#include <lightmetrica/fp.h>
#include <lightmetrica-test/utils.h>

LM_TEST_NAMESPACE_BEGIN

TEST(FPTest, SupportedExceptions)
{
    const auto GetDescription = [](const std::string& out) -> std::string
    {
        std::regex re(R"x(Description +: ([\w_]+))x");
        std::smatch m;
        return std::regex_search(out, m, re) ? m[1] : std::string("");
    };

    const auto Trial = [&](const std::string& desc, const std::function<void()>& func) -> void
    {
        SEHUtils::EnableStructuralException();
        FPUtils::EnableFPControl();

        bool exception = false;
        try
        {
            func();
        }
        catch (const std::runtime_error& e)
        {
            exception = true;
            EXPECT_EQ(desc, std::string(e.what()));
        }
        EXPECT_TRUE(exception);

        FPUtils::DisableFPControl();
        SEHUtils::DisableStructuralException();
    };

    // --------------------------------------------------------------------------------

    Trial("FLT_INVALID_OPERATION", [&]()
    {
        const double t = std::numeric_limits<double>::infinity() * 0;
        LM_UNUSED(t);
    });

    Trial("FLT_INVALID_OPERATION", [&]()
    {
        double z = 0;
        const double t = 0 / z;
        LM_UNUSED(t);
    });

    Trial("FLT_INVALID_OPERATION", [&]()
    {
        std::sqrt(-1);
    });

    Trial("FLT_INVALID_OPERATION", [&]()
    {
        const double t = 1.0 * std::numeric_limits<double>::signaling_NaN();
        LM_UNUSED(t);
    });

    Trial("FLT_DIVIDE_BY_ZERO", [&]()
    {
        double z = 0;
        const double t = 1.0 / z;
        LM_UNUSED(t);
    });
}

TEST(FPTest, UnsupportedExceptions)
{
    // _EM_DENORMAL
    EXPECT_NO_THROW(
    {
        {
            // 4.940656e-324 : denormal
            const double t = 4.940656e-324;
            EXPECT_EQ(FP_SUBNORMAL, std::fpclassify(t));
        }
        
        {
            // 4.940656e-325 : below representable number by denormal -> clamped to zero
            const double t = 4.940656e-325;
            EXPECT_EQ(FP_ZERO, std::fpclassify(t));
        }
    });

    // Overflow
    EXPECT_NO_THROW(
    {
        const double max = std::numeric_limits<double>::max();
        const double t = max + 1.0;

        // Default rouding mode is `round to nearest`.
        EXPECT_EQ(max, t);
    });

    // Underflow
    EXPECT_NO_THROW(
    {
        double t = std::nextafter(std::numeric_limits<double>::min(), -std::numeric_limits<double>::infinity());
        EXPECT_EQ(FP_SUBNORMAL, std::fpclassify(t));
    });

    EXPECT_NO_THROW(
    {
        double t = std::nextafter(std::numeric_limits<double>::denorm_min(), -std::numeric_limits<double>::infinity());
        EXPECT_EQ(FP_ZERO, std::fpclassify(t));
    });

    // Inexact
    EXPECT_NO_THROW(
    {
        const double t = 2.0 / 3.0;
        LM_UNUSED(t);
    });

    EXPECT_NO_THROW(
    {
        const double t = std::log(1.1);
        LM_UNUSED(t);
    });
}

TEST(FPTest, DisabledBehavior)
{
    EXPECT_NO_THROW(
    {
        const double t = std::numeric_limits<double>::infinity() * 0;
        EXPECT_TRUE(glm::isnan(t));
    });

    EXPECT_NO_THROW(
    {
        double z = 0;
        const double t = 0 / z;
        EXPECT_TRUE(glm::isnan(t));
    });

    EXPECT_NO_THROW(
    {
        const double t = std::sqrt(-1);
        EXPECT_TRUE(glm::isnan(t));
    });

    EXPECT_NO_THROW(
    {
        const double t = 1.0 * std::numeric_limits<double>::signaling_NaN();
        EXPECT_TRUE(glm::isnan(t));
    });

    EXPECT_NO_THROW(
    {
        double z = 0;
        const double t = 1.0 / z;
        EXPECT_TRUE(glm::isinf(t));
    });
}

LM_TEST_NAMESPACE_END