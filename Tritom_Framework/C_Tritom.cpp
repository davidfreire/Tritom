#include "C_Tritom.h"
//#include "C_Histogram.h"
#include <opencv2/legacy/legacy.hpp>


C_Tritom::C_Tritom (IplImage *src_image, int *args){

	if(src_image == NULL){
		printf("Error at C_Tritom: No Image");
		exit(1);
	}
	this->image = cvCloneImage(src_image);


    this->storage = cvCreateMemStorage(0);
	this->subdiv = init_delaunay(this->storage);

	this->trixel_list= new vector <C_Trixel *>;
	this->vertex_list = new vector <C_Vertex *>;
	this->edge_list = new vector <C_Edge *>;
	this->trixels_map = cvCreateImage (cvGetSize(this->image), IPL_DEPTH_64F, 1);
	this->vertex_map = cvCreateImage (cvGetSize(this->image), IPL_DEPTH_64F, 1);

	cvZero(this->trixels_map);
	cvZero(this->vertex_map);

	this->centroid_vect = new vector <C_Centroid_List *>;

	this->comparison_obj = new C_Histogram_Comparison();
    
    if(args == NULL)
        this->histogram = 0;
    else
        this->histogram = args[0];


}



C_Tritom::~C_Tritom (){

	delete(this->comparison_obj);


	if(this->vertex_map != NULL)
		cvReleaseImage(&this->vertex_map);

	vector<C_Vertex *>::iterator Iter;

	if(this->vertex_list!=NULL)
	{
		for(Iter = (*this->vertex_list).begin();Iter != (*this->vertex_list).end();Iter++)
		{
			delete(*Iter);
		}
		this->vertex_list->clear();
		delete(this->vertex_list);
	}


	vector<C_Edge *>::iterator Iter2;
	if(this->edge_list!=NULL)
	{
		for(Iter2 = (*this->edge_list).begin();Iter2 != (*this->edge_list).end();Iter2++)
		{
			delete(*Iter2);
		}
		this->edge_list->clear();
		delete(this->edge_list);
	}

	vector<C_Trixel *>::iterator Iter3;
	for(Iter3 = (*this->trixel_list).begin();Iter3 != (*this->trixel_list).end();Iter3++)
		delete (*Iter3);

	vector<C_Centroid_List *>::iterator Iter4;
	for(Iter4 = (*this->centroid_vect).begin();Iter4 != (*this->centroid_vect).end();Iter4++)
		delete (*Iter4);

	(*this->centroid_vect).clear();
	(*this->trixel_list).clear();
    cvReleaseImage(&this->image);
	cvReleaseImage(&this->trixels_map);
	cvReleaseMemStorage( &this->storage );
	delete this->trixel_list;
	delete this->centroid_vect;
}


IplImage * C_Tritom ::Get_Image(){
	return this->image;
}


vector <C_Trixel *> C_Tritom ::Get_Trixel_List(){
	return (*this->trixel_list);
}

vector <C_Edge *> *C_Tritom ::Get_Vector_List(){
	return (this->edge_list);
}


void C_Tritom::Insert_Delaunay_Point(CvPoint new_point)
{
	cvSubdivDelaunay2DInsert( this->subdiv, cvPoint2D32f(new_point.x, new_point.y) );
}


CvSubdiv2D* C_Tritom::init_delaunay( CvMemStorage* storage)
{
    CvSubdiv2D* subdiv;
    CvRect rect = { 0, 0, this->image->width, this->image->height
	};

    subdiv = cvCreateSubdiv2D( CV_SEQ_KIND_SUBDIV2D, sizeof(*subdiv),
                               sizeof(CvSubdiv2DPoint),
                               sizeof(CvQuadEdge2D),
                               storage );
    cvInitSubdivDelaunay2D( subdiv, rect );

    return subdiv;
}

int C_Tritom::Check_vect_size(CvQuadEdge2D* candidate_vct){
	CvSubdiv2DPoint* org_pt;
	CvSubdiv2DPoint* dst_pt;
	org_pt = cvSubdiv2DEdgeOrg((CvSubdiv2DEdge)candidate_vct);
	dst_pt = cvSubdiv2DEdgeDst((CvSubdiv2DEdge)candidate_vct);

	if((cvRound(int(org_pt->pt.x))>0)&&(cvRound(int(org_pt->pt.y))>0)&&(cvRound(int(org_pt->pt.x))<this->image->width)&&(cvRound(int(org_pt->pt.y))<this->image->height)){
		if((cvRound(int(dst_pt->pt.x))>0)&&(cvRound(int(dst_pt->pt.y))>0)&&(cvRound(int(dst_pt->pt.x))<this->image->width)&&(cvRound(int(dst_pt->pt.y))<this->image->height)){
			return 1;
		}
	}

	return 0;


}

