/**
 * @file
 * @author  David Freire-Obregon <dfreire@siani.es>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is Creative Commons License; you can redistribute it and/or
 * modify it under the following terms:
 *
 *    Attribution: You must give appropriate credit, provide a link to the license, and indicate if changes were made.
 *    You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
 *    Reference:
 *    ------------
 *    Freire-Obregon, D. Castrillon-Santana, M. Ramon-Balmaseda, E. Lorenzo-Navarro, J. Automatic clothes segmentation for soft biometrics. Proceedings of the IEEE International Conference on Image Processing (ICIP) 2014.
 *    ------------
 *
 *    NonCommercial: You may not use the material for commercial purposes.
 *
 *    ShareAlike: If you remix, transform, or build upon the material, you must distribute your contributions under the same license as the original.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the Creative Commons License for more details at
 * http://creativecommons.org/licenses/by-nc-sa/4.0/
 *
 * @section DESCRIPTION
 *
 * C_Histogram is the histogram class. Histograms are created, updated and erased in this class. Also provides a way to print the histograms in images.
 
  * C_Histogram_Comparison is a class that allows the histograms comparison considering the C_Histogram class.

 */


#if ! defined ( C_HISTOGRAM_H )
#define C_HISTOGRAM_H

#if ! defined ( C_TRITOM_H )
    #include <opencv2/opencv.hpp>

	//#include <time.h>
	#include <stdio.h>
	#include <string.h>
	#include <vector>
	using namespace::std;
#endif

#include <opencv2/legacy/compat.hpp>

class C_Histogram
{
private:
	IplImage* imgHist;
	vector<CvHistogram *> *Hist_list;
	vector<float *> *Data;
	vector<char *> *Hist_Name;
	int histogram_flag;
	int draw_flag;
	int diff;


	int numBins;
	float range[2];
	float *ranges[1];

    //Variables to generate the graphical infomation
    int offset;  /**< Offset to adjust the text info. */
    int width; /**< Graphs width */
    int height;   /**< Graphs height */
    float scaleX, scaleY;

public:

	

    /**
     * Constructor that initializes the class.
     *
     * @param numBins Number of bins per histogram.
     * @param range_min Min range of data.
     * @param range_max Max range of data.
     */
	C_Histogram(int numBins, int range_min, int range_max){
		this->Hist_list= new vector <CvHistogram *>;
		this->Data= new vector <float *>;
		this->Hist_Name= new vector <char *>;
		this->numBins = numBins;
		this->range[0] = float(range_min);
		this->range[1] = float(range_max);
		this->ranges[0] = this->range;
		this->imgHist = NULL;
		this->histogram_flag=0;
		this->draw_flag=0;
        //Used when number of bins are required to be lower. It suposes 256 as the max number of input values. This number can be changed depending on the histogram max value.
		this->diff = 256 / this->numBins;

		//variables for the GUI windows
		this->offset=60;
		this->width=256;
		this->height=128;

	}

    
    /**
     * Destructor, clears memory (just the pointer, not the object)
     */
	~C_Histogram(){
		vector<CvHistogram *>::iterator Iter;

		for(Iter = (*this->Hist_list).begin();Iter != (*this->Hist_list).end();Iter++){
			delete(*Iter);
		}

		(*this->Hist_list).clear();
		delete this->Hist_list;

		vector<float *>::iterator Iter2;
		for(Iter2 = (*this->Data).begin();Iter2 != (*this->Data).end();Iter2++){
			delete (*Iter2);
		}
		(*this->Data).clear();
		delete this->Data;

		vector<char *>::iterator Iter3;
		for(Iter3 = (*this->Hist_Name).begin();Iter3 != (*this->Hist_Name).end();Iter3++){
			delete (*Iter3);
		}
		(*this->Hist_Name).clear();
		delete this->Hist_Name;
		if (this->imgHist != NULL)
			cvReleaseImage( &this->imgHist );

	};


