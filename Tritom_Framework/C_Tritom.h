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
 * C_TriToM generates and provides the trixel mesh.
 */

#ifndef C_TRITOM_H
#define C_TRITOM_H

#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <vector>
using namespace::std;
#include <list>

#include "C_Centroid.h"

#include "C_Trixel.h"
//#include "C_Vertex.h"
//#include "C_Edge.h"



//#include "C_Histogram.h"
//extern class C_Compare_Histogram;



class C_Tritom
{

    private:
    
	IplImage *image; /**< Source image. */

	vector <C_Trixel *> *trixel_list;     /**< Trixel list. */
	vector <C_Vertex *> *vertex_list;   /**< Vertex list. */
	vector <C_Edge *> *edge_list;   /**< Edge list. */

    
	//Data used during the trixel mesh computation
	CvMemStorage* storage;  /**< Memory storage for the Delaunay triangulation. */
    CvSubdiv2D* subdiv;     /**< Aux variable for the Delaunay triangulation. */
	vector <C_Centroid_List *> *centroid_vect; /**< Centroid vector which contains the centroid of every trixel of the mesh. */


	IplImage *trixels_map;    /**< Image made by pointers. Each pixel points to the trixel it belongs. */
	IplImage *vertex_map;       /**< Image made by pointers. Each vertex pixel points to the trixel it belongs. */


    int histogram; /**< Histogram Flag. Set to 1 if histograms are considered. */
	C_Histogram_Comparison *comparison_obj;    /**< Object to compare two histograms. */
    


    /**
     * Initializes Delaunay memory storage.
     *
     * @param storage Memory storage.
     */
	CvSubdiv2D* init_delaunay( CvMemStorage* storage);

    /**
     * Auxiliar function to determine if a pixel belongs to a trixel.
     */
	float Sign(CvPoint p1, CvPoint p2, CvPoint p3);
    
    /**
     * Determines if a pixel belongs to a trixel given the three vertices of it.
     *
     * @param pt Pixel point.
     * @param buf Buffer that contains the three trixel vertices.
     */
	bool Is_Trixel_Point(CvPoint pt, CvPoint* buf);
    
    /**
     *  Add a trixel the map of trixels.
     *
     * @param trixel Input trixel.
     */
	void Generate_Trixel_Map(C_Trixel *trixel);

	
    /**
     *  Checks the image boundaries during the mesh creation. Checks if an edge is inside the image or not.
     */
	int Check_vect_size(CvQuadEdge2D* candidate_vct);

    /**
     * Generates a first trixel.
     *
     * @param Vector candidate that can help to build a new trixel.
     * @param trixel_queue Trixels in the waiting list for neighbours computation.
     * @param pnt_map_image Map that checks no trixel is computed twice.
     * @param data Pointer to the centroid of the new trixel. If there is no new trixel, this pointer is NULL.
     */
	void Generate_First_Trixel(CvQuadEdge2D* candidate_vct, list <C_Trixel *> &trixel_queue,  IplImage *pnt_map_image, C_Centroid_List **data);

    /**
     * Generates the trixel mesh.
     *
     * @param new_edge Vector candidate that can help to build a new neighbor trixel.
     * @param trixel_queue Trixels in the waiting list for neighbours computation.
     * @param pnt_map_image Map that checks no trixel is computed twice.
     * @param data Pointer to the centroid of the new trixel. If there is no new trixel, this pointer is NULL.
     */
	int Generate_New_Trixel(C_Edge *new_edge,list <C_Trixel *> &trixel_queue, IplImage *pnt_map_image, C_Trixel *trixel, C_Centroid_List **data);

    /**
     *  Checks the image boundaries during the mesh creation.
     */
	int Check_edge_size( CvQuadEdge2D* edge );


public:
    
    
    /**
     * Constructor that initializes the class but it does not create the trixel structure yet.
     *
     * @param src_image Input image.
     * @param args Argument-> Histograms are computed ([0] to 1). if NULL, all flags are 0.
     */
	C_Tritom(IplImage *src_image, int *args = NULL);
    
    
    /**
     * Destructor, clears memory.
     */
	~C_Tritom();
    
   
    /**
     * Inserts a point into the structure before the trinagulation process.
     *
     * @param new_point Input point.
     */
    void Insert_Delaunay_Point(CvPoint new_point);
    
    

    /**
     * Creates the trixel mesh after the points have been added.
     *
     * @see Compute_Trixels()
     */
	void init_Trixels();


	

    /**
     * Returns the source image.
     */
	IplImage * Get_Image();
    /**
     * Returns the trixel list.
     */
	vector <C_Trixel *> Get_Trixel_List();
    /**
     * Returns the edge list (each trixel is made by 3 edges).
     */
	vector <C_Edge *> *Get_Vector_List();






    //Trixel visualization functions

    /**
     * Draws all the trixels on the image. It actually prints all the edges.
     *
     * @param img Image.
     * @param color Trixel color.
     */
	void Print_All_Trixels(IplImage *img, CvScalar color);
    
    
    
    /**
     * Only draws on the image the trixel that belongs to the input point.
     *
     * @param pt Input point.
     * @param img Image.
     * @param color Trixel color.
     */
	void Highlight_Trixel(CvPoint pt, IplImage *img, CvScalar color);
    
    
    //Dibuja un triangulo
    //Imprime todos los triangulos en un color -> en realidad se imprimen todas las aristas
    /**
     * Only draws on the image the specified trixel that belongs to the input point.
     *
     * @param trixel Trixel.
     * @param img Image.
     * @param color Trixel color.
     */
	void Draw_trixel( C_Trixel *trixel, CvScalar color, IplImage *img );


    
    
    
    

    //Edge visualization
    /**
     * Draws a trixel vertex and all the edges that comes into and out from this vertex.
     *
     * @param pt vertex point.
     * @param img Image.
     * @param color edges color.
     */
	void Draw_VrtxandVctr(CvPoint pt, IplImage *img, CvScalar color);

    
    
    
    
    
    
    
    //Dibuja la imagen pero el contenido de cada triangulo es la media de todos los pixels que forman el triángulo
    /**
     * Draws the source image based on the average value of all pixels inside each trixel.
     *
     * @param img Image.
     */
    void Draw_Mean_Image (IplImage *img);

    
    
    /**
     * Given an input point, returns the trixel to which it belongs.
     *
     * @param pt Input point.
     */
    C_Trixel *Get_Trixel(CvPoint pt);
    
    
    
    /**
     * Given an input point, returns the color of the trixel to which it belongs.
     *
     * @param pt Input point.
     */
    CvScalar Get_Trixel_Color(CvPoint pt);
    
    
    
    /**
     * Given an input point, returns the centroid of the trixel to which it belongs.
     *
     * @param pt Input point.
     */
    int Get_Trixel_Centroid(CvPoint pt, CvPoint *centroid);
    
    
    /**
     * Returns the total number of trixels.
     */
    int GetNumTrixels();
    
    
    


    //Histogramas
    //Dado dos puntos, obtiene el triángulo de cada punto y devuelve una gráfica con los histogramas de ambos triángulos.
    void DrawHist_Comp(CvPoint pt1, CvPoint pt2, IplImage *img, CvScalar color, IplImage **signal);

};


#endif // C_TRITOM_H
