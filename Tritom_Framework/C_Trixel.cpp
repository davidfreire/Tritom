#include "C_Trixel.h"

int C_Trixel::Get_id(){
	return this->id_trixel;
}

int C_Trixel::Is_Valid(){
	return this->valid_bit;
}

vector <C_Edge *> *C_Trixel::Get_Edge_List(){
	return this->edge_list;
}

CvPoint C_Trixel::Get_Centroid(){
	return this->centroid;
}

vector <C_Vertex *>  *C_Trixel::Get_Vertex_Points(){
	return this->vertex_points;
}

CvScalar C_Trixel::Get_Color(){
	return this->color;
}



vector <C_Trixel *> C_Trixel::Get_Neighbours(){
    vector <C_Trixel *> vecinos;
    //All the trixel edges are computed in order to check the neighbours.
	vector <C_Edge *> *aristas =  this->Get_Edge_List();
    int tam=int(aristas->size());

    for(int i=0; i<tam;i++){
        vector<C_Trixel *> *list = (*(aristas->begin()+i))->Get_Trixels();
        if(int(list->size())>1){
            if(this == (*list->begin())){ //If the present trixel is the same to the first trixel in the trixel list of the edge.
                if ((*(list->begin()+1))->Is_Valid()) //Then the next trixel (begin+1) is saved.
                    vecinos.push_back(*(list->begin()+1));
            }else{ //Otherwise
                if ((*list->begin())->Is_Valid()) //The (begin) trixel is the saved.
                    vecinos.push_back(*list->begin());
            }
        }
    }
    return vecinos;
}

int C_Trixel::Get_Radius(){
	return this->radius;
}



double C_Trixel::Calculate_Distance(CvPoint point){
	return (double)sqrt((double)((this->centroid.x-point.x)*(this->centroid.x-point.x)+(this->centroid.y-point.y)*(this->centroid.y-point.y)));
}

double C_Trixel::Calculate_Distance_Xaxis(CvPoint point){
       double result = (double)(this->centroid.x-point.x);
       if(result<0)
               result = (-1.0) * result;
       return result;
}

double C_Trixel::Calculate_Distance_Yaxis(CvPoint point){
       double result = (double)(this->centroid.y-point.y);
       if(result<0)
               result = (-1.0) * result;
       return result;
}




double C_Trixel::Get_Num_Pixels(){
	return this->num_pixels;

}

void C_Trixel::Set_Num_Pixels(double pixels){
	this->num_pixels = pixels;
}

void C_Trixel::Set_Color(CvScalar color){
	this->color = color;
}




vector <CvPoint>  *C_Trixel::Get_Pixels(){
	return this->pixels;
}






void C_Trixel::Update_Edges(C_Edge *new_edge){
	this->edge_list->push_back (new_edge);
}



void C_Trixel::Update_Valid_Bit(int val){
	this->valid_bit=val;
}

C_Trixel::C_Trixel(int id, CvPoint centroid){
	this->id_trixel=id;
    this->pixels = new vector <CvPoint>;
	this->centroid.x = centroid.x;
	this->centroid.y = centroid.y;
	this->valid_bit=1;


	this->feat_Hist = NULL;

}


void C_Trixel::Update_Vertex(vector <C_Vertex *> *points){
	this->vertex_points = points;
}




void C_Trixel::Update_Pixels(CvPoint pixel){
    this->pixels->push_back(pixel);
}


void C_Trixel::Update_Edges(vector <C_Edge *> *vctrs){
	this->edge_list = vctrs;
}

void C_Trixel::Update_id(int id){
	this->id_trixel=id;
}



void C_Trixel::Declare_Histogram(int numBins, int range_min, int range_max){
	this->feat_Hist= new C_Histogram(numBins, range_min, range_max);
}

void C_Trixel::Update_Histogram(int element, int id_hist, char *hist_name){
	if(!this->feat_Hist->Add_Element(element, id_hist, hist_name))
		printf("\n Bad use of Add_Element in Update_Histogram->C_Trixel.cpp");
}

