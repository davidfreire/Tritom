#include "C_Vertex.h"
#include "C_Trixel.h"

C_Vertex::C_Vertex(CvPoint pt){
	this->src.x = pt.x;
	this->src.y = pt.y;
	this->dst_list = new vector <C_Edge *>;
};



C_Vertex::~C_Vertex(){
	(*this->dst_list).clear();
	delete this->dst_list;
};


void C_Vertex::Sort_Dst_List()
{
	vector<C_Edge *>::iterator Iter, Iter2;
	C_Edge * ptr;

	for (Iter = this->dst_list->begin(); Iter != this->dst_list->end()-1 ; Iter++ )
	{
		for (Iter2 = Iter+1; Iter2 != this->dst_list->end() ; Iter2++ )
		{
			if((*Iter)->Get_Angle(this)>(*Iter2)->Get_Angle(this))
			{
				ptr = (*Iter);
				(*Iter)=(*Iter2);
				(*Iter2)=ptr;
			}
		}
	}
}

CvPoint* C_Vertex::Get_Vertex_pt()
{
	return (&this->src);
}




void C_Vertex::Add_Vector(C_Edge *obj)
{
	this->dst_list->push_back(obj);
}



C_Edge* C_Vertex::Search_Vector(C_Vertex *vertex2)
{
	vector <C_Edge *>::iterator Iter;

	for(Iter = this->dst_list->begin();Iter != this->dst_list->end();Iter++)
	{
		if((*Iter)->Check_Vertex(this,vertex2))
			return(*Iter);


	}
	return NULL;
}




void C_Vertex::Draw_Vector_List(CvScalar color, IplImage *img)
{
	vector<C_Edge *>::iterator Iter,Iter2;

	for (Iter = this->dst_list->begin(); Iter != this->dst_list->end() ; Iter++ )
	{
		(*Iter)->Draw_Edge(this, color, img);
	}
}

