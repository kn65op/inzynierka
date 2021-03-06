#include "camera.h"


/* odbieranie obarzu z kamery z zastosowaniem bilbioteki videoInput: http://muonics.net/school/spring05/videoInput/ */

Camera::Camera(int d)
{
  init(d);
}

Camera::Camera(void)
{
    //yourBuffer = 0;
    //started = false;
  init(0);
}

void Camera::init(int d)
{
  int numDevices = VI.listDevices();
  device = d;
  //VI.setupDevice(device, 720, 576, VI_COMPOSITE);
  //VI.setupDevice(device, 160, 120);
  VI.setupDevice(device);
  int width = VI.getWidth(device);
  //int width = 728;
  int height = VI.getHeight(device);
  //int height = 576;
  image = cvCreateImage(cvSize(width, height), 8, 3);
  yourBuffer = new unsigned char[VI.getSize(device)];
  //yourBuffer = new unsigned char[width * height];
  started = true;
}

void Camera::setSource(int d)
{
    if (yourBuffer) delete [] yourBuffer;
    if (started)
    {
        VI.stopDevice(device);
        cvReleaseImage(&image);
    }
    init(d);
}

Camera::~Camera(void)
{
  VI.stopDevice(device);
  cvReleaseImage(&image);
  delete [] yourBuffer;
}

/*void Camera::getFrame(cv::Mat & mat)
{
  VI.getPixels(device, yourBuffer, false, false);
  image->imageData = (char*)yourBuffer;
  cvConvertImage(image, image, CV_CVTIMG_FLIP);
  mat = cv::cvarrToMat(image);
}*/

void Camera::getFrame(IplImage **mat)
{
  VI.getPixels(device, yourBuffer, false, false);
  image->imageData = (char*)yourBuffer;
  cvConvertImage(image, image, CV_CVTIMG_FLIP);
  *mat = new IplImage(*image);
}
