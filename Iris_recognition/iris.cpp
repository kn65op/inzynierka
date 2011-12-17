#include <cstdlib>
#include <sstream>
#include "image.cpp"
#include <QtCore/QTextStream>

# define HAMMING 0.32

class Iris {
	public:
                int      maska[8][256];
                IplImage *img;
                double   hamming;
	
	private:
                IplImage *gray, *orginal;
                QString  filename;
		int      width, height;
                int      flash_x, flash_y;
                int      pupil_x, pupil_y, pupil_r;
                int	 iris_rr, iris_rl;
		CvScalar pupil_color, iris_left_color, iris_right_color;
		
	public:
		// Konstruktor
                Iris() {
                    this->orginal = NULL;
                }

                Iris(QString filename) {
                        // ï¿½aduje obraz
			this->filename = filename;
                        this->orginal  = cvLoadImage(this->filename.toAscii().data());
			this->img      = cvCloneImage(this->orginal);
			
			// Pobieranie informacji o rozmiarze
                        CvSize size  = cvGetSize(this->orginal);
			this->width  = size.width;
			this->height = size.height;
			
			// Kolor otoczki
			this->pupil_color      = CV_RGB(200, 50, 50);
			this->iris_left_color  = CV_RGB(50, 200, 50);
                        this->iris_right_color = CV_RGB(200, 50, 50);
			
			// Tworzy obraz monochromatyczny
                        this->gray = cvCreateImage(size, this->orginal->depth, 1);
			cvCvtColor(this->orginal, this->gray, CV_RGB2GRAY);

                        CvRect rect  = cvRect(200, 50, 1100, 1100);
                        cvSetImageROI(this->gray, rect);
                        //cvCopy(tmp, left);
		}
		
		// Destruktor
		~Iris() {
                    if(this->orginal != NULL) {
                        cvReleaseImage(&this->img);
                        cvReleaseImage(&this->orginal);
                        cvReleaseImage(&this->gray);
                    }
		}

                void init(QString filename) {
                        //this->width  = 1100;
                        //this->height = 1100;



                        // Laduje obraz
                        this->filename = filename;
                        this->orginal  = cvLoadImage(this->filename.toAscii().data());

                        CvSize size = cvGetSize(orginal);
                        this->width  = size.width;
                        this->height = size.height;
                        //CvRect rect  = cvRect(200, 50, 1100, 1100);
                        //cvSetImageROI(this->orginal, rect);

                        this->img = cvCloneImage(this->orginal);

                        // Kolor otoczki
                        this->pupil_color      = CV_RGB(200, 50, 50);
                        this->iris_left_color  = CV_RGB(50, 200, 50);
                        this->iris_right_color = CV_RGB(200, 50, 50);

                        // Tworzy obraz monochromatyczny
                        this->gray = cvCreateImage(size, this->orginal->depth, 1);
                        cvCvtColor(this->orginal, this->gray, CV_RGB2GRAY);

                        //cvResetImageROI(this->orginal);
                }

		/* Funkcja znajdujï¿½ca ï¿½renice */
                /**
                    Funkcja znajduj¹ca Ÿrenicê na podstawie jasnego odlasku w œrodku Ÿrenicy. Najpierw znadjowany jest odblask,
                potem algorytmem ekslopduj¹cych okrêgów jest szukany promieñ okrêgu. */
                bool pupil(int binary_value = 110) {
			// Rozmycie filtrem Gaussa
                        IplImage *image = cvCloneImage(this->gray);

                     //   cvSmooth(image, image, CV_GAUSSIAN, 7, 7, 1, 1);
			
			// Znalezienie ï¿½rodka ï¿½renicy
                     //   image = this->find_center_pupil(image, binary_value);
                        if (!this->find_flash_on_pupil(image)) //szukanie odblasku
                        {
                            return false;
                        }
                        IplConvKernel* element = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_ELLIPSE, NULL);

                        cvDilate(image, image, element);
                        cvErode(image, image, element);
                        cvReleaseStructuringElement(&element);
                        find_flash_center(image);
                        explode_rs(image);
                        //explode_circle(image);
                        //find_pupil_by(image);

			// Obrysowanie znalezionej ï¿½renicy
                        cvCircle(this->img, cvPoint(this->pupil_x, this->pupil_y), this->pupil_r, this->pupil_color, 1, 8, 0);
                        cvCircle(this->img, cvPoint(this->pupil_x, this->pupil_y), 2, this->pupil_color, 1, 8, 0);