int C_Tritom::Check_edge_size( CvQuadEdge2D* edge )
{
	CvSubdiv2DEdge t = (CvSubdiv2DEdge)edge;
	CvSubdiv2DPoint* newedge_pt_orig = cvSubdiv2DEdgeOrg( t );
	CvSubdiv2DPoint* newedge_pt_dest = cvSubdiv2DEdgeDst( t );

	if((cvRound(newedge_pt_orig->pt.x)>0)&&(cvRound(newedge_pt_orig->pt.y)>0)&&(cvRound(newedge_pt_orig->pt.x)<this->image->width)&&(cvRound(newedge_pt_orig->pt.y)<this->image->height)){
		if((cvRound(newedge_pt_dest->pt.x)>0)&&(cvRound(newedge_pt_dest->pt.y)>0)&&(cvRound(newedge_pt_dest->pt.x)<this->image->width)&&(cvRound(newedge_pt_dest->pt.y)<this->image->height)){
			return 1;
		}
	}

	return 0;
}


void C_Tritom::init_Trixels(){



	IplImage *pnt_map_image = cvCreateImage (cvGetSize(this->image), IPL_DEPTH_64F, 1);


	//Initializes the pointer map
	cvZero(pnt_map_image);

	CvSeqReader  reader;
	int elem_size = this->subdiv->edges->elem_size;




	cvStartReadSeq( (CvSeq*)(this->subdiv->edges), &reader, 0 );
	CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);

	while( !CV_IS_SET_ELEM( edge )){
		CV_NEXT_SEQ_ELEM( elem_size, reader );
		edge = (CvQuadEdge2D*)(reader.ptr);
	}


	//Selects the first available and valid edge
	while(!(this->Check_edge_size(edge))||(!CV_IS_SET_ELEM( edge ))){
		CV_NEXT_SEQ_ELEM( elem_size, reader );
		edge = (CvQuadEdge2D*)(reader.ptr);
		if(!CV_IS_SET_ELEM( edge ))
			continue;
	}


	//trixel queue
	list <C_Trixel *> trixel_queue;
	//edge list for a trixel
	vector<C_Edge *> *edge_list;
    
	C_Centroid_List *data=NULL;

    //First trixel is computed
	this->Generate_First_Trixel(edge, trixel_queue, pnt_map_image, &data);

	(*this->centroid_vect).push_back(data);
	data=NULL;

	vector<C_Edge *>::iterator edge_iter;

	C_Trixel *trixel_from_list=NULL;


	while(!trixel_queue.empty()){
		trixel_from_list = (C_Trixel *)(*trixel_queue.begin());
		trixel_queue.pop_front();

		edge_list=trixel_from_list->Get_Edge_List();
		for (edge_iter = edge_list->begin(); edge_iter != edge_list->end() ; edge_iter++ )
		{
			if((*edge_iter)->Get_Trixels()->size()<2){//If the edge doesnt belong to the list, new neighbour trixels are computed and added to the trixel queue, otherwise nothing is done.
				if(this->Generate_New_Trixel((*edge_iter),trixel_queue,pnt_map_image,trixel_from_list,&data)){
					if(data!=NULL){
						(*this->centroid_vect).push_back(data);
						data=NULL;
					}
				}
			}

		}

		this->Generate_Trixel_Map(trixel_from_list);
        trixel_from_list->Update_id(int((*this->trixel_list).size())+1);
        (*this->trixel_list).push_back (trixel_from_list);

	}
	trixel_queue.clear();
	cvReleaseImage(&pnt_map_image);
}



float C_Tritom :: Sign(CvPoint p1, CvPoint p2, CvPoint p3)
{
  return float((p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y));
}

bool C_Tritom :: Is_Trixel_Point(CvPoint pt, CvPoint* buf)
{
  bool b1, b2, b3;

  b1 = Sign(pt, buf[0], buf[1]) <= 0.0f;
  b2 = Sign(pt, buf[1], buf[2]) <= 0.0f;
  b3 = Sign(pt, buf[2], buf[0]) <= 0.0f;

  return ((b1 == b2) && (b2 == b3));
}



