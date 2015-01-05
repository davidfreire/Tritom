//
//  main_Tritom.cpp
//  Tritom 1.0
//
//  Created by dfreire on 02/01/15.
//  Copyright (c) 2015 dfreire. All rights reserved.
//

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "tritom/C_DMUM.h"



int main()
{
    C_DMUM *DMUMClusters = NULL;
    C_Tritom *tritom = NULL;
    
    
    
    IplImage *img =cvLoadImage("/Users/dfreire/Dropbox/AAAAAA_PARAPPAPER_TGRABCUT/guapas/7.jpg");
    
    
    //for(int i=0;i<300;i++){
    //    printf("%d\n",i);
    
    //Se crea el objeto y se le pasa la imagen
    DMUMClusters = new C_DMUM(img);
    tritom = new C_Tritom(img);
    
    //Se genera la estructura de triangulos
    if (!DMUMClusters->Compute_Trixels(img, 1,tritom))
        exit(0);
    
    delete DMUMClusters;
    
    
   
    
    if(tritom != NULL){
        tritom->Print_All_Trixels(img, CV_RGB(255, 0, 255));
        cvSaveImage("/Users/dfreire/Dropbox/_Code/C++/Tritom 1.0/prueba3.png", img);
        delete tritom;
    }
    
    
    //}
    
    
    
    cvReleaseImage(&img);
    printf("\nEnd");
    return 0;
}