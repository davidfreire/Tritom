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
 * C_Edge manages the edges that divides trixels. It can be used for a geometric segmentation. This class depends on C_Vertex and C_Trixel, and it is used in C_Vertex.
 */



#if ! defined ( C_VECTOR_H )
#define C_VECTOR_H

#if ! defined ( C_TRITOM_H )
    #include <opencv2/opencv.hpp>

	//#include <time.h>
	#include <stdio.h>
	#include <string.h>
	#include <math.h>
	#include <vector>
	using namespace::std;
#endif




#include "C_Vertex.h"

#include "C_Trixel.h"
	using namespace::std;


class C_Trixel;




class C_Edge
{
    
	vector <C_Trixel *> *trixels; /**< Trixels divided by the edge. */

    
	C_Vertex *vertex1; /**< First vertex. */
	C_Vertex *vertex2; /**< Second vertex. */

    
	CvQuadEdge2D *tmp_edge; /**< Temp variable used during the trixel contruction. */




    /**
     * Provides the angle between two points.
     *
     * @param src_pt Source point.
     * @param dst_pt Destination point.
     * @param bins_x_quadrant Considered bins per quadrant.
     */
	double Calculate_Angle (CvPoint *src_pt, CvPoint *dst_pt, int bins_x_quadrant);
    
    /**
     * Provides the magnitude between two points.
     *
     * @param src_pt Source point.
     * @param dst_pt Destination point.
     */
	float Calculate_Magnitude (CvPoint *src_pt, CvPoint *dst_pt);


public:
    
    /**
     * Constructor that initializes the class.
     *
     * @param vertex_1 First vertex.
     * @param vertex_2 Second vertex.
     * @param tmp_edge Temporal variable.
     */
	C_Edge(C_Vertex *vertex_1, C_Vertex *vertex_2, CvQuadEdge2D *tmp_edge=NULL);
    
    /**
     * Destructor, clears memory.
     */
	~C_Edge();

    
    /**
     * Computes the angle between two points considering src_vertex as source.
     *
     * @param src_vertex Source vertex.
     * @param bins Bins Considered bins per quadrant.
     */
	float Get_Angle(C_Vertex *src_vertex, int bins=0);

    
    /**
     * Provides the magnitude between two points considering src_vertex as source.
     *
     * @param src_vertex Source vertex.
     * @param bins Bins Considered bins per quadrant.
     */
	float Get_Magnitude(C_Vertex *src_vertex, int bins=0);

    
    /**
     * Updates the list of trixels that are separated by the edge.
     *
     * @param obj New trixel to add.
     */
	void Update_Trixel(C_Trixel *obj);

    
    /**
     * Checks if an edge links two vertices (output is 1 if links, or 0 if not).
     *
     * @param vrtx1 First vertex.
     * @param vrtx2 Second vertex.
     */
	int Check_Vertex(C_Vertex *vrtx1, C_Vertex *vrtx2);

    
    /**
     * Draws the edge considering src_vertex as the source point.
     *
     * @param src_vertex Source point.
     * @param color Color.
     * @param img Image.
     */
	void Draw_Edge(C_Vertex *src_vertex, CvScalar color, IplImage *img );
    
    /**
     * Draws the edge on the image.
     *
     * @param color Color.
     * @param img Image.
     */
	void Draw_Edge(CvScalar color, IplImage *img );

    
    /**
     * Returns the trixel separated by the edge.
     *
     */
	vector<C_Trixel *> *Get_Trixels();

    
    /**
     * Returns the edge during the trixel mesh computation.
     *
     */
	CvQuadEdge2D *Get_tmpEdge();




};
#endif