void C_Tritom :: Generate_Trixel_Map(C_Trixel *trixel){
	CvPoint* buf = new CvPoint[3];
	CvPoint high;
    CvPoint low;
	CvScalar pixel;
	int counter=0;
    


	vector <C_Vertex *> *vertex_points = trixel->Get_Vertex_Points();
	vector <C_Vertex *>::iterator vertex;
	vertex = vertex_points->begin();

    buf[0] = cvPoint( (*(vertex))->Get_Vertex_pt()->x, (*(vertex))->Get_Vertex_pt()->y);
	buf[1] = cvPoint( (*(vertex+1))->Get_Vertex_pt()->x, (*(vertex+1))->Get_Vertex_pt()->y);
	buf[2] = cvPoint( (*(vertex+2))->Get_Vertex_pt()->x, (*(vertex+2))->Get_Vertex_pt()->y);

	if((buf[0].x > buf[1].x))
		if(buf[0].x > buf[2].x)
			high.x = buf[0].x;
		else
			high.x = buf[2].x;
	else
		if(buf[1].x > buf[2].x)
			high.x = buf[1].x;
		else
			high.x = buf[2].x;

	if((buf[0].y > buf[1].y))
		if(buf[0].y > buf[2].y)
			high.y = buf[0].y;
		else
			high.y = buf[2].y;
	else
		if(buf[1].y > buf[2].y)
			high.y = buf[1].y;
		else
			high.y = buf[2].y;



	if((buf[0].x < buf[1].x))
		if(buf[0].x < buf[2].x)
			low.x = buf[0].x;
		else
			low.x = buf[2].x;
	else
		if(buf[1].x < buf[2].x)
			low.x = buf[1].x;
		else
			low.x = buf[2].x;


	if((buf[0].y < buf[1].y))
		if(buf[0].y < buf[2].y)
			low.y = buf[0].y;
		else
			low.y = buf[2].y;
	else
		if(buf[1].y < buf[2].y)
			low.y = buf[1].y;
		else
			low.y = buf[2].y;

    
    //RGB color variables
	CvScalar color;
	color = CV_RGB(0,0,0);


    if (this->histogram)
        trixel->Declare_Histogram(256,0,255); //2 secs


	for (int y=low.y;y<=high.y;y++){
		for (int x=low.x;x<=high.x;x++){
			if(this->Is_Trixel_Point(cvPoint(x,y), buf)){
				cvSet2D(this->trixels_map,y,x,cvScalar((long)trixel));  //Saves the trixel pointer in the pixel.
				trixel->Update_Pixels(cvPoint(x,y));

				pixel=cvGet2D(this->image,y,x);
				color.val[0]+=pixel.val[0];
				color.val[1]+=pixel.val[1];
				color.val[2]+=pixel.val[2];

				counter++;

                if (this->histogram){
                    //BGR Histogram
                    char blue_chnnl[30]= "Blue Channel";
                    char green_chnnl[30]= "Green Channel";
                    char red_chnnl[30]= "Red Channel";
				    trixel->Update_Histogram(pixel.val[0], 1, blue_chnnl);
				    trixel->Update_Histogram(pixel.val[1], 2, green_chnnl);
				    trixel->Update_Histogram(pixel.val[2], 3, red_chnnl);

				    //CrCb Histogram
				    /*
                    pixel = cvGet2D(this->Y_img,y,x); //Cr
				    trixel->Update_Histogram(pixel.val[0], 1, "Cr Channel");
				    pixel = cvGet2D(this->X_img,y,x); //Cb
                    trixel->Update_Histogram(pixel.val[0], 2, "Cb Channel");
                    */
                }
				
			}
		}
	}

	trixel->Set_Num_Pixels(counter);





	if(counter >0){
		color.val[0]/=counter;
		color.val[1]/=counter;
		color.val[2]/=counter;
	}else{ //If the trixel has no area, it is considered as no valid.
		color=CV_RGB(0,0,0);
		trixel->Update_Valid_Bit(0);
	}

	trixel->Set_Color(color);
    delete buf;
}






