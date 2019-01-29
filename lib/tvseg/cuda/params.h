#ifndef TVSEG_CUDA_PARAMS_H
#define TVSEG_CUDA_PARAMS_H

#include "tvseg/settings/defineparams.h"


namespace tvseg {
namespace cuda {

typedef unsigned int uint;

} // namespace cuda
} // namespace tvseg


#define TVSEG_CUDA_SOLVER_PARAMS_SEQ    \
    (float, lambda, 1.0f)               \
    (uint8_t, maxSteps, 1000)              \
    (uint8_t, minSteps,  100)              \
    (float, epsilon, 1e-5f)             \
    (uint8_t, checkConverganceEvery, 20)   \
    (bool, displayIteration, true)      \
    (uint8_t, displayIterationEvery, 50)   \
    (bool, autoRecompute, false)

#define TVSEG_CUDA_SOLVER_OUTPUT_SEQ    \
    (uint, steps, 0)                    \
    (bool, cancelled, false)            \
    (float, time, 0.0f)                 \
    (float, energyGap, 0.0f)

#define TVSEG_CUDA_PARZEN_PARAMS_SEQ    \
    (float, colorVariance, 0.02f)        \
    (float, scribbleDistanceFactor, 1000.f)\
    (float, colorScale, 1.0f)            \
    (float, depthScale, 1.0f)            \
    (float, depthVariance, 1.0f)        \
    (bool, normalizeSpaceScale, true)   \
    (bool, scribbleDistanceDepth, true) \
    (bool, scribbleDistancePerspective, false) \
    (bool, useColorKernel, true)        \
    (bool, useDistanceKernel, true)     \
    (bool, useDepthKernel, false)       \
    (bool, fixScribblePixels, true)     \
    (bool, fixScribblePixelsThisOnly, false) \
    (bool, fixScribblePixelsOtherLabelUseMin, false) \
    (bool, useSpatiallyActiveScribbles, false) \
    (float, spatialActivationFactor, 3.0f) \
    (float, activeScribbleLambda, 0.8f)


#define TVSEG_CUDA_INTRINSICS_PARAMS_SEQ \
    (float, s_x, 1.0f)                  \
    (float, s_y, 1.0f)                  \
    (float, o_x, 0.0f)                  \
    (float, o_y, 1.0f)

namespace tvseg {
namespace cuda {

struct SolverParams {
    float lambda;
    uint8_t maxSteps;
    uint8_t minSteps;
    float epsilon;
    uint8_t checkConverganceEvery;
    bool displayIteration;
    uint8_t displayIterationEvery;
    bool autoRecompute;
    SolverParams() {
        lambda = 1.0f;
        maxSteps = 1000;
        minSteps = 100;
        epsilon = 1e-5f;
        checkConverganceEvery = 20;
        displayIteration = true;
        displayIterationEvery = 50;
        autoRecompute = false;
    }
};

struct SolverOutput {
    uint8_t steps;
    bool cancelled;
    float time;
    float energyGap;
    SolverOutput() {
        steps = 0;
        cancelled = false;
        time = 0.0f;
        energyGap = 0.0f;
    }
};

struct ParzenParams {
    float colorVariance;
    float scribbleDistanceFactor;
    float colorScale;
    float depthScale;
    float depthVariance;
    bool normalizeSpaceScale;
    bool scribbleDistanceDepth;
    bool scribbleDistancePerspective;
    bool useColorKernel;
    bool useDistanceKernel;
    bool useDepthKernel;
    bool fixScribblePixels;
    bool fixScribblePixelsThisOnly;
    bool fixScribblePixelsOtherLabelUseMin;
    bool useSpatiallyActiveScribbles;
    float spatialActivationFactor;
    float activeScribbleLambda;
    ParzenParams() {
        colorVariance = 0.02f;
        scribbleDistanceFactor = 1000.f;
        colorScale = 1.0f;
        depthScale = 1.0f;
        depthVariance = 1.0f;
        normalizeSpaceScale = true;
        scribbleDistanceDepth = true;
        scribbleDistancePerspective = false;
        useColorKernel = true;
        useDistanceKernel = true;
        useDepthKernel = false;
        fixScribblePixels = true;
        fixScribblePixelsThisOnly = false;
        fixScribblePixelsOtherLabelUseMin = false;
        useSpatiallyActiveScribbles = false;
        spatialActivationFactor = 3.0f;
        activeScribbleLambda = 0.8f	;
    }
};

struct Intrinsics {
    float s_x;
    float s_y;
    float o_x;
    float o_y;
    Intrinsics() {
        s_x = 1.0f;
        s_y = 1.0f;
        o_x = 0.0f;
        o_y = 1.0f;
    }
};

}
}

#endif // TVSEG_CUDA_PARAMS_H