                        cvReleaseImage(&image);
                        return true;
		}
		
		/* Funkcja znajdujï¿½ca obszar tï¿½czï¿½wki */
		void iris() {
			IplImage *image = cvCloneImage(this->gray);
			cvSmooth(image, image, CV_GAUSSIAN, 7, 7, 1, 1);
			
			// Znalezienie tï¿½czï¿½wki
                        this->find_iris2(image);
			
			// Obrysowanie znalezionymi promieniami
			int n = 32;
			double r_theta[n], r_array_x[n], r_array_y[n];
			double l_theta[n], l_array_x[n], l_array_y[n];
			CvPoint *r_points, *l_points;
			
			r_theta[0] = l_theta[0] = 0.5*M_PI;
			r_points = (CvPoint *)malloc(n * sizeof(CvPoint));
			l_points = (CvPoint *)malloc(n * sizeof(CvPoint));
			
			for(int i=1; i<n; i++) {
				r_theta[i] = r_theta[i-1] - 0.1;
				l_theta[i] = l_theta[i-1] + 0.1;
			}
			
			// Zamiana na wspï¿½rzï¿½dne kartezjaï¿½skie
			for(int k=0; k < n; k++) {
				r_array_x[k] = this->iris_rr * cos(r_theta[k]) + this->pupil_x;
				r_array_y[k] = this->iris_rr * sin(r_theta[k]) + this->pupil_y;
				l_array_x[k] = this->iris_rl * cos(l_theta[k]) + this->pupil_x;
				l_array_y[k] = this->iris_rl * sin(l_theta[k]) + this->pupil_y;
				
				r_points[k].x = (int) r_array_x[k];
				r_points[k].y = (int) r_array_y[k];
				l_points[k].x = (int) l_array_x[k];
				l_points[k].y = (int) l_array_y[k];
			}
			
			cvPolyLine(this->img, &r_points, &n, 1, 0, this->iris_right_color, 1, 8);
			cvPolyLine(this->img, &l_points, &n, 1, 0, this->iris_left_color, 1, 8);

                        cvReleaseImage(&image);
		}

	void draw_semicircles() {
		int n = 8;
		double step = -0.01;
                double rad;
		
		// Narysowanie obszarï¿½w po prawej stronie
		for(int i=1; i<n; i++) {
			int number = 252;
			double theta[number], array_x[number], array_y[number];
			CvPoint *points;
			
			points = (CvPoint *)malloc(number * sizeof(CvPoint));
			rad = this->pupil_r + i * (this->iris_rr - this->pupil_r)/8;
			theta[0] = 0.4 * M_PI;
			for(int m=1; m<number; m++) {
				theta[m] = theta[m-1] + step;	
			}
			
			for(int k=0; k<number; k++) {
				array_x[k]  = rad * cos(theta[k]) + this->pupil_x;
				array_y[k]  = rad * sin(theta[k]) + this->pupil_y;
				points[k].x = (int) array_x[k];
				points[k].y = (int) array_y[k];
			}
			
			cvPolyLine(this->img, &points, &number, 1, 0, this->iris_right_color, 1, 8);
		}
		
		// Narysowanie obszarï¿½w po lewej stronie
		for(int i=1; i<n; i++) {
			int number = 252;
			double theta[number], array_x[number], array_y[number];
			CvPoint *points;
			
			points = (CvPoint *)malloc(number * sizeof(CvPoint));
			rad = this->pupil_r + i * (this->iris_rl - this->pupil_r)/8;
			theta[0] = -0.6 * M_PI;
			for(int m=1; m<number; m++) {
				theta[m] = theta[m-1] + step;	
			}
			
			for(int k=0; k<number; k++) {
				array_x[k]  = rad * cos(theta[k]) + this->pupil_x;
				array_y[k]  = rad * sin(theta[k]) + this->pupil_y;
				points[k].x = (int) array_x[k];
				points[k].y = (int) array_y[k];
			}
			
			cvPolyLine(this->img, &points, &number, 1, 0, this->iris_left_color, 1, 8);
		}
	}
	
	void masking() {
                IplImage* left  = cvCreateImage(cvSize(this->pupil_x, this->height), this->orginal->depth, 1);
                IplImage* right = cvCreateImage(cvSize(this->width-this->pupil_x, this->height), this->orginal->depth, 1);
		IplImage* tmp   = cvCloneImage(this->gray);
		IplImage** gabor_left;
		IplImage** gabor_right;

		// Podziaï¿½ oka na prawï¿½ czï¿½ï¿½ i lewï¿½
		CvRect rect_left  = cvRect(0, 0, this->pupil_x, this->height);
		CvRect rect_right = cvRect(this->pupil_x, 0, this->width-this->pupil_x, this->height);
		
                cvSetImageROI(tmp, rect_left);
		cvCopy(tmp, left);
		
		cvResetImageROI(tmp);
		
		cvSetImageROI(tmp, rect_right);
		cvCopy(tmp, right);
		
		cvResetImageROI(tmp);
		cvReleaseImage(&tmp);
		
                double sigma = 0.31;
		double freq  = 0.15;
		double wr, wl;
		double p = 0;
		

		for(int k=1; k <= 8; k++) {
			wr = -0.4 * M_PI  + (k-1) * 0.1 * M_PI;
			wl = -1.4 * M_PI  + (k-1) * 0.1 * M_PI;
			
			gabor_right = Image::gabor_filter(right, sigma, freq, wr, p);
			gabor_left  = Image::gabor_filter(left,  sigma, freq, wl, p);
                        this->make_mask(gabor_left, gabor_right, k);
                }

	}

        QString get_mask() {
            QString str;
            char buffer[10];

            for(int i=0; i < 8; i++) {
                for(int j=0; j < 256; j++) {
                    itoa(this->maska[i][j], buffer, 2);
                    str += buffer;
                }
            }

            return str;
        }
	
        bool compare(QString code) {
                int    size = 2048;
                bool   res;
                int    k = 0;
                int    c[size];
                int    sum = 0;
		
		for(int i=0; i < 8; i++) {
                        for(int j=0; j < 256; j++) {
                                c[k] = this->maska[i][j] ^ code.at(k).digitValue();
                                k++;
                        }
		}
                //TODO rotacja
		for(int i=0; i < size; i++) {
			sum += c[i];	
		}
		
                this->hamming = (double) sum/size;
		
                if(this->hamming < HAMMING)
			res = true;
		else
			res = false;

		return res;	
        }

        IplImage* getMaskImage() {
            IplImage *result  = cvCreateImage(cvSize(256*2, 8*2), IPL_DEPTH_8U, 1);

            for(int i=0; i < 8; i++) {
                for(int j=0; j < 256; j++) {
                    cvSetReal2D(result, i*2, j*2, this->maska[i][j]);
                    cvSetReal2D(result, i*2+1, j*2+1, this->maska[i][j]);
                    cvSetReal2D(result, i*2+1, j*2, this->maska[i][j]);
                    cvSetReal2D(result, i*2, j*2+1, this->maska[i][j]);
                }
            }

            return result;
        }
		
	private:
		/* Funkcja wyznaczajï¿½ca maskï¿½ tï¿½czï¿½wki */
		void make_mask(IplImage** gabor_left, IplImage** gabor_right, int n) {
			// Indeks tablicy
			int ind = 0;
			double start, ends;
			double sum_real, sum_imag;
			long start2, ends2;
			
                        int ir = gabor_right[0]->height;
                        int jr = gabor_right[0]->width;
                        int il = gabor_left[0]->height;
                        int jl = gabor_left[0]->width;

                        bool lep[il][jl];
                        bool rip[ir][jr];

			// Wyzerowanie tablic
                        memset(lep, 0, il * jl * sizeof(bool));
                        memset(rip, 0, ir * jr * sizeof(bool));
			
			// PRAWA STRONA
                        double step = 0.001;

			for(int w=1; w <= 8; w++) {
				for(int k=1; k <= 8; k++)	 {
					sum_real = sum_imag = 0;
					
					// Wyznaczenie obszarï¿½w caï¿½kowania
					start = -0.4 * M_PI + (n-1) * 0.1 * M_PI + ((k-1)/8) * 0.1 * M_PI;
					ends  = -0.4 * M_PI + n * 0.1 * M_PI - ((double) (8-k)/8) * 0.1 * M_PI;
					
					start2 = lround(this->pupil_r + (w-1) * ((double) this->iris_rr - this->pupil_r)/8);
					ends2  = start2 + lround((double) (this->iris_rr - this->pupil_r)/8);
					
					// Zakres kï¿½towy
					int t_size = (int) ceil((ends - start)/step), r_size = ends2 - start2 + 1;
					double theta[t_size];
					long  rx[r_size];
					
					// Ustawienie wartoï¿½ci poczï¿½tkowych
					theta[0] = start;
					for(int i=1; i<t_size; i++) { theta[i] = theta[i-1] + step; }
					
					rx[0] = start2;
					for(int i=1; i<r_size; i++) { rx[i] = rx[i-1] + 1; }					
					
					for(int i=0; i < r_size; i++) {
						long x[t_size], y[t_size];
						
						for(int m=0; m < t_size; m++) {
							x[m] = lround(rx[i] * cos(theta[m]));
							y[m] = lround(rx[i] * sin(theta[m])) + this->pupil_y;
						}
						
						for(int p=0; p < t_size; p++) {
							if((y[p] > 0) && (y[p] <= ir) && (x[p] > 0) && (x[p] <= jr)) {
								if(rip[y[p]][x[p]] == 0) {
									sum_real += cvGetReal2D(gabor_right[0], y[p], x[p]);
									sum_imag += cvGetReal2D(gabor_right[1], y[p], x[p]);
									
									rip[y[p]][x[p]] = 1;
								}
							}	
						}
					}
					
					// Wyznaczanie bitï¿½w maski
					// W przypadku, gdy czï¿½ï¿½ rzeczywista lub urojona jest wiï¿½ksza od zera to bity majï¿½ wartoï¿½ï¿½ 1
					// w przeciwnym wypadku majï¿½ wartoï¿½ï¿½ 0
					if(sum_real >= 0) {
						this->maska[n-1][ind] = 1;
					} else {
						this->maska[n-1][ind] = 0;
					}
					
					ind++;
					if(sum_imag >= 0) {
						this->maska[n-1][ind] = 1;
					} else {
						this->maska[n-1][ind] = 0;
					}
					
					ind++;
					sum_real = sum_imag = 0;
				}
			}
			
			// LEWA STRONA
			for(int w=1; w <= 8; w++) {
				for(int k=1; k <= 8; k++)	 {
					sum_real = sum_imag = 0;
					
					// Wyznaczenie obszarï¿½w caï¿½kowania
					start = -1.4 * M_PI + (n-1) * 0.1 * M_PI + ((double) (k-1)/8) * 0.1 * M_PI;
					ends  = -1.4 * M_PI + n * 0.1 * M_PI - ((double) (8-k)/8) * 0.1 * M_PI;
					
					start2 = lround(this->pupil_r + (w-1) * ((double) this->iris_rl - this->pupil_r)/8);
					ends2  = start2 + lround((double) (this->iris_rl - this->pupil_r)/8);
					
					// Zakres kï¿½towy
					int t_size = (int) ceil((ends - start)/step), r_size = ends2 - start2 + 1;
					double theta[t_size];
					long  rx[r_size];
					
					// Ustawienie wartoï¿½ci poczï¿½tkowych
					theta[0] = start;
					for(int i=1; i<t_size; i++) { theta[i] = theta[i-1] + step; }
					
					rx[0] = start2;
					for(int i=1; i<r_size; i++) { rx[i] = rx[i-1] + 1; }					
					
					for(int i=0; i < r_size; i++) {
						long x[t_size], y[t_size];
						
						for(int m=0; m < t_size; m++) {
							x[m] = lround(rx[i] * cos(theta[m])) + this->pupil_x;
							y[m] = lround(rx[i] * sin(theta[m])) + this->pupil_y;
						}
						
						for(int p=0; p < t_size; p++) {
							if((y[p] > 0) && (y[p] <= il) && (x[p] > 0) && (x[p] <= jl)) {
								if(lep[y[p]][x[p]] == 0) {
									sum_real += cvGetReal2D(gabor_left[0], y[p], x[p]);
									sum_imag += cvGetReal2D(gabor_left[1], y[p], x[p]);
									
									lep[y[p]][x[p]] = 1;
								}
							}	
						}
					}
					
					// Wyznaczanie bitï¿½w maski
					// W przypadku, gdy czï¿½ï¿½ rzeczywista lub urojona jest wiï¿½ksza od zera to bity majï¿½ wartoï¿½ï¿½ 1
					// w przeciwnym wypadku majï¿½ wartoï¿½ï¿½ 0
					if(sum_real >= 0) {
						this->maska[n-1][ind] = 1;
					} else {
						this->maska[n-1][ind] = 0;
					}

					ind++;
					if(sum_imag >= 0) {
						this->maska[n-1][ind] = 1;
					} else {
						this->maska[n-1][ind] = 0;
					}
					
					ind++;
					sum_real = sum_imag = 0;
				}
			}

                        /*for (int t = 0 ; t < ir ; t++) { delete [] rip[t]; }
                        delete [] rip;
                        for (int t = 0 ; t < il ; t++) { delete [] lep[t]; }
                        delete [] lep;*/
		}

                IplImage* find_iris2(IplImage *src) {
                    IplImage *img   = cvCloneImage(src);

                    //this->find_iris_r(img);

                    float r1 = this->pupil_r * 1.5;

                    this->iris_rr = Image::center_r2(img, this->pupil_x, this->pupil_y, r1);
                    this->iris_rl = Image::center_l2(img, this->pupil_x, this->pupil_y, r1);

                    return img;
                }
		
		/* Funkcja znajdujï¿½ca tï¿½czï¿½wkï¿½ */
		IplImage* find_iris(IplImage *src) {
			IplImage *img   = cvCloneImage(src);
			IplImage *tmp1  = cvCreateImage(cvSize(src->width, src->height), src->depth, 1);
			IplImage *tmp2  = cvCreateImage(cvSize(src->width, src->height), src->depth, 1);
			
			// Binaryzacja z progiem 5 - wyznaczenie tï¿½czï¿½wki
                        cvThreshold(img, tmp1, 110, 1, CV_THRESH_BINARY_INV);

                        cvThreshold(img, tmp2, 60, 1, CV_THRESH_BINARY_INV);

                        cvSub(tmp1, tmp2, img);


                        IplConvKernel* element = cvCreateStructuringElementEx(21, 21, 10, 10, CV_SHAPE_ELLIPSE, NULL);
                        cvErode(img, img, element, 1);
                        cvDilate(img, img, element, 1);

                        img = Image::imfill(img);

                        // Image::showBinaryImage(img, "BLA");
			
                        /*
			// Binaryzacja z progiem 130 - wyznaczenie tï¿½czï¿½wki
			cvThreshold(img, tmp2, 140, 1, CV_THRESH_BINARY);
			
			// Suma na dwï¿½ch obrazach
                        cvOr(tmp1, tmp2, img, NULL);
			
			// Negatyw
			iris3 = Image::imnegative(img);
			
			// Otwarcie w celu usuniï¿½cia drobnych zakï¿½ï¿½ceï¿½
			IplConvKernel* element = cvCreateStructuringElementEx(12, 12, 6, 6, CV_SHAPE_ELLIPSE, NULL);
			cvMorphologyEx(iris3, iris1, tmp1, element, CV_MOP_OPEN, 1);
			
			// Dylatacja w celu domkniï¿½cia otworï¿½w przy granicy tï¿½czï¿½wki i biaï¿½ka
			element = cvCreateStructuringElementEx(14, 14, 7, 7, CV_SHAPE_ELLIPSE, NULL);
			cvDilate(iris1, img, element, 1);
			
			// Zalanie otworï¿½w
			iris2 = Image::imfill(img);
			
			// Odjï¿½cie
			cvSub(iris2, iris1, holes);
			
			// Dylatacja
			element = cvCreateStructuringElementEx(30, 30, 15, 15, CV_SHAPE_ELLIPSE, NULL);
			cvDilate(holes, holes, element, 1);
			
			// Suma na dwï¿½ch obrazach
			cvOr(holes, iris3, img, NULL);
			
			// Koï¿½cowa erozja
			element = cvCreateStructuringElementEx(40, 40, 20, 20, CV_SHAPE_ELLIPSE, NULL);
			cvErode(img, img, element, 1);
			
			// Wypelnienie powstalych dziur
                        img = Image::imfill(img);       */

			// Obliczanie promieni
                        //this->iris_rr = (int) Image::center_r(img, this->pupil_x, this->pupil_y, this->pupil_r);
                        //this->iris_rl = (int) Image::center_l(img, this->pupil_x, this->pupil_y, this->pupil_r);

                        this->find_iris_r(img);

			return img;
		}
		
		
                //TODO: prÃ³g binaryzacji, pÃ³Åºniej jak bÄ™dziemy mieli nowe obrazy
		/* Funkcja znajdujï¿½ca ï¿½rodek ï¿½renicy */
                IplImage* find_center_pupil(IplImage *src, int binary_value) {
                        int *middle;
                        IplImage *tmp = cvCreateImage(cvSize(src->width, src->height), src->depth, 1);
                        //IplImage *tmp2 = cvCreateImage(cvSize(src->width, src->height), src->depth, 1);
                        IplImage *tmpb = cvCreateImage(cvSize(src->width, src->height), src->depth, 1);
                        IplImage *tmpw = cvCreateImage(cvSize(src->width, src->height), src->depth, 1);

                        // Binaryzacja z progiem pobranym z inputa - wyznaczenie odblasku i czesci bialka oka

                //        cvThreshold(src, tmp, binary_value, 255, CV_THRESH_BINARY_INV);
               //         cvShowImage("Przed", src);
                        //cvShowImage("Po", tmp);
                        //while (cvWaitKey(1000) < 0);
                        //cvSaveImage("poprzednie.jpg", tmp);
             //           while (cvWaitKey(1000) < 0);
                        cvThreshold(src, tmpb, 40, 1, CV_THRESH_BINARY_INV);
                        cvThreshold(src, tmpw, 254, 1, CV_THRESH_BINARY);
                        cvOr(tmpb, tmpw, tmp);
                        cvReleaseImage(&tmpb);
                        cvReleaseImage(&tmpw);
/*cvThreshold(tmp, tmp2, 0, 255, CV_THRESH_BINARY);
cvShowImage(filename.toStdString().c_str(), tmp2);
while (cvWaitKey(1000) < 0);*/
                        IplConvKernel* element = cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_ELLIPSE, NULL);
                        cvDilate(tmp, tmp, element, 1);
                        cvErode(tmp, tmp, element, 1);
                        cvReleaseStructuringElement(&element);
                        element = cvCreateStructuringElementEx(11, 11, 5, 5, CV_SHAPE_ELLIPSE, NULL);
                        /*cvThreshold(tmp, tmp2, 0, 255, CV_THRESH_BINARY);
                        cvShowImage("Po", tmp2);
                        while (cvWaitKey(1000) < 0);*/
                        cvErode(tmp, tmp, element, 1);
                        cvDilate(tmp, tmp, element, 1);
                        cvReleaseStructuringElement(&element);
                        /*cvThreshold(tmp, tmp2, 0, 255, CV_THRESH_BINARY);
                        cvShowImage("Po", tmp2);
                        while (cvWaitKey(1000) < 0);*/
                        element = cvCreateStructuringElementEx(23, 23, 11, 11, CV_SHAPE_ELLIPSE, NULL);
                        cvDilate(tmp, tmp, element, 1);
                        cvErode(tmp, tmp, element, 1);
                        cvReleaseStructuringElement(&element);