void C_Tritom::Generate_First_Trixel(CvQuadEdge2D* candidate_vct, list <C_Trixel *> &trixel_queue,  IplImage *pnt_map_image, C_Centroid_List **data){

	C_Trixel *trixel;
	CvPoint centroid = cvPoint(0,0);
	double centroid_x=0;
	double centroid_y=0;
	int edge_counter=0;
	C_Edge *vctr;
	vector <CvPoint> points_x_geometry;
	vector <C_Vertex *> *vrtx_list;
	vector <C_Edge *> *vctr_list;
	int flag=0;
    CvPoint candidates[2];
	candidates[0].x=-1;
	candidates[0].y=-1;
	candidates[1].x=-1;
	candidates[1].y=-1;

	vector <CvQuadEdge2D *> trixel_edges;
	vector<CvQuadEdge2D *>::iterator Iter;
	vector<CvPoint>::iterator IterVrtx;

	for(int i=0;i<2;i++){
		CvSubdiv2DEdge t = (CvSubdiv2DEdge)candidate_vct;


		flag=0;
		centroid_x=0;
		centroid_y=0;
		points_x_geometry.clear();
		trixel_edges.clear();
		edge_counter=0;
        
		//Checks the present vertex and the next two vertices
		while(edge_counter<4)
		{
			//Saves the related vertex point
			CvSubdiv2DPoint* pt = cvSubdiv2DEdgeOrg( t );

			if((int(pt->pt.x) < this->image->width)&&(int(pt->pt.y) < this->image->height)&&(int(pt->pt.y) > 0)&&(int(pt->pt.x) > 0)){
				if(edge_counter == 0){
					candidates[0].x = int(cvSubdiv2DEdgeOrg( t )->pt.x);
					candidates[0].y = int(cvSubdiv2DEdgeOrg( t )->pt.y);
					candidates[1].x = int(cvSubdiv2DEdgeDst( t )->pt.x);
					candidates[1].y = int(cvSubdiv2DEdgeDst( t )->pt.y);

				}else{
					if((((candidates[0].x == int(cvSubdiv2DEdgeOrg( t )->pt.x))) && (candidates[0].y == int(cvSubdiv2DEdgeOrg( t )->pt.y))) || (((candidates[0].x == int(cvSubdiv2DEdgeDst( t )->pt.x))) && (candidates[0].y == int(cvSubdiv2DEdgeDst( t )->pt.y))))
					{
						centroid_x += candidates[1].x;
						centroid_y += candidates[1].y;
						points_x_geometry.push_back(candidates[1]);
						candidates[0].x = int(cvSubdiv2DEdgeOrg( t )->pt.x);
						candidates[0].y = int(cvSubdiv2DEdgeOrg( t )->pt.y);
						candidates[1].x = int(cvSubdiv2DEdgeDst( t )->pt.x);
						candidates[1].y = int(cvSubdiv2DEdgeDst( t )->pt.y);
					}else{
						if((((candidates[1].x == int(cvSubdiv2DEdgeOrg( t )->pt.x))) && (candidates[1].y == int(cvSubdiv2DEdgeOrg( t )->pt.y))) || (((candidates[1].x == int(cvSubdiv2DEdgeDst( t )->pt.x))) && (candidates[1].y == int(cvSubdiv2DEdgeDst( t )->pt.y))))
						{
							centroid_x += candidates[0].x;
							centroid_y += candidates[0].y;
							points_x_geometry.push_back(candidates[0]);
							candidates[0].x = int(cvSubdiv2DEdgeOrg( t )->pt.x);
							candidates[0].y = int(cvSubdiv2DEdgeOrg( t )->pt.y);
							candidates[1].x = int(cvSubdiv2DEdgeDst( t )->pt.x);
							candidates[1].y = int(cvSubdiv2DEdgeDst( t )->pt.y);
						}
					}
				}

				if (edge_counter<3)
					trixel_edges.push_back((CvQuadEdge2D *)t);


				edge_counter++;
			}else{
				flag=1;
				break;
			}

			//process the next edge
			if(i==0){
				t = cvSubdiv2DGetEdge( t, CV_NEXT_AROUND_RIGHT );
			}else{
				t = cvSubdiv2DGetEdge( t, CV_NEXT_AROUND_LEFT );
			}


		}
		if(flag==0){
			//Computes the centroid
			centroid_x /= 3;
			centroid_y /= 3;
			centroid.x = int(centroid_x + 0.5);
			centroid.y = int(centroid_y + 0.5);


			IterVrtx = points_x_geometry.begin();
			Iter = trixel_edges.begin();
			vrtx_list = new vector <C_Vertex *>;
			vctr_list = new vector <C_Edge *>;

			trixel=new C_Trixel(1, centroid);


			for(int vrtx_counter=0;vrtx_counter<3;vrtx_counter++){
				C_Vertex *vrtx = new C_Vertex((*(IterVrtx+vrtx_counter)));
				this->vertex_list->push_back(vrtx); //list that contains all the vertices of the image.
				vrtx_list->push_back(vrtx); //trixel local list of vertices.
				cvSet2D(this->vertex_map,(*(IterVrtx+vrtx_counter)).y,(*(IterVrtx+vrtx_counter)).x,cvScalar((long)vrtx));

				if(vrtx_counter == 1){
					vctr=new C_Edge(*(vrtx_list->begin()),*(vrtx_list->begin()+1),(*(Iter-1))); //The id is updated when the edge is computed through the edge list.
					(*vrtx_list->begin())->Add_Vector(vctr);
					(*(vrtx_list->begin()+1))->Add_Vector(vctr);
					this->edge_list->push_back(vctr);   //list that contains all the edges of the image.
					vctr_list->push_back(vctr); //trixel local list of edges.
					vctr->Update_Trixel(trixel);
				}

				if(vrtx_counter == 2){
					vctr=new C_Edge(*(vrtx_list->begin()+1),*(vrtx_list->begin()+2),(*(Iter-1))); //The id is updated when the edge is computed through the edge list.
					(*(vrtx_list->begin()+1))->Add_Vector(vctr);
					(*(vrtx_list->begin()+2))->Add_Vector(vctr);
					this->edge_list->push_back(vctr);   //list that contains all the edges of the image.
					vctr_list->push_back(vctr); //trixel local list of edges.
					vctr->Update_Trixel(trixel);

					vctr=new C_Edge(*(vrtx_list->begin()+2),*(vrtx_list->begin()),(*Iter)); //The id is updated when the edge is computed through the edge list.
					(*(vrtx_list->begin()+2))->Add_Vector(vctr);
					(*vrtx_list->begin())->Add_Vector(vctr);
					this->edge_list->push_back(vctr);   //list that contains all the edges of the image.
					vctr_list->push_back(vctr); //trixel local list of edges.
					vctr->Update_Trixel(trixel);
				}
				Iter++;
			}
			trixel->Update_Edges(vctr_list);
			trixel->Update_Vertex(vrtx_list);


			C_Centroid_List *list_aux = new C_Centroid_List();
			list_aux->Add_Element((long)trixel);
			*data=list_aux;

			cvSet2D(pnt_map_image,trixel->Get_Centroid().y,trixel->Get_Centroid().x,cvScalar((long)list_aux));
			trixel_queue.push_back(trixel);

			break;
		}else
			flag=0;
	}
	trixel_edges.clear();
	points_x_geometry.clear();
}




