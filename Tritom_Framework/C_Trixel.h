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
 * C_Trixel provides the class for the trixels.
 */
#if ! defined ( C_TRIXEL_H )
#define C_TRIXEL_H


#if ! defined ( C_TRITOM_H )
    #include <opencv2/opencv.hpp>
	#include <stdio.h>
	#include <string.h>
	#include <vector>
	using namespace::std;
#endif


#include "C_Edge.h"
#include "C_Vertex.h"


#include "C_Histogram.h"
class C_Histogram;



class C_Trixel
{
private:

	//Variables
	int id_trixel;                    /**< Trixel ID. It is unique per each trixel. */
	double num_pixels;                  /**< Number of pixels inside the trixel. */
	vector <C_Edge *> *edge_list;     /**< The three edges of the trixel. */
	vector <C_Vertex *> *vertex_points; /**< The three vertices of the trixel. */
	CvPoint centroid;                   /**< The centroid of the trixel. */
	
    int valid_bit;                      /**< The valid bit. Some trixels may have a 0 area due to the position of their vertices. They are not considered as valid trixels (valid_bit is 0). */

	//Color Features
	CvScalar color;                     /**< The RGB mean color inside the trixel. */
    
    //Histogram
	C_Histogram *feat_Hist;             /**< Trixel histogram. */
   
    
    vector <CvPoint> *pixels;           /**< Location of the pixels that belong to the trixel. */


	//Geometric Features
	int radius; /**< Radius of the circle circumscribed about a triangle. The center of the circle is located at the trixel centroid*/


    
    
    
    
    /**
     * Returns 1 if both points are equal.
     *
     * @param pt1 First point.
     * @param pt2 Second point.
     */
	int Compare_Points(CvPoint pt1, CvPoint pt2);





public:
    
    /**
     * Constructor that initializes the class.
     *
     * @param id Identifier (unique per each trixel).
     * @param centroid Trixel centroid.
     */
	C_Trixel(int id, CvPoint centroid);

    /**
     * Destructor, clears memory.
     */
	~C_Trixel();
    
    
    
    
    
    /**
     * Updates the trixel vertices list.
     *
     * @param points Pointer to all the vexter points.
     */
    void Update_Vertex(vector <C_Vertex *> *points);
    
    
    
    /**
     * Updates the trixel edge list.
     *
     * @param vctrs Pointer to all the vectors.
     */
    void Update_Edges(vector <C_Edge *> *vctrs);
    


    /**
     * Creates a new histogram instance.
     *
     * @param numBins Number of bins.
     * @param range_min Min range of the histogram.
     * @param range_max Max range of the histogram.
     */
    void Declare_Histogram(int numBins, int range_min, int range_max);

    
    
    
    
    /**
     * Returns the trixel id.
     *
     */
	int Get_id();
    
    
    /**
     * Returns if the trixel is valid. If it has an area returns 1, otherwise returns 0.
     *
     */
	int Is_Valid();
    
    
    /**
     * Returns the trixel centroid.
     *
     */
	CvPoint Get_Centroid();
    
    
    /**
     * Returns the trixel color.
     *
     */
	CvScalar Get_Color();
    
    
    /**
     * Returns the trixel neighbour list.
     *
     */
    vector <C_Trixel *> Get_Neighbours();
    
    /**
     * Returns the trixel edge list.
     *
     */
	vector <C_Edge *> *Get_Edge_List();
    
    /**
     * Returns the trixel vertices.
     *
     */
	vector <C_Vertex *> *Get_Vertex_Points();
    
    /**
     * Returns the trixel area = (b*h)/2.
     *
     */
	double Get_Area();
    
    /**
     * Returns the trixel perimeter.
     *
     */
	float Get_Perimeter();
    
    /**
     * Returns the max radio of a circumscribed circle inside the trixel
     *
     */
	int Get_Radius();



    
    /**
     * Returns the number of pixels within the trixel.
     *
     */
	double Get_Num_Pixels();
    
 
    
    /**
     * Returns the trixel pixels location.
     *
     */
	vector <CvPoint> *Get_Pixels();
    
    /**
     * Updates the number of pixels.
     *
     * @param pixels Number of pixels.
     */
	void Set_Num_Pixels(double pixels);

    
    /**
     * Returns the trixel histogram.
     *
     */
	C_Histogram* Get_Histogram();

    /**
     * Updates the trixel histogram.
     *
     * @param element New value.
     * @param id_hist the histogram id.
     * @param hist_name Histogram name.
     */
	void Update_Histogram(int element, int id_hist=0, char *hist_name=NULL);






    
    /**
     * Updates the trixel id.
     *
     * @param id new identifier.
     */
	void Update_id(int id);
    
    

    
    
    /**
     * Updates the valid bit.
     *
     * @param val The valid bit.
     */
	void Update_Valid_Bit (int val);

    
    /**
     * Updates the edge list.
     *
     * @param val The valid bit.
     */
	void Update_Edges(C_Edge *new_edge);
    

    
    /**
     * Add a pixel to the pixel vector of the trixel.
     *
     * @param pixel The new pixel.
     */
	void Update_Pixels(CvPoint pixel);

    

    
    /**
     * Computes the Euclidean distance between any point and the trixel centroid.
     *
     * @param point Input point.
     */
	double Calculate_Distance(CvPoint point);
    
	
    /**
     * Computes the Euclidean distance between the X component of a point and the trixel X component of the centroid.
     *
     * @param point Input point.
     */
	double Calculate_Distance_Xaxis(CvPoint point);
    
	 /**
     * Computes the Euclidean distance between the Y component of a point and the trixel Y component of the centroid.
     *
     * @param point Input point.
     */
	double Calculate_Distance_Yaxis(CvPoint point);
    
    
	/**
     * Computes angle between the trixel centroid and another point.
     *
     * @param height Image height. The Y-axis increases as the Y goes down in the image, the height solves this issue.
     * @param point Input point.
     * @param bins_x_quadrant Number of bins per quadrant. The angle can be divided into bins (p.e. if bins_X_quadrant is 2, then 8 bins are considered (0-45 and 46-90 for the first quadrant,..).
     */
	float Calculate_Angle (int height, CvPoint point, int bins_x_quadrant=0);
    
    
    
    /**
     * Updates the trixel color.
     *
     * @param color New color.
     */
	void Set_Color(CvScalar color);



    
    /**
     * Checks if two trixels are equal by looking their vertices.
     *
     * @param vert1 First vertex.
     * @param vert2 Second vertex.
     * @param vert3 Third vertex.
     * @see init_Trixels()
     */
	int Check_Vertex(CvPoint vert1, CvPoint vert2, CvPoint vert3);


};

#endif
