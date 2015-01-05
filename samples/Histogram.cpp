//
//  main.cpp
//  Tritom 1.0
//
//  Created by dfreire on 02/01/15.
//  Copyright (c) 2015 dfreire. All rights reserved.
//
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "tritom/C_DMUM.h"


IplImage *src_img;
IplImage *tmp_img;
IplImage *hist_img;
CvPoint old_p;
CvPoint new_p;
int count_click;
C_Tritom *tritom;



void Histogram_Comparison( int event, int x, int y, int flags, void* param )
{
    
    
    
    if( !src_img )
        exit(0);
    
    //   printf("\nOUT x=%d\ty=%d",x,y);
    
    if( event == CV_EVENT_LBUTTONDOWN ){
        if(!count_click){
            old_p.x = x;
            old_p.y = y;
            cvReleaseImage(&tmp_img);
            tmp_img = cvCloneImage(src_img);
            tritom->Print_All_Trixels(tmp_img, CV_RGB(255, 0, 255));
            tritom->Highlight_Trixel(cvPoint(x,y),tmp_img,CV_RGB(255,255,255));
            count_click++;
        }else{
            new_p.x = x;
            new_p.y = y;
            tritom->Highlight_Trixel(cvPoint(x,y),tmp_img,CV_RGB(255,255,255));
            tritom->DrawHist_Comp(old_p,new_p,tmp_img,CV_RGB(255,255,255),&hist_img);
            count_click=0;
        }
    }
    
}



int main()
{
    C_DMUM *DMUMClusters;
    
    int c;
    
    src_img=NULL;
    tmp_img=NULL;
    hist_img=NULL;
    old_p.x = -1;
    old_p.y = -1;
    new_p.x = -1;
    new_p.y = -1;
    count_click=0;
    
    int args[1];
    args[0]=1; //Histograms are considered
    
    
    
    IplImage *img =cvLoadImage("/Users/dfreire/Dropbox/AAAAAA_PARAPPAPER_TGRABCUT/guapas/7.jpg");
    
    
    //for(int i=0;i<300;i++){
    //    printf("%d\n",i);
    
    //Se crea el objeto y se le pasa la imagen
    DMUMClusters = new C_DMUM(img);
    tritom = new C_Tritom(img, args);
    
    //Se genera la estructura de triangulos
    if (!DMUMClusters->Compute_Trixels(img, 1, tritom))
        exit(0);
    
    delete DMUMClusters;
    
    
    
    
    
    src_img = cvCloneImage(img);
    tmp_img = cvCloneImage(src_img);
    tritom->Print_All_Trixels(tmp_img, CV_RGB(255, 0, 255));
    
    
    //Image Window
    cvNamedWindow( "TriToM", 0 );
    cvNamedWindow( "TriToM", CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO );
    //Histograms Window
    cvNamedWindow( "Histograms", 0 );
    cvNamedWindow( "Histograms", CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO );
    //Mouse Callback
    cvSetMouseCallback("TriToM", Histogram_Comparison, 0 );
    
    printf("\nPlease, press Esc to exit");
    
    //Main loop
    for(;;)
    {
        c = cvWaitKey(10);
        
        if( (char)c == 27 ) //Esc
            break;
        
        cvShowImage( "TriToM", tmp_img );
        if(hist_img != NULL)
            cvShowImage( "Histograms", hist_img );
        
    }
    
    cvDestroyWindow( "TriToM" );
    cvDestroyWindow( "Histograms" );
    
    
    if(tritom != NULL){
        delete tritom;
        if(src_img != NULL)
            cvReleaseImage(&src_img);
        if(tmp_img != NULL)
            cvReleaseImage(&tmp_img);
        if(hist_img != NULL)
            cvReleaseImage(&hist_img);
    }
    
    
    //}
    
    
    
    cvReleaseImage(&img);
    printf("\nEnd");
    return 0;
}