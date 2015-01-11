/**
 * @file
 * @author  Luis Anton-Canalis <lanton@siani.es>
 * @version 1.1
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
 *    Luis Anton-Canalis, Mario Hernandez-Tejera, and Elena Sanchez-Nielsen, “Distance maps from unthresholded magnitudes,” Pattern Recognition, vol. 45, no. 9, pp. 3125 – 3130, 2012.
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
 * C_DMUM provides the list of local minima and makes the call to create the mesh.
 * @see init_Triangles()
 */

#ifndef C_DMUM_H
#define C_DMUM_H

//#include <algorithm>
//#include <vector>
//#include <list>

#include "C_Tritom.h"


#define N_SCALES 1

struct C_DMUM_Result
{
public:
	CvPoint p;
	int scale;
	double value;
	float index;
};


class C_DMUM
{
    public:

	IplImage *_scales[N_SCALES];
	std::vector<C_DMUM_Result> final_extrema;
	IplImage *collage;


    /**
     * Constructor that initializes the class.
     *
     * @param img Input image.
     */
	C_DMUM(IplImage *img);
    
    /**
     * Destructor, clears memory.
     */
	~C_DMUM();

	void edges(IplImage *img, int scale);
	void DMUM(IplImage *img, float gamma);
	void compute_minima();

	int Generate_tritom_from_DMUM(C_Tritom *tritom);

    /**
     * Computes the Delaunay triangularization from DTUM minima and creates the Tritom structure.
     *
     * @param img Input image.
     * @param gamma DMUM parameter.
     * @param tritom Tritom structure.
     */
	int Compute_Trixels(IplImage *img, float gamma, C_Tritom *tritom);


    private:
    
    static CvSubdiv2D* init_delaunay( CvMemStorage* storage, CvRect rect );

	IplImage *gray[N_SCALES], *dx[N_SCALES], *dy[N_SCALES], *mag[N_SCALES], *background[N_SCALES];
	IplImage *integral[N_SCALES], *dist[N_SCALES], *integralc[N_SCALES];
	IplImage *patchdxA, *patchdyA, *patchdxB, *patchdyB;

	double _sumaRectangulo(IplImage *integral, int j, int i, int q);

	static bool compare (C_DMUM_Result r1, C_DMUM_Result r2) {return r1.value > r2.value;};
	static bool shuffle (C_DMUM_Result r1, C_DMUM_Result r2) {return r1.index > r2.index;};
};

#endif // C_DMUM_H






