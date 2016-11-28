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

#pragma once

#include "inversemaputils.h"

#define INVERSEMAP_DEBUG_SIMPLIFY_BIDIR_MUT_DELETE_ALL 0
#define INVERSEMAP_DEBUG_SIMPLIFY_BIDIR_MUT_PT 0
#define INVERSEMAP_DEBUG_SIMPLIFY_INDEPENDENT 0
#if LM_DEBUG_MODE
#define INVERSEMAP_MLT_DEBUG_IO 0
#else
#define INVERSEMAP_MLT_DEBUG_IO 0
#endif

LM_NAMESPACE_BEGIN

enum class MLTStrategy : int
{
    Bidir,
    Lens,
    Caustic,
    Multichain,
    ManifoldLens,   // Manifold lens perturbation supporting [LD]S+DS+E paths
    Identity,
};

// Bidirectional mutation first narrows the mutation space by limiting the deleted range
// in the current path, so it requires some additional information other than proposed path itself.
struct Prop
{
    Path p;
    int kd;
    int dL;
};

class MLTMutationStrategy
{
public:

    static auto Mutate(MLTStrategy strategy, const Scene* scene, Random& rng, const Path& currP) -> boost::optional<Prop>
    {
        if (strategy == MLTStrategy::Bidir)             { return Mutate_Bidir(scene, rng, currP); }
        else if (strategy == MLTStrategy::Lens)         { return Mutate_Lens(scene, rng, currP); }
        else if (strategy == MLTStrategy::Caustic)      { return Mutate_Caustic(scene, rng, currP); }
        else if (strategy == MLTStrategy::Multichain)   { return Mutate_Multichain(scene, rng, currP); }
        else if (strategy == MLTStrategy::ManifoldLens) { return Mutate_ManifoldLens(scene, rng, currP); }
        else if (strategy == MLTStrategy::Identity)     { return Prop{currP, -1 -1}; }
        LM_UNREACHABLE();
        return Prop();
    }

    static auto Q(MLTStrategy strategy, const Scene* scene, const Path& x, const Path& y, int kd, int dL) -> Float
    {
        if (strategy == MLTStrategy::Bidir)             { return Q_Bidir(scene, x, y, kd, dL); }
        else if (strategy == MLTStrategy::Lens)         { return Q_Lens(scene, x, y, kd, dL); }
        else if (strategy == MLTStrategy::Caustic)      { return Q_Caustic(scene, x, y, kd, dL); }
        else if (strategy == MLTStrategy::Multichain)   { return Q_Multichain(scene, x, y, kd, dL); }
        else if (strategy == MLTStrategy::ManifoldLens) { return Q_ManifoldLens(scene, x, y, kd, dL); }
        else if (strategy == MLTStrategy::Identity)     { return 1_f; }
        LM_UNREACHABLE();
        return 0_f;
    }

private:

    static auto Mutate_Bidir(const Scene* scene, Random& rng, const Path& currP) -> boost::optional<Prop>;
    static auto Mutate_Lens(const Scene* scene, Random& rng, const Path& currP) -> boost::optional<Prop>;
    static auto Mutate_Caustic(const Scene* scene, Random& rng, const Path& currP) -> boost::optional<Prop>;
    static auto Mutate_Multichain(const Scene* scene, Random& rng, const Path& currP) -> boost::optional<Prop>;
    static auto Mutate_ManifoldLens(const Scene* scene, Random& rng, const Path& currP) -> boost::optional<Prop>;

private:

    static auto Q_Bidir(const Scene* scene, const Path& x, const Path& y, int kd, int dL) -> Float;
    static auto Q_Lens(const Scene* scene, const Path& x, const Path& y, int kd, int dL) -> Float;
    static auto Q_Caustic(const Scene* scene, const Path& x, const Path& y, int kd, int dL) -> Float;
    static auto Q_Multichain(const Scene* scene, const Path& x, const Path& y, int kd, int dL) -> Float;
    static auto Q_ManifoldLens(const Scene* scene, const Path& x, const Path& y, int kd, int dL) -> Float;

};

LM_NAMESPACE_END
