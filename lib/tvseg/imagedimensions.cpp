#include "tvseg/imagedimensions.h"

#include "tvseg/util/opencvhelper.h"
#include "tvseg/util/logging.h"
#include "tvseg/util/errorhandling.h"

namespace tvseg {


bool checkDimensions(const cv::Mat image, const Dim3 specDim, const ImageSizeSpec spec)
{
    Dim3 imageDimBySpec;

    switch (spec.enum_value()) {
    case ImageSizeSpec::Input:
        imageDimBySpec = specDim;
        imageDimBySpec.labels = 0;
        break;
    case ImageSizeSpec::Dataterm:
        imageDimBySpec = specDim;
        break;
    case ImageSizeSpec::Labels:
        imageDimBySpec.height = 1;
        imageDimBySpec.width = specDim.labels;
        imageDimBySpec.labels = 0;
        break;
    case ImageSizeSpec::Invalid:
        LWARNINGF("checkDimensions: Checking for Invalid ImageSizeSpec");
        break;
    default:
        NOT_REACHABLE();
        break;
    }

    const Dim3 imageDim = matDim3(image);

    if (imageDim.width != imageDimBySpec.width && imageDim.height != imageDimBySpec.height && imageDim.labels != imageDimBySpec.labels) {
        std::cout << "Expected image dim " << imageDimBySpec.width << " for image spec " << spec << " but got " << imageDim.width;
        return false;
    } else {
        return true;
    }
}


} // namespace tvseg