    /**
     * Returns the number of bins.
     *
     */
	int Get_NumBins(){
		return this->numBins;
	}

    /**
     * Returns the histogram.
     *
     */
	vector<CvHistogram *> * Get_Hist_List(){
		if(!this->histogram_flag)
			this->Create_Histograms();
		return (this->Hist_list);
	}
    
    
    /**
     * Returns the offset in order to adjust the text info.
     *
     */
    int Get_Offset(){
        return (this->offset);
    }
    
    /**
     * Returns the graph width.
     *
     */
    int Get_Width(){
        return (this->width);
    }
    
    /**
     * Returns the graph height.
     *
     */
    int Get_Height(){
        return (this->height);
    }
    
    /**
     * Returns the X Scale.
     *
     */
    int Get_ScaleX(){
        return (this->scaleX);
    }
    
    /**
     * Returns the Y Scale.
     *
     */
    int Get_ScaleY(){
        return (this->scaleY);
    }
    
    

    /**
     * An element is added to the histogram. Returns 0 if the identifier does not fit to the number of histograms. Otherwise, returns 1.
     *
     * @param element New data.
     * @param id_hist Histogram identifier inside the histogram vector. If id_hist is bigger than the vector size, a new histogram is created.
     * @param hist_name Histogram name.
     */
	int Add_Element(int element, int id_hist=0, char *hist_name=NULL){

		if (id_hist>int((*this->Data).size())){
			if (id_hist==((*this->Data).size()+1)){
				float *new_hist=new float[this->numBins];
				for(int i=0;i<this->numBins;new_hist[i]=0,i++);
				(*(this->Data)).push_back(new_hist);
				if(hist_name != NULL){
					char *name = new char[50];
					strcpy(name,hist_name);
					(*(this->Hist_Name)).push_back(name);
				}else{
				    char *name = new char[50];
					strcpy(name,"Unknown");
					(*(this->Hist_Name)).push_back(name);
				}
			}else{
				return 0; //the id_hist is bigger than the size of the histogram vector + 1.
			}
		}
		vector<float *>::iterator Iter;
		Iter = (*this->Data).begin();
		Iter+=id_hist-1;
		(*Iter)[(int(element/this->diff))]++;
		return 1;
	}

    
    /**
     * Creates a histogram.
     *
     */
	int Create_Histograms(){
		this->histogram_flag=1;
		vector<float *>::iterator Iter;
		for(Iter = (*this->Data).begin();Iter != (*this->Data).end();Iter++){
			CvHistogram *hist = new CvHistogram;
			cvMakeHistHeaderForArray( 1, &this->numBins, hist, (*Iter), this->ranges, 1 );
			cvNormalizeHist(hist,1); //Histogram is normalized between 0 and 1.
		    (*(this->Hist_list)).push_back(hist); //One-dimensional histogram
		}

		return 1;
	}


    /**
     * Draws histograms.
     *
     * @param scaleX Scale on the x axis.
     * @param scaleY Scale on the Y axis.
     */
	void DrawHistograms(float scaleX=1, float scaleY=1)
	{
		this->draw_flag=1;
		float histMax = 0;
		int mcolor=0;
		CvScalar color[] = { CV_RGB(0,0,255), CV_RGB(0,255,0),
                          CV_RGB(255,0,0), CV_RGB(255,255,255) };

		CvFont font;
		double hScale=0.5;
		double vScale=0.5;
		int    lineWidth=1;
		cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);

		if(!this->histogram_flag)
			this->Create_Histograms();

		this->scaleX=scaleX;
		this->scaleY=scaleY;

		float onesize= (128*scaleY+offset);
		this->imgHist = cvCreateImage(cvSize(int(this->width*scaleX), 10+int(this->Hist_list->size()*(this->height*scaleY+this->offset))), 8 ,3);
		cvZero(imgHist);


