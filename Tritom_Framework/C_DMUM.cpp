#include "C_DMUM.h"

#include <opencv2/legacy/legacy.hpp>

//using namespace std;


CvSubdiv2D* C_DMUM::init_delaunay( CvMemStorage* storage,
                                 CvRect rect )
{
    CvSubdiv2D* subdiv;
    
    subdiv = cvCreateSubdiv2D( CV_SEQ_KIND_SUBDIV2D, sizeof(*subdiv),
                              sizeof(CvSubdiv2DPoint),
                              sizeof(CvQuadEdge2D),
                              storage );
    cvInitSubdivDelaunay2D( subdiv, rect );
    
    return subdiv;
}


C_DMUM::C_DMUM(IplImage *img)
{
	for (int s = 0; s < N_SCALES; s++)
	{
		_scales[s] = cvCreateImage(cvSize(img->width/pow(2.0f,(float)s), img->height/pow(2.0f,(float)s)), img->depth, img->nChannels);
		//_scales[s] = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
		background[s] = cvCloneImage(_scales[s]);
		gray[s] = cvCreateImage(cvGetSize(_scales[s]),  IPL_DEPTH_8U, 1);
		dx[s] = cvCreateImage(cvGetSize(_scales[s]), IPL_DEPTH_32F, 1);
		dy[s] = cvCreateImage(cvGetSize(_scales[s]), IPL_DEPTH_32F, 1);
		mag[s] = cvCreateImage( cvGetSize(_scales[s]), IPL_DEPTH_32F, 1);

		dist[s] = cvCreateImage( cvGetSize(_scales[s]), IPL_DEPTH_32F, 1 );
		integral[s] = cvCreateImage( cvSize(_scales[s]->width + 1, _scales[s]->height +1), IPL_DEPTH_64F, 1 );
		integralc[s] = cvCreateImage( cvSize(_scales[s]->width + 1, _scales[s]->height +1), IPL_DEPTH_64F, 3 );

		cvZero(dist[s]);
		cvZero(mag[s]);
	}
	int patchSize = 7;
	patchdxA = cvCreateImage(cvSize(patchSize, patchSize), IPL_DEPTH_32F, 1);
	patchdyA = cvCreateImage(cvSize(patchSize, patchSize), IPL_DEPTH_32F, 1);
	patchdxB = cvCreateImage(cvSize(patchSize, patchSize), IPL_DEPTH_32F, 1);
	patchdyB = cvCreateImage(cvSize(patchSize, patchSize), IPL_DEPTH_32F, 1);


	collage = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 3);
	cvZero(collage);

}

C_DMUM::~C_DMUM()
{
	for (int s = 0; s < N_SCALES; s++)
	{
		cvReleaseImage(&_scales[s]);
		cvReleaseImage(&background[s]);
		cvReleaseImage(&gray[s]);
		cvReleaseImage(&dx[s]);
		cvReleaseImage(&dy[s]);
		cvReleaseImage(&mag[s]);
		cvReleaseImage(&dist[s]);
		cvReleaseImage(&integral[s]);
		cvReleaseImage(&integralc[s]);
	}

	cvReleaseImage(&patchdxA);
	cvReleaseImage(&patchdyA);
	cvReleaseImage(&patchdxB);
	cvReleaseImage(&patchdyB);

	cvReleaseImage(&collage);


}





double C_DMUM::_sumaRectangulo(IplImage *integral, int j, int i, int q)
{
	int x1 = j - q;
	int x2 = j + q;
	int y1 = i - q + 1;
	int y2 = i + q + 1;

	x1 = max(0,x1);
	x2 = min(integral->width-1, x2);
	y1 = max(0, y1);
	y2 = min(integral->height-1, y2);


	double sumx2y2 = ((double*)(integral->imageData + integral->widthStep*(y2)))[x2];
	double sumx1y2 = ((double*)(integral->imageData + integral->widthStep*(y2)))[x1];
	double sumx2y1 = ((double*)(integral->imageData + integral->widthStep*(y1)))[x2];
	double sumx1y1 = ((double*)(integral->imageData + integral->widthStep*(y1)))[x1];
	double sum = (sumx2y2 - sumx1y2 - sumx2y1 + sumx1y1);

	return sum;
}







void C_DMUM::DMUM(IplImage *img, float gamma)
{
	//cvSmooth(img, img, CV_GAUSSIAN, 3, 3);
	cvCopy(img, _scales[0]);
	//Actualiza escalas
	for (int s = 0; s < N_SCALES; s++)
	{
		if (s > 0)
		{
			cvPyrDown(_scales[s-1], _scales[s]);
			//cvCopy(_scales[s-1], _scales[s]);
		}
		cvZero(dist[s]);
		edges(_scales[s], s);
		double min, max;
		cvMinMaxLoc(mag[s], &min, &max);
		cvConvertScale(mag[s], mag[s], 1.0 / (max- min));
		cvIntegral( mag[s], integral[s]);
		cvIntegral( _scales[s], integralc[s]);
		int maxQsize;
		if(_scales[s]->width <= _scales[s]->height)
			maxQsize = _scales[s]->width;
		else
			maxQsize = _scales[s]->height;


		double min_dist = 1e30;
		double max_dist = 0;

		int i = 0;

		float g = gamma;
		//float g = gamma / (float)(s*s+1);

		//#pragma omp parallel for private (i)
		for (i = 0; i < _scales[s]->width*_scales[s]->height; i++)
		{
			int y = i / _scales[s]->width;
			int x = i % _scales[s]->width;

			int q = 1;
			double area_gradient = (((float*)(mag[s]->imageData))[i]) * g;

			double CDT = area_gradient;

			while ((area_gradient <= 1) && (q < maxQsize))
			{
				double area_weight = area_gradient;
				CDT += (area_weight );

				q++;

				area_gradient = ((_sumaRectangulo(integral[s], x, y, q)))*g;
			}

			double distance = (double)q - CDT;


			((float*)(dist[s]->imageData))[i] = (float)distance;
			if (distance < min_dist)
				min_dist = distance;
			if (distance > max_dist)
				max_dist = distance;
		}
		

	}
}