int C_Tritom::Generate_New_Trixel(C_Edge *new_edge,list <C_Trixel *> &trixel_queue, IplImage *pnt_map_image, C_Trixel *trixel, C_Centroid_List **data){


	CvPoint centroid = cvPoint(0,0);
	int edge_counter=0;
	int flag=0;
	int already_added=0;
	vector <C_Edge *> *vctr_list;
	vector <C_Vertex *> *vrtx_list;
	C_Vertex *vrtx;
	C_Edge *vctr;
	CvScalar map_point;
	int ret_val=1;
	double centroid_x=0;
	double centroid_y=0;
	vector<CvPoint>::iterator IterVrtx;
	C_Trixel *aux;
	C_Centroid_List *list_aux = NULL;
	CvPoint candidates[2];


	int status=0;



	vector <CvPoint> points_x_geometry;
	vector <CvQuadEdge2D *> trixel_edges;


	vector<CvQuadEdge2D *>::iterator Iter;
	vector<C_Edge *>::iterator Iter2;






	vector<C_Edge *> *edge_vect = trixel->Get_Edge_List();
	Iter2 = edge_vect->begin();







	for(int i=0;i<2;i++){
		CvSubdiv2DEdge t = (CvSubdiv2DEdge)new_edge->Get_tmpEdge();

		flag=0;
		centroid_x=0;
		centroid_y=0;
		points_x_geometry.clear();
		trixel_edges.clear();
		edge_counter=0;
		candidates[0].x=-1;
		candidates[0].y=-1;
		candidates[1].x=-1;
		candidates[1].y=-1;
		//checks the present vertex and the next two vertices
		while(edge_counter<4)
		{
			//Saves the related vertex point
			CvSubdiv2DPoint* pt = cvSubdiv2DEdgeOrg( t );
            
            //already_added is check when the vector has been already added to the trixel
			already_added=0;

			if((int(pt->pt.x) < this->image->width)&&(int(pt->pt.y) < this->image->height)&&(int(pt->pt.y) > 0)&&(int(pt->pt.x) > 0)){

				if(edge_counter < 3){
					CvSubdiv2DPoint* newedge_pt_orig = cvSubdiv2DEdgeOrg( t );
					CvSubdiv2DPoint* newedge_pt_dest = cvSubdiv2DEdgeDst( t );
					for(int i=0;i<3;i++){
                        //The proposed edge (t) is compared to each edge that belongs to the present trixel
						CvSubdiv2DPoint* tedges_pt_orig = cvSubdiv2DEdgeOrg( (CvSubdiv2DEdge)(*(Iter2+i))->Get_tmpEdge() );
						CvSubdiv2DPoint* tedges_pt_dest = cvSubdiv2DEdgeDst( (CvSubdiv2DEdge)(*(Iter2+i))->Get_tmpEdge() );

						if(((newedge_pt_orig==tedges_pt_orig)||(newedge_pt_orig==tedges_pt_dest))&&((newedge_pt_dest==tedges_pt_dest)||(newedge_pt_dest==tedges_pt_orig))){
							//If matchs, alert variable is set to 1.
							already_added=1;
							break;
						}
					}
				}
				//There is no problem if they match once. Two trixels can share an edge.
				if((edge_counter==0)||(!already_added)){
					if(edge_counter == 0){
						candidates[0].x = int(cvSubdiv2DEdgeOrg( t )->pt.x);
						candidates[0].y = int(cvSubdiv2DEdgeOrg( t )->pt.y);
						candidates[1].x = int(cvSubdiv2DEdgeDst( t )->pt.x);
						candidates[1].y = int(cvSubdiv2DEdgeDst( t )->pt.y);

					}else{
						if((((candidates[0].x == int(cvSubdiv2DEdgeOrg( t )->pt.x))) && (candidates[0].y == int(cvSubdiv2DEdgeOrg( t )->pt.y))) || (((candidates[0].x == int(cvSubdiv2DEdgeDst( t )->pt.x))) && (candidates[0].y == int(cvSubdiv2DEdgeDst( t )->pt.y))))
						{
							centroid_x += candidates[1].x;
							centroid_y += candidates[1].y;
							points_x_geometry.push_back(candidates[1]);
							candidates[0].x = int(cvSubdiv2DEdgeOrg( t )->pt.x);
							candidates[0].y = int(cvSubdiv2DEdgeOrg( t )->pt.y);
							candidates[1].x = int(cvSubdiv2DEdgeDst( t )->pt.x);
							candidates[1].y = int(cvSubdiv2DEdgeDst( t )->pt.y);
						}else{
							if((((candidates[1].x == int(cvSubdiv2DEdgeOrg( t )->pt.x))) && (candidates[1].y == int(cvSubdiv2DEdgeOrg( t )->pt.y))) || (((candidates[1].x == int(cvSubdiv2DEdgeDst( t )->pt.x))) && (candidates[1].y == int(cvSubdiv2DEdgeDst( t )->pt.y))))
							{
								centroid_x += candidates[0].x;
								centroid_y += candidates[0].y;
								points_x_geometry.push_back(candidates[0]);
								candidates[0].x = int(cvSubdiv2DEdgeOrg( t )->pt.x);
								candidates[0].y = int(cvSubdiv2DEdgeOrg( t )->pt.y);
								candidates[1].x = int(cvSubdiv2DEdgeDst( t )->pt.x);
								candidates[1].y = int(cvSubdiv2DEdgeDst( t )->pt.y);
							}
						}
					}
					if ((edge_counter!=0)&&(edge_counter<3))
						trixel_edges.push_back((CvQuadEdge2D *)t);



					edge_counter++;
				}else{//if not, edges are processed in the opposite sense
					flag=1;
					break;
				}
			}else{
				flag=1;
				break;
			}

			//process the next edge
			if(i==0){
				t = cvSubdiv2DGetEdge( t, CV_NEXT_AROUND_RIGHT );
			}else{
				t = cvSubdiv2DGetEdge( t, CV_NEXT_AROUND_LEFT );
			}
		}
		if(flag==0)
			break;
	}

	int list_status=-2; //0->creates a new list, 1->it exists but it doesnt belong to any list

	if(flag==0){
		//Centroid is calculated
		centroid_x /= 3;
		centroid_y /= 3;
		centroid.x = int(centroid_x + 0.5);
		centroid.y = int(centroid_y + 0.5);



		 map_point = cvGet2D(pnt_map_image,centroid.y,centroid.x);
		 if(map_point.val[0]==0){
			 status = 0;
			 list_status = 0;
			 aux = NULL;
		 }else{ //if map_point.val[0]=!0 has something, then it is an available address
			 list_aux = (C_Centroid_List *) ((long)(map_point.val[0]));
			 for(int i=0;i<list_aux->Get_Size();i++){
				 //Takes the trixel
				 aux = (C_Trixel *) ((long)(list_aux->Get_Element(i)));
				 if(aux->Check_Vertex((*points_x_geometry.begin()),(*(points_x_geometry.begin()+1)),(*(points_x_geometry.begin()+2)))){
					 status = aux->Get_id();
					 break;
				 }else{
					 status=0;
					 list_status=1;
					 aux=NULL;
				 }
			 }
		 }


		 if(status != -1){ //if it doesnt belong to the trixel queue (otherwise, nothing is done, it waits until trixel is processed)
			 if (status == 0){ //if it is not an accepted trixel

				 IterVrtx = points_x_geometry.begin();
				 Iter = trixel_edges.begin();
				 vrtx_list = new vector <C_Vertex *>;
				 vctr_list = new vector <C_Edge *>;
				 int is_new[3];
				 is_new[0]=0;
				 is_new[1]=0;
				 is_new[2]=0;



				 C_Trixel *new_trixel=new C_Trixel(-1, centroid);
                //Iterates each vertex
				 for(int vrtx_counter=0;vrtx_counter<3;vrtx_counter++){
				     //Gets the trixel point in the pointers map
					 map_point = cvGet2D(this->vertex_map,(*(IterVrtx+vrtx_counter)).y,(*(IterVrtx+vrtx_counter)).x);
                     //If the vetex doesnt exist, then it is created and added to the map. It is also added to the trixel vertex list.
					 if(map_point.val[0]==0){
						 vrtx = new C_Vertex((*(IterVrtx+vrtx_counter)));
						 this->vertex_list->push_back(vrtx);
						 vrtx_list->push_back(vrtx);
						 cvSet2D(this->vertex_map,(*(IterVrtx+vrtx_counter)).y,(*(IterVrtx+vrtx_counter)).x,cvScalar((long)vrtx));
						 is_new[vrtx_counter]++;
					 }else{ //If not, it is created and added to the trixel vertex list.
						 vrtx = (C_Vertex *) ((long)(map_point.val[0]));
						 vrtx_list->push_back(vrtx);
					 }

					 if(vrtx_counter == 1){ //the first pointer is the present edge.
						 vctr_list->push_back(new_edge); //to the trixel local list.
						 new_edge->Update_Trixel(new_trixel);
					}

					if(vrtx_counter == 2){
						if((is_new[1]==1) || (is_new[2]==1)){
							vctr=new C_Edge(*(vrtx_list->begin()+1),*(vrtx_list->begin()+2),(*Iter));//The id is updated when the edge is computed through the edge list.
							(*(vrtx_list->begin()+1))->Add_Vector(vctr);
							(*(vrtx_list->begin()+2))->Add_Vector(vctr);
							this->edge_list->push_back(vctr);   //list that contains all the edges of the image.
							vctr_list->push_back(vctr); //trixel local list.
							vctr->Update_Trixel(new_trixel);
						 }else{ //If none of both are new, then it is necessary to check if already exists an edge between both vertices.
							 vctr = (*(vrtx_list->begin()+1))->Search_Vector(*(vrtx_list->begin()+2));
							 if(vctr == NULL){ //Both verties exist but there is no edge linking them.
								 vctr=new C_Edge(*(vrtx_list->begin()+1),*(vrtx_list->begin()+2),(*Iter));
								(*(vrtx_list->begin()+1))->Add_Vector(vctr);
								(*(vrtx_list->begin()+2))->Add_Vector(vctr);
								this->edge_list->push_back(vctr);   //list that contains all the edges of the image.
							 }
							 vctr_list->push_back(vctr); //trixel local list.
							 //The edge list and the trixel list are always updated.
							 vctr->Update_Trixel(new_trixel);
                        }
						Iter++;

						if((is_new[2]==1) || (is_new[0]==1)){
							vctr=new C_Edge(*(vrtx_list->begin()+2),*(vrtx_list->begin()),(*Iter));
							(*(vrtx_list->begin()+2))->Add_Vector(vctr);
							(*vrtx_list->begin())->Add_Vector(vctr);
							this->edge_list->push_back(vctr);   //list that contains all the edges of the image.
							vctr_list->push_back(vctr); //trixel local list.
							vctr->Update_Trixel(new_trixel);
						 }else{ //If none of both are new, then it is necessary to check if already exists an edge between both vertices.
							 vctr = (*(vrtx_list->begin()+2))->Search_Vector(*(vrtx_list->begin()));
							 if(vctr == NULL){ //Both verties exist but there is no edge linking them.
								 vctr=new C_Edge(*(vrtx_list->begin()+2),*(vrtx_list->begin()),(*Iter));
								(*(vrtx_list->begin()+2))->Add_Vector(vctr);
								(*vrtx_list->begin())->Add_Vector(vctr);
								this->edge_list->push_back(vctr);   //list that contains all the edges of the image.
							 }
							 vctr_list->push_back(vctr); //trixel local list.
							 //The edge list and the trixel list are always updated.
							 vctr->Update_Trixel(new_trixel);
						 }

					}

				 }
				 new_trixel->Update_Edges(vctr_list);
				 new_trixel->Update_Vertex(vrtx_list);


				 if(list_status==0){
					list_aux = new C_Centroid_List();
					list_aux->Add_Element((long)new_trixel);
					*data = list_aux;
				}else{
					list_aux->Add_Element((long)new_trixel);
				}
				cvSet2D(pnt_map_image,new_trixel->Get_Centroid().y,new_trixel->Get_Centroid().x,cvScalar((long)list_aux));
				trixel_queue.push_back(new_trixel);

			 }
		}
	}
	trixel_edges.clear();
	points_x_geometry.clear();
	return ret_val;
}