C_Histogram* C_Trixel::Get_Histogram(){
	return(this->feat_Hist);
}


C_Trixel::~C_Trixel(){
	this->edge_list->clear();
	delete(this->edge_list);
	this->vertex_points->clear();
	delete(this->vertex_points);
	  this->pixels->clear();
	delete (this->pixels);


	if(this->feat_Hist != NULL)
		delete(this->feat_Hist);

    
}




//Area based on Heron equation
double C_Trixel::Get_Area(){
	vector<C_Vertex *>::iterator Iter;
	CvPoint *point_org;
	CvPoint *point_dst;
	float lados[3];

	Iter = this->vertex_points->begin();

	for(int i=0;i<3;i++){
		point_org = (*(Iter+i))->Get_Vertex_pt();
		point_dst = (*(Iter+((i+1)%3)))->Get_Vertex_pt();
		lados[i]=sqrt(pow(float((*point_org).x-(*point_dst).x),2) + pow(float((*point_org).y-(*point_dst).y),2));
	}

	float sf = (lados[0] + lados[1] + lados[2])/2.0;
    double areaf = (double)sqrt(sf * (sf - lados[0]) * (sf - lados[1]) * (sf - lados[2]));

	return areaf;
}


float C_Trixel::Get_Perimeter(){
	vector<C_Vertex *>::iterator Iter;
	CvPoint *point_org;
	CvPoint *point_dst;
	float perim=0;

	Iter = this->vertex_points->begin();

	for(int i=0;i<3;i++){
		point_org = (*(Iter+i))->Get_Vertex_pt();
		point_dst = (*(Iter+((i+1)%3)))->Get_Vertex_pt();
		perim+=sqrt(pow(float((*point_org).x-(*point_dst).x),2) + pow(float((*point_org).y-(*point_dst).y),2));
		i++;
	}

	return perim;

}





int C_Trixel::Compare_Points(CvPoint pt1, CvPoint pt2){
	if((pt1.x == pt2.x)&&(pt1.y == pt2.y))
		return 1;
	else
		return 0;
}








float C_Trixel::Calculate_Angle (int height, CvPoint point, int bins_x_quadrant){
	float v_x = point.x;
	float v_y = point.y;
	float output;

	if(v_y < 0)
			v_x *= (-1);

	double angle = atan2(float(height-v_y - this->centroid.y), float(v_x - this->centroid.x)) * 180 / 3.14159265358979323846;
	if(angle < 0){
		angle = atan2(float(height-v_y - this->centroid.y), -1*float(v_x - this->centroid.x)) * 180 / 3.14159265358979323846;
		output = (180. - (angle));
	}else
		output = angle;

	if(bins_x_quadrant == 0)
		return output;
	else
		return int(output/(90/bins_x_quadrant));
}


int C_Trixel::Check_Vertex(CvPoint vert1, CvPoint vert2, CvPoint vert3){

	vector<C_Vertex *>::iterator iter;
	iter = this->vertex_points->begin();

	if((Compare_Points(*((*iter)->Get_Vertex_pt()),vert1))||(Compare_Points(*((*iter)->Get_Vertex_pt()),vert2))||(Compare_Points(*((*iter)->Get_Vertex_pt()),vert3))){
		if((Compare_Points(*((*(iter+1))->Get_Vertex_pt()),vert1))||(Compare_Points(*((*(iter+1))->Get_Vertex_pt()),vert2))||(Compare_Points(*((*(iter+1))->Get_Vertex_pt()),vert3))){
			if((Compare_Points(*((*(iter+2))->Get_Vertex_pt()),vert1))||(Compare_Points(*((*(iter+2))->Get_Vertex_pt()),vert2))||(Compare_Points(*((*(iter+2))->Get_Vertex_pt()),vert3))){
				return 1;
			}
		}
	}
	return 0;
}

