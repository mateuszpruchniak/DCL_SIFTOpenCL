/*!
 * \file SIFTOpenCL_Processor.hpp
 * \brief
 * \author mpruchniak
 * \date 2012-04-27
 */

#ifndef SIFTOPENCL_PROCESSOR_HPP_
#define SIFTOPENCL_PROCESSOR_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Props.hpp"

#include <cv.h>
#include <highgui.h>

#include "Types/stream_OpenCV.hpp"

#include "SiftGPU.h"

namespace Processors {
namespace SIFTOpenCL {

using namespace cv;

/*!
 * \brief CvThreshold properties
 */
struct SIFTOpenCLProps: public Base::Props {
	cv::Size2i kernel;

	double sigmax;
	double sigmay;

	/*!
	 * \copydoc Base::Props::load
	 */
	void load(const ptree & pt) {
		kernel = pt.get("kernel", cv::Size2i(71, 71));

		sigmax = pt.get("sigmax", 0.0);
		sigmay = pt.get("sigmay", 0.0);
	}

	/*!
	 * \copydoc Base::Props::save
	 */
	void save(ptree & pt) {
		pt.put("kernel", kernel);

		pt.put("sigmax", sigmax);
		pt.put("sigmay", sigmay);
	}

};

/*!
 * \class SIFTOpenCL_Processor
 * \brief Example processor class.
 */
class SIFTOpenCL_Processor: public Base::Component {
public:
	/*!
	 * Constructor.
	 */
	SIFTOpenCL_Processor(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~SIFTOpenCL_Processor();

	/*!
	 * Return window properties
	 */
	Base::Props * getProperties() {
		return &props;
	}

protected:

	SiftGPU* siftGPU;
	feature* features;
	
	

	/*!
	 * Connects source to given device.
	 */
	bool onInit();

	/*!
	 * Disconnect source from device, closes streams, etc.
	 */
	bool onFinish();

	/*!
	 * Retrieves data from device.
	 */
	bool onStep();

	/*!
	 * Start component
	 */
	bool onStart();

	/*!
	 * Stop component
	 */
	bool onStop();

	/*!
	 * Event handler function.
	 */
	void onNewImage();

	/// Event handler.
	Base::EventHandler<SIFTOpenCL_Processor> h_onNewImage;

	/// Input data stream
	Base::DataStreamIn<Mat> in_img;

	/// Event raised, when image is processed
	Base::Event * newImage;

	/// Output data stream - processed image
	Base::DataStreamOut<Mat> out_img;

	/// Threshold properties
	SIFTOpenCLProps props;
};

} //: namespace SIFTOpenCL
} //: namespace Processors

/*
 * Register processor component.
 */
REGISTER_PROCESSOR_COMPONENT("SIFTOpenCL", Processors::SIFTOpenCL::SIFTOpenCL_Processor, Common::Panel_Empty)

#endif /* SIFTOPENCL_PROCESSOR_HPP_ */

