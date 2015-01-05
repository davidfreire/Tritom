#include <time.h>





class C_Timer
{
private:
	clock_t	t_init;
	clock_t	t_end;



public:
	C_Timer(){};
	~C_Timer(){};

	//Inicia contador
	void Tic()
	{
		this->t_init=clock();
	}

	//Finaliza contador
	void Toc()
	{
		this->t_end=clock();
	}

	//Finaliza contador devolviendo segundos
	float Toc_seconds()
	{
		this->t_end=clock();
		long clicks=this->t_end - this->t_init;
		float seconds = (float)clicks/CLOCKS_PER_SEC;
		return seconds;
	}

	//Finaliza contador devolviendo clicks
	long Toc_clicks()
	{
		this->t_end=clock();
		long clicks=this->t_end - this->t_init;
		return clicks;
	}

	//Devuelve segundos, es necesario hacer un toc antes (de cualquier tipo)
	float Get_seconds()
	{
		long clicks=this->t_end - this->t_init;
		float seconds = (float)clicks/CLOCKS_PER_SEC;
		return seconds;
	}

	//Devuelve clicks, es necesario hacer un toc antes (de cualquier tipo)
	float Get_clicks()
	{
		long clicks=this->t_end - this->t_init;
		return clicks;
	}


};