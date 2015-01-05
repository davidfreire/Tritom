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
 * C_Vertex manages the vertices of the trixels. This class is useful to handle the data between trixel edges and their neighbours (p.e. in geometric segmentation). Depends on C_Edge and it is used in "C_Edge.h".
 */

#if ! defined ( C_VERTEX_H )
#define C_VERTEX_H

#if ! defined ( C_TRITOM_H )
    #include <opencv2/opencv.hpp>

	//#include <time.h>
	#include <stdio.h>
	#include <string.h>
	#include <vector>
	using namespace::std;
#endif




class C_Edge;



class C_Vertex
{
    
	CvPoint src; /**< Vertex location. */
    
	vector <C_Edge *> *dst_list; /**< list of target vertices. Each list entry is a edge destination from the current vertex. This entry can not be a vertex due to useful information between the source vertex and the destination vertex such as angle*/



    //ordena por ángulo los vectores que salen del vértice
    /**
     * Sorts the list of destination vertices depending on their angle.
     *
     */
	void Sort_Dst_List();



public:
    /**
     * Constructor that initializes the class.
     *
     * @param pt The vertex location.
     */
	C_Vertex(CvPoint pt);
    
    /**
     * Destructor, clears memory.
     */
	~C_Vertex();

    /**
     * Returns the vertex 2D point.
     *
     */
	CvPoint* Get_Vertex_pt();


    /**
     * Add a new vector to the vector list.
     *
     * @param obj New vector.
     */
	void Add_Vector(C_Edge *obj);
    
    
	//Dado un vértice devuelve el vector que existe entre ese vértice y el objeto actual, en otro caso devuelve NULL.
    /**
     * Given a vertex, it returns the vector between this vertex and the class vertex. Otherwise, returns NULL.
     *
     * @param vertex2 Given vertex.
     */
	C_Edge *Search_Vector(C_Vertex *vertex2);

    
    /**
     * Draws all the vector related to the vertex.
     *
     * @param color vector color.
     * @param img Image.
     */
	void Draw_Vector_List(CvScalar color, IplImage *img);






};
#endif