/*cvThreshold(tmp, tmp2, 0, 255, CV_THRESH_BINARY);
cvShowImage("Po", tmp2);
while (cvWaitKey(1000) < 0);*/
                        element = cvCreateStructuringElementEx(23, 23, 11, 11, CV_SHAPE_ELLIPSE, NULL);
                        cvDilate(tmp, tmp, element, 1);
                        cvErode(tmp, tmp, element, 1);
                        cvReleaseStructuringElement(&element);
                        tmp = Image::clearborders(tmp);
/*cvThreshold(tmp, tmp2, 0, 255, CV_THRESH_BINARY);
cvShowImage("clear", tmp2);
while (cvWaitKey(1000) < 0);
cvDestroyAllWindows();
return tmp;*/
//                        element = cvCreateStructuringElementEx(41, 41, 20, 20, CV_SHAPE_ELLIPSE, NULL);
  //                      cvErode(tmp, tmp, element, 1);
    //                    cvDilate(tmp, tmp, element, 1);
/*cvThreshold(tmp, tmp2, 0, 255, CV_THRESH_BINARY);
cvShowImage("Po", tmp2);
while (cvWaitKey(1000) < 0);*/
                        middle = this->find_center(tmp);

                        return tmp;
		}
		
		/* Funkcja znajdujï¿½ca ï¿½rodek oblasku */

                void find_iris_r(IplImage *image) {
                    int max_left = this->width, max_right = 0;

                    for(int j=0; j<this->pupil_x; j++) {
                        if(cvGetReal2D(image, this->pupil_y, j) == 1) {
                            max_left = j;
                            break;
                        }
                    }

                    for(int j=this->pupil_x; j<this->width; j++) {
                        if(cvGetReal2D(image, this->pupil_y, j) == 1) {
                            max_right = j;
                        } else {
                            break;
                        }
                    }

                    this->iris_rl = 0.92 * (this->pupil_x - max_left);
                    this->iris_rr = 0.87 * (max_right - this->pupil_x);
                }

                int* find_center(IplImage *image) {
			int sum_x = 0;
			int sum_y = 0;
			int cnt   = 0;
                        int *tab = new int[3];
                        int max_x = 0, max_y = 0;
			
			for(int i=0; i<this->height; i++) {
				for(int j=0; j<this->width; j++) {
                                        if(cvGetReal2D(image, i, j) == 255) {
                                                if(max_x < j)
                                                    max_x = j;

                                                if(max_y < i)
                                                    max_y = i;

						sum_x += j;
						sum_y += i;
						cnt++;
					}
				}
			}
			
                        tab[0] = this->pupil_x = sum_x/cnt;
                        tab[1] = this->pupil_y = sum_y/cnt;

                        int abs_x = max_x - this->pupil_x;
                        int abs_y = max_y - this->pupil_y;

                        if(abs_x > abs_y)
                            tab[2] = abs_x;
                       else
                           tab[2] = abs_y;

                        this->pupil_r = tab[2];
			
			return tab;
		}

                /**
                  Funkcja znajduj¹ca odblask na Ÿrenicy.
                  */
                bool find_flash_on_pupil(IplImage *image)
                {
                    qDebug() << filename;
                    IplImage *bin = cvCreateImage(cvSize(image->width, image->height), image->depth, 1);
                    cvThreshold(image, bin, 254, 1, CV_THRESH_BINARY); //binaryzacja z progiem 254, zostaj¹ tylko najjaœniejszepiksele
                    int dark_count = 0;
                    bool no_found = true;
                    for (int i=20; i<bin->height - 20 && no_found; i++) //iteracja po kolumnach
                    {
                        for (int j=20; j<bin->width - 20 && no_found; j++) //iteracja po wierszach
                        {
                            if (cvGetReal2D(bin, i, j) == 1) //znaleziono œwiat³o, sprawdzenie otoczenia
                            {
                                dark_count = 0;
                                for (int k=10; k<20; k++)
                                {
                                    if (cvGetReal2D(image, i-k, j-k) < 50 && cvGetReal2D(image, i+k, j+k) < 50) //sprawdzamy czy jest ciemne na obrazie
                                    {
                                        dark_count++;
                                    }
                                }
                                if (dark_count>6) // liczba czarnych jest wystarczaj¹ca
                                {
                                  no_found = false;
                                  this->pupil_x = j;
                                  pupil_y = i;
                                  pupil_r = 20;
                                }
                            }
                        }
                    }
                    if (no_found)
                    {
                        pupil_x = pupil_y = 0;
                        pupil_r = 100;
                    }
                    cvReleaseImage(&bin);
                    return !no_found;
                }

                /**
                  Funkcja znajduj¹ca œrodek odblasku na Ÿrenicy (dowolny punkt odblasku musi byæ zapisuany w pupil_x i pupil_y)
                  */
                void find_flash_center(IplImage *image)
                {
                    IplImage *bin = cvCreateImage(cvSize(image->width, image->height), image->depth, 1);
                    cvSetReal2D(bin, pupil_y, pupil_x, 1);
                    for (int i = 1; i < 25; i++)
                    {
                        for (int j = 1; j < 25; j++)
                        {
                            if (cvGetReal2D(image, pupil_y + i, pupil_x + j) > 240)
                            {
                                cvSetReal2D(bin, pupil_y + i, pupil_x + j, 255);
                            }
                            if (cvGetReal2D(image, pupil_y - i, pupil_x + j) > 240)
                            {
                                cvSetReal2D(bin, pupil_y - i, pupil_x + j, 255);
                            }
                            if (cvGetReal2D(image, pupil_y + i, pupil_x - j) > 240)
                            {
                                cvSetReal2D(bin, pupil_y + i, pupil_x - j, 255);
                            }
                            if (cvGetReal2D(image, pupil_y - i, pupil_x - j) > 240)
                            {
                                cvSetReal2D(bin, pupil_y - i, pupil_x - j, 255);
                            }
                        }
                        if (cvGetReal2D(image, pupil_y + i, pupil_x) > 240)
                        {
                            cvSetReal2D(bin, pupil_y + i, pupil_x, 255);
                        }
                        if (cvGetReal2D(image, pupil_y, pupil_x - i) > 240)
                        {
                            cvSetReal2D(bin, pupil_y, pupil_x - i, 255);
                        }
                        if (cvGetReal2D(image, pupil_y - i, pupil_x) > 240)
                        {
                            cvSetReal2D(bin, pupil_y - i, pupil_x, 255);
                        }
                        if (cvGetReal2D(image, pupil_y, pupil_x + i) > 240)
                        {
                            cvSetReal2D(bin, pupil_y, pupil_x + i, 255);
                        }
                    }
                    find_center(bin);
                    cvReleaseImage(&bin);
                }

                /** Funkcja realizuj¹ca operacjê eksploduj¹cych okrêgów dla Ÿrenicy. Szuka najwiêkszego zmniejszenia jasnoœci */
                void explode_circle(IplImage *image)
                {
                    // tworzenie tablicy katow
                    int angles_count = 72;
                    double alfa[angles_count];
                    double coss[angles_count];
                    double sins[angles_count];
                    for (int i=0; i < angles_count; i++)
                    {
                        alfa[i] = (2 * M_PI) * i / angles_count;
                        coss[i] = cos(alfa[i]);
                        sins[i] = sin(alfa[i]);
                    }
                    int p_x, p_y, p_r, act_x, act_y;
                    int r = pupil_r + 15;
                    int dif = 0;
                    int dmin = 0;
                    int lastsum = angles_count * 300;
                    int x, y;
                    int sum = cvGetReal2D(image, pupil_y, pupil_x);
                    //IplImage *border_points = cvCreateImage(cvSize(2, angles_count), 1, 1);
                    for (int k=-3; k<=3; k = k +3)
                    {
                        for (int l=-3; l<=3; l = l + 3)
                        {
                            act_x = pupil_x + k;
                            act_y = pupil_y + l;
                            lastsum = angles_count * 300;
                    //act_x = pupil_x;
                    //act_y = pupil_y;
                            for (; r < 60; r++) //zwiêkszanie promienia
                            {
                                sum = 0;
                                for (int j=0; j<angles_count; j++) //przechodzenie po okgrêgu i liczenie jansoœci
                                {
                                    x = act_x + sins[j] * r;
                                    y = act_y + coss[j] * r;
                                    if (x > 0 && y > 0)
                                    {
                                        sum += cvGetReal2D(image, x, y);
                                    }
                                }
                                dif = sum - lastsum;
                                qDebug() << r << " " << dif;
                                if (dif > dmin)
                                {
                                    dmin = dif;
                                    p_r = r;
                                    p_x = act_x;
                                    p_y = act_y;
                                }
                                lastsum = sum;
                            }
                        }
                    }
                    pupil_x = p_x;
                    pupil_r = p_r;
                    pupil_y = p_y;
                    qDebug() << pupil_r;
                }

                /** Funkcja realizuj¹ca operacjê eksploduj¹cych okrêgów dla Ÿrenicy. Szuka najwiêkszego zmniejszenia jasnoœci */
                void explode_rs(IplImage *image)
                {
                    // tworzenie tablicy katow
                    int angles_count = 36;
                    double alfa[angles_count];
                    double coss[angles_count];
                    double sins[angles_count];
                    for (int i=0; i < angles_count; i++)
                    {
                        alfa[i] = (2 * M_PI) * i / angles_count;
                        coss[i] = cos(alfa[i]);
                        sins[i] = sin(alfa[i]);
                     //   qDebug() << i << " " << sin(alfa[i]) << " " << cos(alfa[i]) << " " << ((2 * M_PI) * i / angles_count);
                    }
                    int act_x, act_y, tmp_x, tmp_y;
                    int r = pupil_r + 10;
                    int dif = 0;
                    int dmin = 0;
                    int lastsum = 0;
                    int x, y;
                    int sum = cvGetReal2D(image, pupil_y, pupil_x);
                    //IplImage *border_points = cvCreateImage(cvSize(angles_count, 2), 1, 1);
                    IplImage *border_points = cvCreateImage(cvSize(image->width, image->height), image->depth, 1);
                    for (int i=0; i<border_points->height; i++)
                    {
                        for (int j=0; j<border_points->width; j++)
                        {
                            cvSetReal2D(border_points, i, j, 0);
                        }
                    }
                    cvShowImage("w", border_points);
                    while (cvWaitKey(100) < 0);
                    /*for (int k=-3; k<=3; k = k +3)
                    {
                        for (int l=-3; l<=3; l = l + 3)
                        {*/
                            //act_x = pupil_x + k;
                            //act_y = pupil_y + l;
                    act_x = pupil_x;
                    act_y = pupil_y;
                    //wersja z eksploduj¹cymi promieniami
                    for (int i = 0; i < angles_count; i++) //dla ka¿dego k¹ta
                    {
                        x = act_x + sins[i] * (pupil_r + 9);
                        y = act_y + coss[i] * (pupil_r + 9);
                        lastsum = cvGetReal2D(image, x, y);
                        dmin = 0;
                        for (r = pupil_r + 10; r<60; r++) //zwiêkszanie promienia
                        {
                            x = act_x + sins[i] * r;
                            y = act_y + coss[i] * r;
                          //  qDebug() << sins[i] << ": " << x << " " << coss[i] << ": " << y ;
                            if (x > 0 && y > 0)
                            {
                                sum = cvGetReal2D(image, x, y);
                            }
                            dif = sum - lastsum;
                            if (dif > dmin)
                            {
                                dmin = dif;
                                tmp_x = x;
                                tmp_y = y;
                            }
                            lastsum = sum;
                        }
                        //qDebug() << i << " " << tmp_x << " " << tmp_y;
                        //cvSetReal2D(border_points, 0, i, tmp_x);
                        //cvSetReal2D(border_points, 1, i, tmp_y);
                        if (cvGetReal2D(image, tmp_y-2, tmp_x-2) < 50 ||
                            cvGetReal2D(image, tmp_y+2, tmp_x-2) < 50 ||
                            cvGetReal2D(image, tmp_y-2, tmp_x+2) < 50 ||
                            cvGetReal2D(image, tmp_y+2, tmp_x+2) < 50)
                        {
                                cvSetReal2D(border_points, tmp_y, tmp_x, 255);
                        }
                    }
                    cvShowImage("w", border_points);
                    while (cvWaitKey(100) < 0);
                    find_center(border_points);
                    //cvMinEnclosingCircle(border_points, &center, &f_r);
                    /*p_r = f_r;
                    p_x = center.x;
                    p_y = center.y;*/
                    /*        for (; r < 60; r++) //zwiêkszanie promienia
                            {
                                sum = 0;
                                for (int j=0; j<angles_count; j++) //przechodzenie po okgrêgu i liczenie jansoœci
                                {
                                    x = act_x + sins[j] * r;
                                    y = act_y + coss[j] * r;
                                    if (x > 0 && y > 0)
                                    {
                                        sum += cvGetReal2D(image, x, y);
                                    }
                                }
                                dif = sum - lastsum;
                                qDebug() << r << " " << dif;
                                if (dif < dmin)
                                {
                                    dmin = dif;
                                    p_r = r;
                                    p_x = act_x;
                                    p_y = act_y;
                                }
                                lastsum = sum;
                            }*/
                        /*}
                    }*/
                    /*pupil_x = p_x;
                    pupil_r = p_r;
                    pupil_y = p_y;*/
                    qDebug() << pupil_r;
                }

                /** Funkcja realizuj¹ca operacjê eksploduj¹cych okrêgów dla Ÿrenicy. Szuka najwiêkszego zmniejszenia jasnoœci */
                void find_pupil_by(IplImage *image)
                {
                    //IplImage *border_points = cvCreateImage(cvSize(angles_count, 2), 1, 1);
                    /*for (int i=0; i<border_points->height; i++)
                    {
                        for (int j=0; j<border_points->width; j++)
                        {
                            cvSetReal2D(border_points, i, j, 0);
                        }
                    }*/
                    int serial = 10;
                    int threshold = 50;
                    int x = pupil_x - 15;
                    int y = pupil_y - 15;
                    int count = 0;
                    bool no_found = true;
                    int minx, miny, maxx, maxy;
                    while (--x > 0 && no_found)
                    {
                        if (cvGetReal2D(image, x, pupil_y) > threshold)
                        {
                            if (++count > serial)
                            {
                                minx = x;
                                no_found = false;
                            }
                        }
                        else
                        {
                            count = 0;
                        }
                    }
                    if (!x)
                    {
                        minx = 0;
                    }
                    x = pupil_x + 15;
                    no_found = true;
                    count = 0;
                    while (++x < image->height && no_found)
                    {
                        if (cvGetReal2D(image, x, pupil_y) > threshold)
                        {
                            if (++count > serial)
                            {
                                maxx = x;
                                no_found = false;
                            }
                        }
                        else
                        {
                            count = 0;
                        }
                    }
                    if (x == image->height)
                    {
                        maxx = image->height;
                    }
                    no_found = true;
                    y = pupil_y - 15;
                    count = 0;
                    while (--y > 0 && no_found)
                    {
                        if (cvGetReal2D(image, pupil_x, y) > threshold)
                        {
                            if (++count > serial)
                            {
                                miny = y;
                                no_found = false;
                            }
                        }
                        else
                        {
                            count = 0;
                        }
                    }
                    if (!y)
                    {
                        miny = 0;
                    }
                    no_found = true;
                    y = pupil_y + 15;
                    count = 0;
                    while (++y < image->width && no_found)
                    {
                        if (cvGetReal2D(image, pupil_x, y) > threshold)
                        {
                            if (++count > serial)
                            {
                                maxy = y;
                                no_found = false;
                            }
                        }
                        else
                        {
                            count = 0;
                        }
                    }
                    if (y == image->width)
                    {
                        maxy = image->width;
                    }
                    int add = 20;
                    if (minx > add) minx -= add;
                    if (miny > add) miny -= add;
                    if (maxx < image->height + add) maxx += add;
                    if (maxy < image->width + add) maxy += add;
                    qDebug() << minx << " " << maxx << " " << miny << " " << maxy;
                    cvSetImageROI(image, cvRect(minx, miny, maxx-minx, maxy-miny));
                    IplImage *border_points = cvCreateImage(cvSize(maxx-minx, maxy-miny), image->depth, 1);
                    IplImage *border_points2 = cvCreateImage(cvSize(maxx-minx, maxy-miny), image->depth, 1);
                    //cvSetImageROI(border_points, cvRect(pupil_x - q, pupil_y -q, 2*q, 2*q));
                    cvThreshold(image, border_points, 50, 1, CV_THRESH_BINARY_INV);
                    //border_points = Image::clearborders(border_points);

                    //cvResetImageROI(border_points);
                    /*while ()
                    {

                    }*/
                    cvThreshold(border_points, border_points2, 0, 255, CV_THRESH_BINARY);
                    cvShowImage("w", border_points2);
                    while (cvWaitKey(100) < 0);
                    IplConvKernel* element = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_ELLIPSE, NULL);
                    cvErode(border_points, border_points, element);
                    cvDilate(border_points, border_points, element);
                    cvThreshold(border_points, border_points2, 0, 255, CV_THRESH_BINARY);
                    cvShowImage("w", border_points2);
                    while (cvWaitKey(100) < 0);
                    border_points = Image::clearborders(border_points);
                    cvThreshold(border_points, border_points2, 0, 255, CV_THRESH_BINARY);
                    cvShowImage("w", border_points2);
                    while (cvWaitKey(100) < 0);
                    //find_center(border_points);

                }
		
};
