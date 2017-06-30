#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/valgen.h>
#include <wx/statusbr.h>

#define e 15
#define BLACK -1
#define WHITE 1
#define FREE 0			
#define rad 12               

struct Step{			//это структура для записи возможных ходов при клике на определенную шашку
	int green;		//при нажатии возможные ходы будут окрашиваться в зеленый
	int who;		//номер клетки, на которую было произведено нажатие
	int dead[4];		//кто умирает при таком ходе
};

class check20{			//контейнер для хранения положения шашек
public:
	int a[20];  		//если a[i] = 1, то на i-ой клетке стоит белая шашка, если -1 - черная, 0 - ничего
	wxPoint pos[20];	//координаты клеток в отведенном для поля окне
	Step go[2];		//для хранения двух возможных ходов при клике на шашку
	int turn;		//для хранения информации о том, кто в данный момент ходит
	int my_color;		//цвет шашек получаемый пользователем при игре по сети
	int ready;		//информация о готовности пользователи при игре по сети

	check20();
	int getNum(wxPoint);	//выдает номер клетки по координате клика
	void stepClear();	//убирает с поля целеные клетки, означающие возможные ходы при клике мимо шашек
	void stepPrep(int);	//подготавливает ходы при клике на определенную шашку, окрашивает вожные ходы в зеленый
	bool ingreen(int);	//проверяет, окрашено ли поле в зеленый
	bool kill_opp();	//проверяет, есть ли возможность рубить, если есть, то иных ходов делать нельзя
	int win_check();	//проверяет, опеделился ли победитель в партии
	void act(int);		//осуществляет выбранный ход
	


	class Iter{		//так как поле представляет из себя кольцо из 20 клеток, а колируется как массив
		int* arr;	//был создан итератор, чтобы не беспокоиться о выходе за пределы массива
		int idx;	//при просмотре соседних клеток

		public:

		Iter(int*);
		Iter* operator=(int);
		Iter* operator++(int);
		int operator[](int);
	};
};

//класс, описывающий панель с полем для игры
class DrawPanel: public wxPanel{
  public:	
        DrawPanel(wxPanel *parent, wxStatusBar *sb, wxSocketClient *sc); //в конструкторе указывается адрес объекта, который ее содержит
        void OnPaint(wxPaintEvent & event);	//для рисования
	void OnDclick(wxMouseEvent& event); 	//для работы с кликами
	check20 pl;				//информация о поле
	wxStatusBar *dpsb;			//для вывода сообщений в статус бар
	wxSocketClient *m_sc;			//информация о сокете
};

// Фрейм, содержащий предыдущую панель, кнопки, статус бар
class AddE: public wxFrame{   
	wxSocketClient * m_SocketClient;		//клиентский сокет 
public:
	AddE(const wxString& title);
	~AddE(); 
	DECLARE_EVENT_TABLE()    
	
	wxPanel *m_pan; 		
	wxButton *bt, *ng; 		
	DrawPanel *dp; 		 
	
	wxMenuBar *menubar;	//полоска для меню
	wxMenu *file; 		
	wxStatusBar *sb;	// статус бар
	wxString ss;
	void OnQuit(wxCommandEvent& event);	//работает при нажатии на кнопку "quit"
	void OnNew(wxCommandEvent& event);	//new - новая игра
	
	// Обработчик событий от сокета
	void OnClientSocketEvent(wxSocketEvent & event);
	// Обработчик кнопки КОННЕКТ
	void OnConnect(wxCommandEvent & event);
	// Обработчик кнопки ДИСКОННЕКТ
	void OnDisconnect(wxCommandEvent & event);
};


class Pril: public wxApp{
    public:
        virtual bool OnInit();
};


