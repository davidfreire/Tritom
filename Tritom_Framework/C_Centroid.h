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
 * C_Centroid save list of pointer to trixels. This list is saved into a map, being the coordinates the centroid and the content of these coordinates a list of trixels.
 * @see Generate_New_Triangle()
 * @see Generate_First_Triangle()
 */

#if ! defined ( C_CENTROID_H )
#define C_CENTROID_H


class C_Centroid_List
{
private: 
	public:
	vector <long> *tlist;

    /**
     * Constructor that initializes the list.
     *
     */
	C_Centroid_List(){
		this->tlist= new vector <long>;
	}

    /**
     * Provides the size of the list.
     *
     */
	int Get_Size(){
		return int((*(this->tlist)).size());
	}
    
    /**
     * A new element (trixel address) is added to the list.
     *
     */
	void Add_Element(long new_add){
		(*(this->tlist)).push_back(new_add);
	}

    /**
     * Retrieves an address from the list.
     *
     *@param i the element position.
     */
	long Get_Element(int i){
		vector<long>::iterator iter;
		iter = ((*this->tlist).begin())+i;
		return (*iter);
	}

    /**
     * Returns the list.
     *
     */
	vector<long> * GetList(){
		return this->tlist;
	}

	/**
     * Destructor that delete all the pointers to the trixels (not the trixels).
     *
     */
	~C_Centroid_List(){
		(*this->tlist).clear();
		delete this->tlist;
	};


};

#endif