		vector<CvHistogram *>::iterator Iter;
		vector<char *>::iterator Iter_name;
		Iter_name = (*this->Hist_Name).begin();

		for(Iter = (*this->Hist_list).begin();Iter != (*this->Hist_list).end();Iter++){
			cvGetMinMaxHistValue((*Iter), 0, &histMax, 0, 0);
			for(int i=0;i<this->numBins-1;i++)
			{

				float histValue = cvQueryHistValue_1D((*Iter), i);
				float nextValue = cvQueryHistValue_1D((*Iter), i+1);


				CvPoint pt1 = cvPoint(int(i*scaleX), int((onesize*mcolor)+(128*scaleY)+offset));
				CvPoint pt2 = cvPoint(int(i*scaleX+scaleX), int((onesize*mcolor)+(128*scaleY)+offset));
				CvPoint pt3 = cvPoint(int(i*scaleX+scaleX), int((onesize*mcolor)+((128-nextValue*128/histMax)*scaleY)+offset));
				CvPoint pt4 = cvPoint(int(i*scaleX), int((onesize*mcolor)+((128-histValue*128/histMax)*scaleY)+offset));

				int numPts = 5;
				CvPoint pts[] = {pt1, pt2, pt3, pt4, pt1};

				cvFillConvexPoly(imgHist, pts, numPts, color[mcolor]);
			}
			cvPutText (imgHist,(*Iter_name),cvPoint(80,int((onesize*mcolor)+15)), &font, cvScalar(255,255,255));
			Iter_name++;
			mcolor++;
		}
	}


    /**
     * Returns a histogram image.
     *
     * @param img Histogram image pointer.
     */
	void Get_Histogram_Image(IplImage **img){
		if(!this->draw_flag)
			this->DrawHistograms(); //If the histogram does not exist, it is created considering a 1 to 1 scale.
		*img = this->imgHist;
	}


    /**
     * Erases the histogram image. It is too expensive to keep an image per trixel.
     *
     */
	void Clear_Histogram_Image(){
		if (this->draw_flag)
			cvReleaseImage( &this->imgHist );
		this->draw_flag=0;
		this->imgHist=NULL;
	}

    
    /**
     * Equalizes an image due to http://opencv-code.com/Histogram_Equalization. Details of the imput image are emphasized.
     *
     * @param src Source image.
     * @param dst Equalized image.
     */
	void Equalize_Image(IplImage* src, IplImage* dst)
	{
	  IplImage* hsv, * h, * s, * v;

	  if (src->nChannels == 3)
	  {
		hsv = cvCreateImage(cvGetSize(src), 8, 3);
		h   = cvCreateImage(cvGetSize(src), 8, 1);
		s   = cvCreateImage(cvGetSize(src), 8, 1);
		v   = cvCreateImage(cvGetSize(src), 8, 1);

		cvCvtColor(src, hsv, CV_BGR2HSV);
		cvSplit(hsv, h, s, v, NULL);

		cvEqualizeHist(v, v);

		cvMerge(h, s, v, NULL, hsv);
		cvCvtColor(hsv, dst, CV_HSV2BGR);
	  }
	  else if (src->nChannels == 1)
		cvEqualizeHist(src, dst);

	  if (hsv) cvReleaseImage(&hsv);
	  if (h) cvReleaseImage(&h);
	  if (s) cvReleaseImage(&s);
	  if (v) cvReleaseImage(&v);
	}
};








class C_Histogram_Comparison
{
private:
	IplImage* dataImg;  /**< Image containig data. */
	vector<double> *correl;/**< Measure: correlation. */
	vector<double> *chisqr; /**< Measure: Chi square. */
	vector<double> *intersec; /**< Measure: Intersection. */
	vector<double> *bhatta; /**< Measure: Bhattacharyya distance. */
	vector<double> *emd_L1; /**< Measure: Earth Mover's Distance (EMD) on 1D histograms. */
	int comparison_flag;