void C_DMUM::edges(IplImage *img, int s)
{
	if (img->nChannels > 1)
	{
		cvCvtColor(img, gray[s], CV_BGR2GRAY);
	}
	else
		cvCopy(img, gray[s]);

	cvZero(mag[s]);

	cvSobel(gray[s], dx[s], 1,0);
	cvSobel(gray[s], dy[s], 0,1);

	float avg = 0;

	for (int i = 0; i < mag[s]->height*mag[s]->width; i++)
	{
		float valX = ((float*)(dx[s]->imageData ))[i];
		float valY = ((float*)(dy[s]->imageData ))[i];

		//float m = sqrt(valX*valX + valY*valY);
		float m = max(fabs(valX), fabs(valY));

		((float*)(mag[s]->imageData ))[i]  = m;
		avg += m;
	}
	avg /= mag[s]->height*mag[s]->width;
	for (int i = 0; i < mag[s]->height*mag[s]->width; i++){
		if ((((float*)(mag[s]->imageData ))[i] - avg*0.5) > 0)
			((float*)(mag[s]->imageData ))[i] = (((float*)(mag[s]->imageData ))[i] - avg*0.5);
		else
			((float*)(mag[s]->imageData ))[i] = 0;


	}


	//add_to_collage(mag[s], 319, 0);
}


void C_DMUM::compute_minima()
{
	final_extrema.clear();
	for (int s = 0; s < N_SCALES; s++)
	{
		for (int i = 0; i < _scales[s]->width*_scales[s]->height; i++)
		{
			int y = i / _scales[s]->width;
			int x = i % _scales[s]->width;
			if ((x < 1) || (y < 1) || (x > _scales[s]->width-2) || (y > _scales[s]->height-2)) continue;
			float str = ((float*)(dist[s]->imageData + dist[s]->widthStep*y))[x];

			bool maximo = true;
			int w = 1;

			if (str > 0)
			{
				for (int a = y-w; a <= y+w; a++)
				{
					for (int b = x-w; b <= x+w; b++)
					{
						if ((a == y) && (b == x)) continue;
						if ((a < w) || (a > _scales[s]->height-w) || (b < w) || (b > _scales[s]->width-w)) continue;

						float n = ((float*)(dist[s]->imageData + dist[s]->widthStep*a))[b];
						if (str <= n)
						{
							maximo = false;
							break;
						}
						if (maximo == false)
							break;
					}
					if (maximo == false)
					break;
				}
			}
			else
				maximo = false;

			if (maximo)
			{
				C_DMUM_Result r = C_DMUM_Result();
				r.p.x = x;
				r.p.y = y;
				r.value = str;
				final_extrema.push_back(r);
			}
		}
	}
}



int C_DMUM::Generate_tritom_from_DMUM(C_Tritom *titrom)
{
    CvMemStorage* storage;
    IplImage *show_img;
    
    storage = cvCreateMemStorage(0);
    
    CvSubdiv2D* subdiv;
    
    
    subdiv = this->init_delaunay( storage, cvRect(0, 0, _scales[0]->width, _scales[0]->height));
        
    show_img = cvCloneImage(_scales[0]);
    cvZero(show_img);
        
    float max_dist = 0;
        
    for (int i = 0; i < show_img->width*show_img->height; i++)
    {
        int y = i / show_img->width;
        int x = i % show_img->width;
        if ((x < 1) || (y < 1) || (x > show_img->width-2) || (y > show_img->height-2)) continue;
        float str = ((float*)(dist[0]->imageData + dist[0]->widthStep*y))[x];
        if (str > max_dist)
            max_dist = str;
            
        bool minimo = true;
        int w = 1;
            
        
        for (int a = y-w; a <= y+w; a++)
        {
            for (int b = x-w; b <= x+w; b++)
            {
                if ((a == y) && (b == x)) continue;
                if ((a < w) || (a > show_img->height-w) || (b < w) || (b > show_img->width-w)) continue;
                        
                float n = ((float*)(dist[0]->imageData + dist[0]->widthStep*a))[b];
                if (str >= n)
                {
                    minimo = false;
                    break;
                }
                if (minimo == false)
                    break;
            }
            if (minimo == false)
                break;
        }
        
            
        if (minimo)
        {
            C_DMUM_Result r = C_DMUM_Result();
            r.p.x = x;
            r.p.y = y;
            r.value = str;
            r.index = (float)rand()/(float)RAND_MAX;
                
            cvCircle (show_img, cvPoint(x,y), str, CV_RGB(0,255,0), 1);
            final_extrema.push_back(r);
                
            CvPoint2D32f fp = cvPoint2D32f( r.p.x, r.p.y);
            cvSubdivDelaunay2DInsert( subdiv, fp );
                
            //Update C_Tritom
            titrom->Insert_Delaunay_Point(r.p);
        }
            
    }
    
    cvReleaseImage(&show_img);
    cvReleaseMemStorage( &storage );
    
    //Trixel structure is initialized
    titrom->init_Trixels();
        
    return 1;
}



int C_DMUM::Compute_Trixels(IplImage *img, float gamma, C_Tritom *titrom)
{
	//printf("\nGamma = %f",gamma);
	this->DMUM(img, gamma);
	this->compute_minima();
	return this->Generate_tritom_from_DMUM(titrom);
}
