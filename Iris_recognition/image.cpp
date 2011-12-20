#include <iostream>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cvaux.h>
#include <highgui.h>
#include <math.h>
#include <cv.h>
#include <stdlib.h>

#include <fstream>

class Image {
        public:

                static void imblank(IplImage *src) {
                        for(int i=0; i < src->height; i++) {
                                for(int j=0; j < src->width; j++) {
                                        cvSetReal2D(src, i, j, 0);
                                }
                        }
                }

                static IplImage* imfill(IplImage *src) {
                        IplImage* result = cvCreateImage(cvSize(src->width, src->height), src->depth, 0);

                        result = Image::imnegative(src);
                        result = Image::clearborders(result);
                        cvOr(result, src, result);

                        return result;
                }

                static IplImage* imlabel(IplImage *src) {
                        IplImage* result = cvCreateImage(cvSize(src->width, src->height), src->depth, 0);
                        double tab[4], min, max, label = 0;
                        int wsk_m = 1;
                        uchar merge[256];
                        CvMat* cvmLut = cvCreateMatHeader(1, 256, CV_8UC1);

                        // czyszczenie rezultatu
                        Image::imblank(result);

                        // wyzerowanie tablicy skleje�
                        for(int m=0; m<256; m++) {
                                merge[m] = 0;
                        }

                        for(int i=1; i < src->height; i++) {
                                for(int j=1; j < src->width-1; j++) {
                                        if(cvGetReal2D(src, i, j) == 1) {
                                                tab[0] = cvGetReal2D(result, i-1, j-1);
                                                tab[1] = cvGetReal2D(result, i-1, j);
                                                tab[2] = cvGetReal2D(result, i-1, j+1);
                                                tab[3] = cvGetReal2D(result, i, j-1);

                                                if(tab[0]==0 && tab[1]==0 && tab[2]==0 && tab[3]==0) {
                                                        label = merge[wsk_m] = wsk_m;
                                                        wsk_m++;
                                                } else {
                                                        min = 255; max = 0;
                                                        for(int k = 0; k < 4; k++){
                                                            if(tab[k] > max){ max = tab[k]; }
                                                            if(tab[k] < min && tab[k] != 0){ min = tab[k]; }
                                                    }

                                                        if(min == max) {
                                                                label = max;
                                                        } else {
                                                                label = min;
                                                                merge[(int) max] = (uchar) label;
                                                        }
                                                }

                                                cvSetReal2D(result, i, j, label);
                                        }
                                }
                        }

                        cvSetData(cvmLut, &merge, 0);
                        cvLUT(result, result, cvmLut);
                        //for(int l=0; l<256; l++) { std::cout << l << ": " << cvGetReal1D(cvmLut, l) << std::endl; }

                        return result;
                }

                static IplImage* deletelabel(IplImage *src, double label) {
                        IplImage* result = cvCloneImage(src);

                        for(int i=0; i < src->height; i++) {
                                for(int j=0; j < src->width; j++) {
                                        if(cvGetReal2D(result, i, j) == label) {
                                                cvSetReal2D(result, i, j, 0);
                                        }
                                }
                        }

                        return result;
                }

                static IplImage* clearborders(IplImage *src) {
                        IplImage* result = Image::imlabel(src);


                        // wyczyszczenie pikseli granicznych - lewa i prawa strona
                        for(int i=0; i < src->height; i++) {
                                cvSetReal2D(result, i, 0, 0);
                                cvSetReal2D(result, i, src->width-1, 0);

                                // znalezienie obiekt�w granicznych i usuni�cie ich - lewa
                                if(cvGetReal2D(result, i, 1) != 0) {
                                        result = Image::deletelabel(result, cvGetReal2D(result, i, 1));
                                }
                                // sprawdzany prawa
                                if(cvGetReal2D(result, i, src->width-2) != 0) {
                                        result = Image::deletelabel(result, cvGetReal2D(result, i, src->width-2));
                                }
                        }


                        // wyczyszczenie pikseli granicznych - g�ra i d�
                        for(int i=0; i < src->width; i++) {
                                cvSetReal2D(result, 0, i, 0);
                                cvSetReal2D(result, src->height-1, i, 0);

                                // znalezienie obiekt�w granicznych i usuni�cie ich - g�ra
                                if(cvGetReal2D(result, 1, i) != 0) {
                                        result = Image::deletelabel(result, cvGetReal2D(result, 1, i));
                                }
                                // sprawdzany d�
                                if(cvGetReal2D(result, src->height-2, i) != 0) {
                                        result = Image::deletelabel(result, cvGetReal2D(result, src->height-2, i));
                                }
                        }

                        // przerobienie na binarny obraz
                        cvThreshold(result, result, 0, 1, CV_THRESH_BINARY);

                        return result;
                }