    /**
     * Returns the Earth Mover's Distance (EMD) on 1D histograms
     *
     * @param h1 Histogram 1.
     * @param h2 Histogram 2.
     * @param nbins Number of bins.
     */
	float EmdDist_L1(CvHistogram *h1, CvHistogram *h2, int nbins)
	{
		float	d	= 0.;
		float	dk	= 0.;
		for(int k=0; k< nbins; ++k)	{
			dk	+= cvQueryHistValue_1D(h1, k)-cvQueryHistValue_1D(h2, k);
			d	+= fabs(dk);
		}
		return d/(nbins-1); //returns the normalized distance. The largest distance is 255 (256-1), p.e., if a histogram is all 255s and the other is all 0s.
	}


	public:


    /**
     * Constructor that initializes the class.
     *
     */
	C_Histogram_Comparison(){
		this->dataImg = NULL;
		this->correl = new vector<double>;
		this->chisqr = new vector<double>;
		this->intersec = new vector<double>;
		this->bhatta = new vector<double>;
		this->emd_L1 = new vector<double>;
		this->comparison_flag=0;
	}

    /**
     * Destructor, clears memory (just the pointer, not the object).
     */
    ~C_Histogram_Comparison(){
		if( this->dataImg != NULL)
			cvReleaseImage( &this->dataImg );
		this->correl->clear();
		delete (this->correl);
		this->chisqr->clear();
		delete (this->chisqr);
		this->intersec->clear();
		delete (this->intersec);
		this->bhatta->clear();
		delete (this->bhatta);
		this->emd_L1->clear();
		delete (this->emd_L1);
	};

    
    /**
     * Generates the comparison between histograms.
     *
     * @param h1 Histogram 1.
     * @param h2 Histogram 2.
     * @param draw_data if graphic data need to be generated.
     */
	int Generate_Comparison(C_Histogram *h1, C_Histogram *h2, int draw_data=1){
		this->comparison_flag=1;
		vector<CvHistogram *> *hst_1;
		vector<CvHistogram *> *hst_2;

		hst_1 = h1->Get_Hist_List();
		hst_2 = h2->Get_Hist_List();

		if (hst_1->size() != hst_2->size())
			return 0;


		vector<CvHistogram *>::iterator Iter, Iter2;
		int i=0;

		//Previous information is cleared
		this->correl->clear();
		this->chisqr->clear();
		this->intersec->clear();
		this->bhatta->clear();
		this->emd_L1->clear();

		Iter2 = hst_2->begin();
		for(Iter = hst_1->begin();Iter != hst_1->end();Iter++){
				this->correl->push_back(cvCompareHist((*Iter),(*Iter2),CV_COMP_CORREL));
				this->chisqr->push_back(cvCompareHist((*Iter),(*Iter2),CV_COMP_CHISQR));
				this->intersec->push_back(cvCompareHist((*Iter),(*Iter2),CV_COMP_INTERSECT));
				this->bhatta->push_back(cvCompareHist((*Iter),(*Iter2),CV_COMP_BHATTACHARYYA));
				this->emd_L1->push_back(this->EmdDist_L1((*Iter),(*Iter2),h1->Get_NumBins()));
				i++;
				Iter2++;
		}
		//Graphical information is generated by default, but it might not be necessary.
		if(draw_data)
			Draw_Data(h1,h2);
		return 1;
	}

    
    /**
     * Draws the comparison between histograms.
     *
     * @param h1 Histogram 1.
     * @param h2 Histogram 2.
     */
	void Draw_Data(C_Histogram *h1, C_Histogram *h2){
		IplImage *img1,*img2;
		CvFont font;
		double hScale=0.5;
		double vScale=0.5;
		int    lineWidth=1;
		cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);
		char numb[50];
		int sep;


		h1->Get_Histogram_Image(&img1);
		h2->Get_Histogram_Image(&img2);

