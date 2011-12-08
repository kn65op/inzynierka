#pragma once

#include "libs/videoInput.h"
#include <cv.h>
#include <highgui.h>


/* odbieranie obarzu z kamery z zastosowaniem bilbioteki videoInput: http://muonics.net/school/spring05/videoInput/ */

/**
\class Klasa Camera pozwalaj�ca na obs�ug� kamery
*/

class Camera
{
public:
  Camera(void);
  Camera(int d);
  ~Camera(void);
  /**
  Funckja pozwalaj�ca pobra� kolejn� klatk� obrazu z kamery
  \param docelowa macierz cv::Mat
  */
//  void getFrame(cv::Mat& mat);
  void setSource(int d);
  void getFrame(IplImage **mat);

private:
  void init(int d);

  videoInput VI;
  IplImage* image;
  unsigned char* yourBuffer;
  int device;
  bool started;

};