void C_Tritom :: Print_All_Trixels(IplImage *input_img, CvScalar color){
	vector<C_Edge *>::iterator Iter;

	if(this->edge_list!=NULL)
	{
		for(Iter = (*this->edge_list).begin();Iter != (*this->edge_list).end();Iter++)
		{
			(*Iter)->Draw_Edge(color, input_img);

		}
	}
}



void C_Tritom :: Highlight_Trixel(CvPoint pt, IplImage *input_img, CvScalar color){
	CvScalar map_point;

	map_point = cvGet2D(this->trixels_map,pt.y,pt.x);

	C_Trixel *aux = (C_Trixel *) ((long)(map_point.val[0]));

	if(aux != 0)
		this->Draw_trixel(aux,color, input_img);

}

CvScalar C_Tritom :: Get_Trixel_Color(CvPoint pt){
	CvScalar map_point = CV_RGB(0,0,0);
	CvScalar data = CV_RGB(0,0,0);

	map_point = cvGet2D(this->trixels_map,pt.y,pt.x);

	C_Trixel *aux = (C_Trixel *) ((long)(map_point.val[0]));

	if(aux != 0)
		data = aux->Get_Color();
    return data;

}


int C_Tritom :: Get_Trixel_Centroid(CvPoint pt, CvPoint *centroid){
	CvScalar map_point = CV_RGB(0,0,0);
    

	map_point = cvGet2D(this->trixels_map,pt.y,pt.x);

	C_Trixel *aux = (C_Trixel *) ((long)(map_point.val[0]));

	if(aux != 0)
	{
	    (*centroid) = aux->Get_Centroid();
        return 1;
    }
    return 0;

}

