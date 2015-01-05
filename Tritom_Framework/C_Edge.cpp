#include "C_Edge.h"



C_Edge::C_Edge(C_Vertex *vertex_1, C_Vertex *vertex_2, CvQuadEdge2D *tmp_edge){
	this->vertex1=vertex_1;
	this->vertex2=vertex_2;
	this->tmp_edge=tmp_edge;
	trixels = new vector <C_Trixel *>;
};
C_Edge::~C_Edge(){
	this->vertex1=NULL;
	this->vertex2=NULL;
	this->trixels->clear();
	delete this->trixels;
};


float C_Edge::Get_Angle(C_Vertex *src_vertex, int bins){
	CvPoint *src_pt, *dst_pt;
	if(src_vertex == this->vertex1){
		src_pt = this->vertex1->Get_Vertex_pt();
		dst_pt = this->vertex2->Get_Vertex_pt();
	}else{
		src_pt = this->vertex2->Get_Vertex_pt();
		dst_pt = this->vertex1->Get_Vertex_pt();
	}
	return Calculate_Angle (src_pt, dst_pt, bins);
}

float C_Edge::Get_Magnitude(C_Vertex *src_vertex, int bins){
	CvPoint *src_pt, *dst_pt;
	if(src_vertex == this->vertex1){
		src_pt = this->vertex1->Get_Vertex_pt();
		dst_pt = this->vertex2->Get_Vertex_pt();
	}else{
		src_pt = this->vertex2->Get_Vertex_pt();
		dst_pt = this->vertex1->Get_Vertex_pt();
	}
	return Calculate_Magnitude (src_pt, dst_pt);
}

void C_Edge::Update_Trixel(C_Trixel *obj)
{
	this->trixels->push_back(obj);
}


int C_Edge::Check_Vertex(C_Vertex *vrtx1, C_Vertex *vrtx2)
{
	if((this->vertex1 == vrtx1) || (this->vertex1 == vrtx2))
		if((this->vertex2 == vrtx1) || (this->vertex2 == vrtx2))
			return 1;
	return 0;

}



double C_Edge::Calculate_Angle (CvPoint *src_pt, CvPoint *dst_pt, int bins_x_quadrant){
	float output;
	float angle;

	if( dst_pt->y < 0)
  	    angle = atan2(float( dst_pt->y - src_pt->y), float(((-1)*dst_pt->x) - src_pt->x)) * 180 / 3.14159265358979323846;
	else
		angle = atan2(float(dst_pt->y - src_pt->y), float(dst_pt->x - src_pt->x)) * 180 / 3.14159265358979323846;


	if(angle < 0){
		angle = atan2(float(dst_pt->y - src_pt->y), (-1)*float(dst_pt->x - src_pt->x)) * 180 / 3.14159265358979323846;
		output = (180. - (angle));
	}else
		output = angle;

	//printf("\nThe angle from (%d,%d) to (%d,%d) is %.2f", src_pt->x, src_pt->y, dst_pt->x, dst_pt->y, output);

	if(bins_x_quadrant == 0)
		return output;
	else
		return int(output/(90/bins_x_quadrant));
}

float C_Edge::Calculate_Magnitude (CvPoint *src_pt, CvPoint *dst_pt){
	return sqrt(((float(dst_pt->y - src_pt->y)*float(dst_pt->y - src_pt->y))) + (float(dst_pt->x - src_pt->x) * float(dst_pt->x - src_pt->x)));
}


void C_Edge::Draw_Edge(C_Vertex *src_vertex, CvScalar color, IplImage *img )
{
	CvPoint *src_pt, *dst_pt;
	if(src_vertex == this->vertex1){
		src_pt = this->vertex1->Get_Vertex_pt();
		dst_pt = this->vertex2->Get_Vertex_pt();
	}else{
		src_pt = this->vertex2->Get_Vertex_pt();
		dst_pt = this->vertex1->Get_Vertex_pt();
	}

	cvLine( img, *src_pt, *dst_pt, color, 1, 8, 0 );
}


void C_Edge::Draw_Edge(CvScalar color, IplImage *img )
{
     cvLine( img, (*this->vertex1->Get_Vertex_pt()), (*this->vertex2->Get_Vertex_pt()), color, 1, 8, 0 );
}

vector<C_Trixel *> * C_Edge::Get_Trixels()
{
	return this->trixels;

}


CvQuadEdge2D *C_Edge::Get_tmpEdge(){
	return this->tmp_edge;
}

