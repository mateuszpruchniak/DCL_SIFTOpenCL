/*!
 * \file CameraOpenCV_Source.hpp
 * \brief Class responsible for retrieving images from camera
 * \author mstefanc
 * \date 2010-06-10
 */


#ifndef CAMERAOPENCV_SOURCE_HPP_
#define CAMERAOPENCV_SOURCE_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
#include "Panel_Empty.hpp"
#include "DataStream.hpp"
#include "Property.hpp"

#include <cv.h>
#include <highgui.h>

namespace Sources {
namespace CameraOpenCV {

using namespace cv;

/*!
 * \class CameraOpenCV_Source
 * \brief Class responsible for retrieving images from movies.
 */
class CameraOpenCV_Source : public Base::Component {

public:
	/*!
	 * Constructor. Sets ID and startup variables.
	 */
	CameraOpenCV_Source(const std::string & name = "");

	/*!
	 * Destructor.
	 */
	virtual ~CameraOpenCV_Source();

protected:

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
	void onTrigger();

	/// Event handler.
	Base::EventHandler<CameraOpenCV_Source> h_onTrigger;


	/// Event signaling that new image was retrieved.
	Base::Event * newImage;

	/// Output data stream
	Base::DataStreamOut<Mat> out_img;

	/// Capture device
	VideoCapture cap;

	/// Movie frame
	Mat frame;

	bool trig;


	Base::Property<int> m_device;
	Base::Property<int> m_width;
	Base::Property<int> m_height;

	void onDeviceCahnged(int old_device, int new_device);

	bool valid;
};

}//: namespace CameraOpenCV
}//: namespace Sources

/*
 * Register source component.
 */
REGISTER_SOURCE_COMPONENT("CameraOpenCV", Sources::CameraOpenCV::CameraOpenCV_Source, Common::Panel_Empty)

#endif /* CAMERAOPENCV_SOURCE_HPP_ */