int C_Tritom :: GetNumTrixels()
{
    return int(this->trixel_list->size());
}

C_Trixel *C_Tritom :: Get_Trixel(CvPoint pt){
	CvScalar map_point;

	map_point = cvGet2D(this->trixels_map,pt.y,pt.x);

	C_Trixel *aux = (C_Trixel *) ((long)(map_point.val[0]));

	if(aux != 0)
		return aux;
    return NULL;

}



void C_Tritom :: Draw_trixel( C_Trixel *trixel, CvScalar color, IplImage *img )
{
	vector<C_Edge *>::iterator Iter;

	vector <C_Edge *> *edges = trixel->Get_Edge_List();
    if(edges!=NULL)
	{
		for(Iter = edges->begin();Iter != edges->end();Iter++)
		{
			(*Iter)->Draw_Edge(color, img);
		}
	}

}





void C_Tritom :: Draw_VrtxandVctr(CvPoint pt, IplImage *img, CvScalar color){
    CvScalar map_point;

    map_point = cvGet2D(this->vertex_map,pt.y,pt.x);

    C_Vertex *aux = (C_Vertex *) ((long)(map_point.val[0]));

    if(aux != 0){
        cvCircle( img, pt, 3, CV_RGB(255,255,255), CV_FILLED, 8, 0 );
        aux->Draw_Vector_List(color, img);
    }
}