		sep = int((h1->Get_ScaleY()*h1->Get_Height())/5); //offset of each data per channel. There are 5 measures per channel: chisqur, intersect, bhatta, correl and emd_L1.


		if(this->dataImg != NULL){
			cvReleaseImage(&this->dataImg);
			this->dataImg=NULL;

		}
		this->dataImg= cvCreateImage(cvSize((cvGetSize(img1).width*2)+300, cvGetSize(img1).height), img1->depth, img1->nChannels);
		cvZero(this->dataImg);

		cvSetImageROI(img1, cvRect(0,0, cvGetSize(img1).width, cvGetSize(img1).height));
		cvSetImageROI(this->dataImg, cvRect(0,0, cvGetSize(img1).width, cvGetSize(img1).height));
		cvCopy(img1, this->dataImg);
		cvResetImageROI(img1);
		cvSetImageROI(img2, cvRect(0,0, cvGetSize(img2).width, cvGetSize(img2).height));
		cvSetImageROI(this->dataImg, cvRect(cvGetSize(img1).width+300,0, cvGetSize(img2).width, cvGetSize(img2).height));
		cvCopy(img2, this->dataImg);
		cvResetImageROI(this->dataImg);
		cvResetImageROI(img2);




		float onesize= (128*h1->Get_ScaleY()+h1->Get_Offset());

		for(int i=0;i<int(this->correl->size());i++){
			sprintf(numb, "Correl: %.3f", (*(this->correl->begin()+i)));
			cvPutText (this->dataImg,numb,cvPoint(cvGetSize(img1).width+100,int(h1->Get_Offset() + onesize*i)), &font, cvScalar(255,255,255));
			sprintf(numb, "Chisqr: %.3f", (*(this->chisqr->begin()+i)));
			cvPutText (this->dataImg,numb,cvPoint(cvGetSize(img1).width+100,int(1*sep+(h1->Get_Offset() + onesize*i))), &font, cvScalar(255,255,255));
			sprintf(numb, "Intersect: %.3f", (*(this->intersec->begin()+i)));
			cvPutText (this->dataImg,numb,cvPoint(cvGetSize(img1).width+100,int(2*sep+(h1->Get_Offset() + onesize*i))), &font, cvScalar(255,255,255));
			sprintf(numb, "Bhatta: %.3f", (*(this->bhatta->begin()+i)));
			cvPutText (this->dataImg,numb,cvPoint(cvGetSize(img1).width+100,int(3*sep+(h1->Get_Offset() + onesize*i))), &font, cvScalar(255,255,255));
			sprintf(numb, "EMD L1: %.3f", (*(this->emd_L1->begin()+i)));
			cvPutText (this->dataImg,numb,cvPoint(cvGetSize(img1).width+100,int(4*sep+(h1->Get_Offset() + onesize*i))), &font, cvScalar(255,255,255));
		}

		//To free space. The cost is speed.
		h1->Clear_Histogram_Image();
		h2->Clear_Histogram_Image();
	}
    
    
    /**
     * Returns the data image.
     *
     * @param img Returned image.
     */
	void Get_Image(IplImage **img){
		*img = this->dataImg;
	}

    /**
     * Returns the comparison measure.
     *
     * @param ptr Pointer to the measure data.
     * @param op The measure required: correlation(0), chi square (1), intersection (2), Bhattacharyya distance (3) and EMD (4).
     */
	void Get_Data(vector<double> **ptr, int op){
		if(!this->comparison_flag){
			printf("\nPlease, first compare both histograms (Generate_Comparison)");
			(*ptr)=NULL;
			return;
		}
		switch(op){
			case 0:
				(*ptr)=this->correl;
				break;
			case 1:
				(*ptr)=this->chisqr;
				break;
			case 2:
				(*ptr)=this->intersec;
				break;
			case 3:
				(*ptr)=this->bhatta;
				break;
			case 4:
				(*ptr)=this->emd_L1;
		}
	}


};



#endif