                static IplImage* imadjust(IplImage *src) {
                        IplImage* result = cvCloneImage(src);
                        double  min, max;

                        cvMinMaxLoc(result, &min, &max);

                        double wsp = 255/(max-min);
                        cvConvertScale(result, result, wsp, -(min*wsp));

                        return result;
                }

                static IplImage** gabor_filter(IplImage *src, int size, double ab, double f, double w, double p, double theta) {
                        IplImage* result_real = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_32F, 1);
                        IplImage* result_imag = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_32F, 1);
                        IplImage** res = (IplImage **) malloc(sizeof(IplImage*)*2);

                        //int k = 1, number = 2*size+1; //CHANGE
                        int k = 10, number = size; //CHANGE
                        int xy0 = 0;
                        double real[number][number];
                        double imag[number][number];
                        double sinusoid_real, sinusoid_imag, gausian, rest_real, rest_imag;
                        double A, B;


                        //tmp
    //                    theta = M_PI / 8;
  //                      double a = 0.25;
//                        double b = 0.125;
                        //tmp
                        double ab2 = ab*ab;

                        //oblicznie wartosci filtru gabora
                        for(int x=-size/2; x <= size/2; x++) {
                                for(int y=-size/2; y <= size/2; y++) {
                                        sinusoid_real = cos(2 * M_PI * f * (cos(w) * x + sin(w) * y) + p);
                                        sinusoid_imag = sin(2 * M_PI * f * (cos(w) * x + sin(w) * y) + p);

                                        A = (x - xy0)*cos(theta) + (y - xy0)*sin(theta);
                                        A *= A;
                                        B = -(x - xy0)*sin(theta) + (y - xy0)*cos(theta);
                                        B *= B;

                                        gausian = k * exp(-M_PI * (A * (ab2) + (ab2) * B)); //rownanie 3
                                        //rest_real = exp(-M_PI * pow(f/ab, 2)) * cos(w);
                                        //rest_imag = exp(-M_PI * pow(f/ab, 2)) * sin(w);

                                        real[x+size/2][y+size/2] = gausian;// * sinusoid_real;// - rest_real);
                                        imag[x+size/2][y+size/2] = gausian * sinusoid_imag;// - rest_imag);
                                }
                        }

                        //tmp
                        std::ofstream of("gausian.csv", std::ios::trunc);
                        for (int i=0; i<size; i++)
                        {
                            for (int j=0; j<size; j++)
                            {
                                of << real[i][j] << ";";
                            }
                            of << "\n";
                        }
                        of.close();
                        //tmp

                        //?

                        IplImage* src32 = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_32F, 1);
                        cvConvertScale(src, src32, 1);

                        CvMat *filter_real = cvCreateMat(number, number, CV_32FC1);
                        cvSetData(filter_real, real, filter_real->step);

                        CvMat *filter_imag = cvCreateMat(number, number, CV_32FC1);
                        cvSetData(filter_imag, imag, filter_imag->step);

                        cvFilter2D(src32, result_imag, filter_imag);
                        cvFilter2D(src32, result_real, filter_real);

                        res[0] = result_real;
                        res[1] = result_imag;

                        cvReleaseImage(&src32);

                        return res;
                }

                static IplImage* imnegative(IplImage *src) {
                        IplImage *img = cvCreateImage(cvSize(src->width, src->height), src->depth,1);

                        for(int i=0; i < src->height; i++) {
                                for(int j=0; j < src->width; j++) {
                                        if(cvGetReal2D(src, i, j) == 0) {
                                                cvSetReal2D(img, i, j, 1);
                                        } else {
                                                cvSetReal2D(img, i, j, 0);
                                        }
                                }
                        }

                        return img;
                }

                static double center_l(const IplImage *img, double x0, double y0, double rp) {
                        int number = 26, ile = 0;
                        double theta[number];
                        double points[number];
                        int limit = (int) x0 - 1;

                        // wpisywanie warto�ci pocz�tkowych
                        theta[0]  = 0.8*M_PI;
                        points[0] = 0;
                        for(int i=1; i < number; i++) {
                                theta[i]  = theta[i-1] + 0.05;
                                points[i] = 0;
                        }

                        double r[limit];
                        r[0] = rp;
                        for(int j=1; j<limit; j++) {
                                r[j] = r[j-1] + 1;
                        }

                        // Wyszukiwanie punkt�w przeci�cia
                        for(int i=0; i < number; i++) {
                                double array_x[limit], array_y[limit];

                                for(int k=0; k < limit; k++) {
                                        array_x[k] = r[k] * cos(theta[i]);
                                        array_y[k] = r[k] * sin(theta[i]);
                                }

                                int j = 0;
                                for(; j < limit; j++) {
                                        if(cvGetReal2D(img, (int) (y0 + array_y[j]), (int) (x0 + array_x[j])) == 0)	 {
                                                break;
                                        }
                                }

                                points[ile] = j + rp;
                                ile++;
                        }

                        // �rednia d�ugo�ci spr�yn
                        double srednia, sum = 0;
                        for(int i=0; i < ile; i++) {
                                sum += points[i];
                        }
                        srednia = sum/ile;

                        return 0.85*srednia;
                }

                static int center_r2(const IplImage *img, int x0, int y0, int rp) {
                    int number = 126;
                    double theta[number];
                    int rad;
                    int limit = img->width - x0 - 20;

                    // Wyznaczenie prawego fragmentu okr�gu
                    theta[0]  = 0.2*M_PI;
                    for(int i=1; i < number; i++) {
                        theta[i]  = theta[i-1] - 0.01;
                    }

                    // Ustawienie warto�ci pocz�tkowych zmiennych pomocniczych
                    int sum = 0, sum2 = 0;
                    int max = 0, max2 = 0, cnt  = 0;

                    // Liczenie nat�enia pierwszego okr�gu
                    double array_x[number], array_y[number];

                    for(int i=0; i < number; i++) {
                        array_x[i] = rp * cos(theta[i]);
                        array_y[i] = rp * sin(theta[i]);
                    }

                    for(int i=0; i < number; i++) {
                        sum = sum + cvGetReal2D(img, y0 + (int) array_y[i], x0 + (int) array_x[i]);
                        cnt++;
                    }

                    sum = (int) sum/cnt;

                    // Iteracyjne zwi�kszanie promienia
                    for(int radius=rp; radius < limit; radius += 8) {
                        cnt = 0;
                       // qDebug() << radius;
                        for(int i=0; i < number; i++) {
                            array_x[i] = radius * cos(theta[i]);
                            array_y[i] = radius * sin(theta[i]);
                        }

                        for(int n=0; n < number; n++) {
                            if (y0 + (int) array_y[n] < img->height && y0 + (int) array_y[n] >= 0 && x0 + (int) array_x[n] < img->width && x0 + (int) array_x[n] >= 0)
                            {
                                sum2 = sum2 + cvGetReal2D(img, y0 + (int) array_y[n], x0 + (int) array_x[n]);
                                cnt++;
                            }
                        }

                        sum2 = (int) sum2/cnt;
                        max2 = sum2 - sum;

                        if(max2 >= max) {
                            max = max2;
                            rad = radius;
                        }

                        sum = sum2;
                        sum2 = 0;
                    }

                    float result = 0.9 * (float) rad;

                    return result;
                }

                static int center_l2(const IplImage *img, int x0, int y0, int rp) {
                    int number = 126;
                    double theta[number];
                    int rad;
                    int limit = x0 - 20;

                    // Wyznaczenie prawego fragmentu okr�gu
                    theta[0]  = 0.8*M_PI;
                    for(int i=1; i < number; i++) {
                        theta[i]  = theta[i-1] + 0.01;
                    }

                    // Ustawienie warto�ci pocz�tkowych zmiennych pomocniczych
                    int sum = 0, sum2 = 0;
                    int max = 0, max2 = 0, cnt  = 0;

                    // Liczenie nat�enia pierwszego okr�gu
                    double array_x[number], array_y[number];

                    for(int i=0; i < number; i++) {
                        array_x[i] = rp * cos(theta[i]);
                        array_y[i] = rp * sin(theta[i]);
                    }


                    for(int i=0; i < number; i++) {
                        sum = sum + cvGetReal2D(img, y0 + (int) array_y[i], x0 + (int) array_x[i]);
                        cnt++;
                    }

                    sum = (int) sum/cnt;

                    // Iteracyjne zwi�kszanie promienia
                    for(int radius=rp; radius < limit; radius += 8) {
                        cnt = 0;

                        for(int i=0; i < number; i++) {
                            array_x[i] = radius * cos(theta[i]);
                            array_y[i] = radius * sin(theta[i]);
                        }

                        for(int n=0; n < number; n++) {
                            // qDebug("%d", x0 + (int) array_x[n]);
                            if (y0 + (int) array_y[n] < img->height && y0 + (int) array_y[n] >= 0 && x0 + (int) array_x[n] < img->width && x0 + (int) array_x[n] >= 0)
                            {
                                sum2 = sum2 + cvGetReal2D(img, y0 + (int) array_y[n], x0 + (int) array_x[n]);
                                cnt++;
                            }
                        }

                        sum2 = (int) sum2/cnt;
                        max2 = sum2 - sum;

                        if(max2 >= max) {
                            max = max2;
                            rad = radius;
                        }

                        sum = sum2;
                        sum2 = 0;
                    }

                    float result = 0.9 * (float) rad;

                    return result;
                }

                static double center_r(const IplImage *img, double x0, double y0, double rp) {
                        int number = 26, ile = 0;
                        double theta[number];
                        double points[number];
                        int limit = img->width - (int) x0 - 1;

                        // wpisywanie warto�ci pocz�tkowych
                        theta[0]  = 0.2*M_PI;
                        points[0] = 0;
                        for(int i=1; i < number; i++) {
                                theta[i]  = theta[i-1] - 0.05;
                                points[i] = 0;
                        }

                        double r[limit];
                        r[0] = rp;
                        for(int j=1; j<limit; j++) {
                                r[j] = r[j-1] + 1;
                        }

                        // Wyszukiwanie punkt�w przeci�cia
                        for(int i=0; i < number; i++) {
                                double array_x[limit], array_y[limit];

                                for(int k=0; k < limit; k++) {
                                        array_x[k] = r[k] * cos(theta[i]);
                                        array_y[k] = r[k] * sin(theta[i]);
                                }

                                int j = 0;
                                for(; j < limit; j++) {
                                        if(cvGetReal2D(img, (int) (y0 + array_y[j]), (int) (x0 + array_x[j])) == 0)	 {
                                                break;
                                        }
                                }

                                points[ile] = j + rp;
                                ile++;
                        }

                        // �rednia d�ugo�ci spr�yn
                        double srednia, sum = 0;
                        for(int i=0; i < ile; i++) {
                                sum += points[i];
                        }
                        srednia = sum/ile;

                        return 0.85*srednia;
                }

                static double* center(const IplImage *img, double x, double y) {
                        double *tab = new double[3];
                        int number = 63, ile = 0;
                        double k = 1/(double) number;
                        double theta[number];
                        double points[number], diff_r[number];
                        int rp[91];

                        // wpisywanie warto�ci pocz�tkowych
                        theta[0]  = 0;
                        points[0] = 0;
                        for(int i=1; i < number; i++) {
                                theta[i] = theta[i-1] + 0.1;
                                points[i] = 0;
                        }

                        rp[0] = 10;
                        for(int i=1; i<91; i++) {
                                rp[i] = rp[i-1] + 1;
                        }

                        // Wyszukiwanie punkt�w przeci�cia
                        for(int i=0; i < number; i++) {
                                double array_x[91], array_y[91];

                                for(int k=0; k < 91; k++) {
                                        array_x[k] = rp[k] * cos(theta[i]);
                                        array_y[k] = rp[k] * sin(theta[i]);
                                }

                                int j = 0;
                                for(; j < 91; j++) {
                                        if(cvGetReal2D(img, (int) (y + array_y[j]), (int) (x + array_x[j])) == 0)	 {
                                                break;
                                        }
                                }

                                points[ile] = j + 10;
                                ile++;
                        }

                        // �rednia d�ugo�ci spr�yn
                        double srednia, sum = 0;
                        for(int i=0; i < ile; i++) {
                                sum += points[i];
                        }
                        srednia = sum/ile;

                        // Wektor kolejnych dlugosci pojedynczych sprezyn
                        double f_y[number], f_x[number];
                        double fw_y, fw_x, sum_y = 0, sum_x = 0;
                        for(int i=0; i < number; i++) {
                                diff_r[i] = srednia - points[i];
                                f_y[i] = diff_r[i] * sin(theta[i]);
                                f_x[i] = diff_r[i] * cos(theta[i]);
                                sum_y += f_y[i];
                                sum_x += f_x[i];
                        }

                        // Si�y wypadkowe
                        fw_y = sum_y * -k;
                        fw_x = sum_x * -k;

                        // Przesuni�cie �rodka o wyznaczone wektory
                        tab[0] = x + fw_x;
                        tab[1] = y + fw_y;
                        tab[2] = srednia * 1.2;

                        return tab;
                }

                /* Funkcja wy�wietlaj�ca obraz */
                static void showImage(IplImage *image, const char *name) {
                        cvNamedWindow(name);
                        cvMoveWindow(name, 20, 20);
                        cvShowImage(name, image);
                        cvWaitKey(0);
                }

                /* Funkcja wy�wietlaj�ca obraz binarny */
                static void showBinaryImage(IplImage *image, const char *name) {
                        IplImage *show = cvCloneImage(image);

                        for(int i=0; i < show->height; i++) {
                                for(int j=0; j < show->width; j++) {
                                        if(cvGetReal2D(show, i, j) == 1) {
                                                cvSetReal2D(show, i, j, 255);
                                        } else {
                                                cvSetReal2D(show, i, j, 0);
                                        }
                                }
                        }

                        cvNamedWindow(name, CV_WINDOW_AUTOSIZE);
                        cvMoveWindow(name, 50, 50);
                        cvShowImage(name, show);
                        cvWaitKey(0);
                }

                static void showMatrix(IplImage *show) {
                        for(int i=0; i < show->height; i++) {
                                for(int j=0; j < show->width; j++) {
                                        std::cout << cvGetReal2D(show, i, j) << " ";
                                }
                                std::cout << std::endl;
                        }
                }

                static void test() {
                        IplImage *src = cvLoadImage("tmp/fill.jpg");
                        IplImage *img = cvCreateImage(cvSize(src->width, src->height), src->depth, 1);
                        cvCvtColor(src, img, CV_RGB2GRAY);
                        cvThreshold(img, img, 45, 1, CV_THRESH_BINARY);

                        img = Image::imfill(img);
                        //Image::showBinaryImage(img);
                }


           /*QImage *IplImageToQImage(const IplImage * iplImage, uchar **data, double mini, double maxi) {
                    uchar *qImageBuffer = NULL;
                    int width = iplImage->width;

                    // Note here that OpenCV image is stored so that each lined is
                    // 32-bits aligned thus * explaining the necessity to "skip"
                    // the few last bytes of each line of OpenCV image buffer.
                    int widthStep = iplImage->widthStep;
                    int height = iplImage->height;

                    switch (iplImage->depth)
                    {
                    case IPL_DEPTH_8U:
                        if (iplImage->nChannels == 1)
                        {
                            // IplImage is stored with one byte grey pixel.
                            // We convert it to an 8 bit depth QImage.
                            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
                            uchar *QImagePtr = qImageBuffer;
                            const uchar *iplImagePtr = (const uchar *)iplImage->imageData;
                            for (int y = 0; y < height; y++)
                            {
                                // Copy line by line
                                memcpy(QImagePtr, iplImagePtr, width);
                                QImagePtr += width;
                                iplImagePtr += widthStep;
                            }
                        }
                        else if (iplImage->nChannels == 3)
                        {
                            // IplImage is stored with 3 byte color pixels (3 channels).
                            // We convert it to a 32 bit depth QImage.
                            qImageBuffer = (uchar *) malloc(width*height*4*sizeof(uchar));
                             uchar *QImagePtr = qImageBuffer;
                             const uchar *iplImagePtr = (const uchar *) iplImage->imageData;

                            for (int y = 0; y < height; y++)
                            {
                                for (int x = 0; x < width; x++)
                                {
                                    // We cannot help but copy manually.
                                    QImagePtr[0] = iplImagePtr[0];
                                    QImagePtr[1] = iplImagePtr[1];
                                    QImagePtr[2] = iplImagePtr[2];
                                    QImagePtr[3] = 0;

                                    QImagePtr += 4;
                                    iplImagePtr += 3;
                                }
                                iplImagePtr += widthStep-3*width;
                            }
                        }
                        else
                        {
                            qDebug("IplImageToQImage: image format is not supported:\
                                    depth=8U and %d channels\n", iplImage->nChannels);
                        }
                        break;

                    case IPL_DEPTH_16U:
                        if (iplImage->nChannels == 1)
                        {
                            // IplImage is stored with 2 bytes grey pixel.
                            // We convert it to an 8 bit depth QImage.
                            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
                            uchar *QImagePtr = qImageBuffer;
                            const uint16_t *iplImagePtr = (const uint16_t *)iplImage->imageData;

                            for (int y = 0; y < height; y++)
                            {
                                for (int x = 0; x < width; x++)
                                {
                                    // We take only the highest part of the 16 bit value.
                                    // It is similar to dividing by 256.
                                    *QImagePtr++ = ((*iplImagePtr++) >> 8);
                                }
                                iplImagePtr += widthStep/sizeof(uint16_t)-width;
                            }
                        }
                        else
                        {
                            qDebug("IplImageToQImage: image format is not supported:\
                                    depth=16U and %d channels\n", iplImage->nChannels);
                        }
                        break;

                    case IPL_DEPTH_32F:
                        if (iplImage->nChannels == 1)
                        {
                            // IplImage is stored with float (4 bytes) grey pixel.
                            // We convert it to an 8 bit depth QImage.
                            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
                            uchar *QImagePtr = qImageBuffer;
                            const float *iplImagePtr = (const float *) iplImage->imageData;

                            for (int y = 0; y < height; y++)
                            {
                                for (int x = 0; x < width; x++)
                                {
                                    uchar p;
                                    float pf = 255 * ((*iplImagePtr++) - mini) / (maxi - mini);

                                    if (pf < 0) p = 0;
                                    else if (pf > 255) p = 255;
                                    else p = (uchar) pf;

                                    *QImagePtr++ = p;
                                }
                                iplImagePtr += widthStep/sizeof(float)-width;
                            }
                        }
                        else
                        {
                            qDebug("IplImageToQImage: image format is not supported:\
                                    depth=32F and %d channels\n", iplImage->nChannels);
                        }
                        break;

                    case IPL_DEPTH_64F:
                        if (iplImage->nChannels == 1)
                        {
                            // OpenCV image is stored with double (8 bytes) grey pixel.
                            // We convert it to an 8 bit depth QImage.
                            qImageBuffer = (uchar *) malloc(width*height*sizeof(uchar));
                            uchar *QImagePtr = qImageBuffer;
                            const double *iplImagePtr = (const double *) iplImage->imageData;

                            for (int y = 0; y < height; y++)
                            {
                                for (int x = 0; x < width; x++)
                                {
                                    uchar p;
                                    double pf = 255 * ((*iplImagePtr++) - mini) / (maxi - mini);

                                    if (pf < 0) p = 0;
                                    else if (pf > 255) p = 255;
                                    else p = (uchar) pf;

                                    *QImagePtr++ = p;
                                }
                                iplImagePtr += widthStep/sizeof(double)-width;
                            }
                        }
                        else
                        {
                            qDebug("IplImageToQImage: image format is not supported:\
                                    depth=64F and %d channels\n", iplImage->nChannels);
                        }
                        break;

                    default:
                        qDebug("IplImageToQImage: image format is not supported: depth=%d\
                                and %d channels\n", iplImage->depth, iplImage->nChannels);
                    }

                    QImage *qImage;
                    if (iplImage->nChannels == 1)
                    {
                        QVector<QRgb> colorTable;
                        for (int i = 0; i < 256; i++)
                        {
                            colorTable.push_back(qRgb(i, i, i));
                        }
                        qImage = new QImage(qImageBuffer, width, height, QImage::Format_Indexed8);
                        qImage->setColorTable(colorTable);
                    }
                    else
                    {
                        qImage = new QImage(qImageBuffer, width, height, QImage::Format_RGB32);
                    }
                    *data = qImageBuffer;

                    return qImage;
                }*/
};