void C_Tritom :: Draw_Mean_Image (IplImage *img) {

    for(int y=0;y<this->trixels_map->height;y++){
        for(int x=0; x<this->trixels_map->width;x++){
            CvScalar map_point;
            map_point = cvGet2D(this->trixels_map,y,x);
            if(map_point.val[0] != 0){
                C_Trixel *aux = (C_Trixel *) ((long)(map_point.val[0]));
                cvSet2D(img,y,x,aux->Get_Color());
            }
        }
    }

    vector<C_Edge *>::iterator Iter;
    vector<C_Trixel *>::iterator Iter2;


    vector<C_Trixel *> *vct;

    if(this->edge_list!=NULL)
    {
        for(Iter = (*this->edge_list).begin();Iter != (*this->edge_list).end();Iter++)
        {
            vct = (*Iter)->Get_Trixels();
            Iter2 = vct->begin();
            
            if((*Iter2)->Is_Valid()){
                (*Iter)->Draw_Edge((*Iter2)->Get_Color(),img);
            }
        }
    }

}






void C_Tritom :: DrawHist_Comp(CvPoint pt1, CvPoint pt2, IplImage *img, CvScalar color, IplImage **signal){



	CvScalar map_point;

	map_point = cvGet2D(this->trixels_map,pt1.y,pt1.x);
	C_Trixel *t1 = (C_Trixel *) ((long)(map_point.val[0]));

	map_point = cvGet2D(this->trixels_map,pt2.y,pt2.x);
	C_Trixel *t2 = (C_Trixel *) ((long)(map_point.val[0]));



	if((t1 != 0)&&(t2 != 0)){
	    if(t1->Is_Valid() && t2->Is_Valid()){
            this->Draw_trixel(t1, color, img);
            this->Draw_trixel(t2, color, img);
            if(this->comparison_obj->Generate_Comparison(t1->Get_Histogram(),t2->Get_Histogram()))
                this->comparison_obj->Get_Image(signal);
	    }
	}
}
