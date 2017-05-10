#include "wind.h"

// constructor check20()
check20::check20(){
	for(int i = 0; i < 20; i++)
		a[i] = FREE;
	for(int i = 2; i < 6; i++)
		a[i] = WHITE;
	for(int i = 12; i < 16; i++)
		a[i] = BLACK;

	for(int i = 0; i < 8; i++)
		pos[i] = wxPoint(20 + 30 * i,20);
	pos[8] = wxPoint(230,50);
	pos[9] = wxPoint(230,80);
	for(int i = 0; i < 8; i++)
		pos[10 + i] = wxPoint(230 - i * 30,110);
	pos[18] = wxPoint(20,80);
	pos[19] = wxPoint(20,50);
	 
	turn = WHITE;	

	stepClear();
};

int check20::getNum(wxPoint t){
	for(int i = 0; i < 20; i++)
		if(pos[i].x < t.x && t.x < pos[i].x + 2 * e
		&& pos[i].y < t.y && t.y < pos[i].y + 2 * e)
			return i;
	return -1;
};


void check20::stepClear(){
	for(int i = 0; i < 2; i++){
		go[i].who = -1;
		go[i].green = -1;
		for(int j = 0; j < 4; j++)		
			go[i].dead[j] = -1;
	}
};


bool check20::kill_opp(){
	Iter t(a);
	
	for(int i = 0; i < 20; i++)
		if( a[t[i]] == turn && 
		(a[t[i-1]] == -turn || a[t[i+1]] == -turn))
			return true;
	return false;
			
};

int check20::win_check(){
	int white_num = 0, black_num = 0;
	for(int i = 0; i < 20; i++)
		if(a[i] == 1)
			white_num++;
		else if(a[i] == -1)
			black_num++;

	if(black_num == 0 || white_num == 0)
		return 1;
	else if(black_num == 1 && white_num == 1)
		return -1;
	else
		return 0;
	
};

void check20::stepPrep(int i){
	stepClear();
	Iter t(a);
	
	if(kill_opp()) {	
		int j = i, k = 0;
		while(a[t[j-2]] == FREE && a[t[i]] * a[t[j-1]] == -1){
			go[0].who = i;		
			go[0].green = t[j-2];
			go[0].dead[k] = t[j-1];
			k++;
			j = t[j-2];
		};

		j = i; k = 0;
		while(a[t[j+2]] == FREE && a[t[i]] * a[t[j+1]] == -1){
			go[1].who = i;		
			go[1].green = t[j+2];
			go[1].dead[k] = t[j+1];
			k++;
			j = t[j+2];
		};
	} else {
	
		if(a[t[i-1]] == FREE && go[1].dead[0] == -1){
			go[0].who = i;		
			go[0].green = t[i-1];
		} 
		
		if(a[t[i+1]] == FREE && go[0].dead[0] == -1){
			go[1].who = i;		
			go[1].green = t[i+1];
		}
	}
};

bool check20::ingreen(int i){
	if(go[0].green == i || go[1].green == i)
		return 1;
	else
		return 0;

};


void check20::act(int i){
	if(go[0].green == i){
		a[go[0].green] = a[go[0].who];
		a[go[0].who] = 0;
		for(int j = 0; j < 4; j++)
			if(go[0].dead[j] != -1)
				a[go[0].dead[j]] = FREE;
	} else if(go[1].green == i){
		a[go[1].green] = a[go[1].who];
		a[go[1].who] = 0;
		for(int j = 0; j < 4; j++)
			if(go[1].dead[j] != -1)
				a[go[1].dead[j]] = FREE;	
	}

	if(turn == WHITE)
		turn = BLACK;
	else 
		turn = WHITE;	
	

	//m_SocketClient
	stepClear();

};

const int ID_DRAW =1001;
const int ID_BUTTON =1002;

enum
{
    ID_CONNECT,
    ID_DISCONNECT,
    ID_SOCKET_CLIENT
};

BEGIN_EVENT_TABLE(AddE, wxFrame)
	EVT_SOCKET(ID_SOCKET_CLIENT, AddE::OnClientSocketEvent)
	EVT_BUTTON(ID_CONNECT, AddE::OnConnect)
	EVT_BUTTON(ID_DISCONNECT, AddE::OnDisconnect)
END_EVENT_TABLE()

//constructor AddE
AddE::AddE(const wxString &title):wxFrame(NULL,wxID_ANY,title,wxDefaultPosition,wxSize(500,400)){


	ss<<wxT("QQQ!");
	sb=CreateStatusBar();
	sb->SetStatusText(ss);

	m_pan= new wxPanel(this,wxID_ANY);

	bt= new wxButton(m_pan,wxID_EXIT,wxT("Quit"),wxPoint(10,10));
	ng= new wxButton(m_pan,ID_BUTTON,wxT("New"),wxPoint(100,10));
	dp=new DrawPanel(m_pan, sb, m_SocketClient);

	menubar = new wxMenuBar;
	file = new wxMenu;
	file->Append(wxID_EXIT, wxT("&Quit"));
	file->Append(ID_BUTTON, wxT("&New"));
	menubar->Append(file, wxT("&File"));
	SetMenuBar(menubar);
	Connect(wxID_EXIT,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(AddE::OnQuit));

	Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(AddE::OnQuit));
	Connect(ID_BUTTON, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AddE::OnNew));
	Connect(ID_BUTTON, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(AddE::OnNew));
	
	wxButton * connect_btn = new wxButton(m_pan, ID_CONNECT, wxT("Connect"), wxPoint(190,10));
	    wxButton * disconnect_btn = new wxButton(m_pan, ID_DISCONNECT, wxT("Disonnect"),wxPoint(280,10));
  
};

AddE::~AddE()
{
    // При удалении формы, закрываем наш сокет
    if(m_SocketClient)
    {
        m_SocketClient->Destroy();
    }
}


void AddE::OnQuit(wxCommandEvent& event){
	Close(true);
};

void AddE::OnNew(wxCommandEvent& event){
	delete dp;
	dp=new DrawPanel(m_pan, sb, m_SocketClient);
};

void AddE::OnConnect(wxCommandEvent & event)
{
    // Если мы вдруг нажали на кнопку КОННЕКТ когда соединение уже установлено, то выход (воoбще, такого не должно случиться...)
    if(m_SocketClient) return;
    // Показываем диаложек для ввода адреса (или хоста) сервера
    wxString addr_str = wxGetTextFromUser(_("Введите адрес сервера:"), _("Соединение"), wxT("localhost"));
    // Настраиваем адрес для подключения
    wxIPV4address addr;
    addr.Service(3000);
    addr.Hostname(addr_str);
    // Создаем сокет
    m_SocketClient = new wxSocketClient;
    // Привязываем его к нашей форме
    m_SocketClient->SetEventHandler(*this, ID_SOCKET_CLIENT);
    m_SocketClient->SetNotify(wxSOCKET_CONNECTION_FLAG|wxSOCKET_INPUT_FLAG|wxSOCKET_OUTPUT_FLAG);
    m_SocketClient->Notify(TRUE);
    dp->m_sc = m_SocketClient;
    if(m_SocketClient)
    {
        // Пытаемся сконнектиться с сервером
        m_SocketClient->Connect(addr, false);
        // Ожидание соединения 10 сек.
        m_SocketClient->WaitOnConnect(10);
           dp->m_sc = m_SocketClient;
        // Если соединение установлено...
        wxString m_ss;
        if(m_SocketClient->IsConnected())
        {
            // Говорим что все ОК
          m_ss<<wxT("Соединение установлено..");
       
        }
        else
        {
          m_ss<<wxT("Соединение не установлено..");
        }
        sb->SetStatusText(m_ss);
    }
};

void AddE::OnDisconnect(wxCommandEvent & event)
{
    // При нажатии на кнопку ДИСКОННЕКТ прерываем соединение и удаляем сокет
if(m_SocketClient)
    {
        if(m_SocketClient->IsConnected()) m_SocketClient->Close();
        m_SocketClient->Destroy();
    }
    m_SocketClient = NULL;
    wxString ms;
    ms<<wxT("Соединение прервано..");
    sb->SetStatusText(ms);
};

void AddE::OnClientSocketEvent(wxSocketEvent & event)
{    
	wxSocketBase *sock = event.GetSocket();
	wxIPV4address addr;
	int buffer[20];
	wxString m_ss;
    switch(event.GetSocketEvent())
    {
        // Если пришло сообщение
        case wxSOCKET_INPUT:
        {       
            // Прочитали
            sock->Read(buffer, 20*sizeof(int));
            // Поругались, если ошибка
            if(sock->Error())
            {
            	m_ss<<wxT("Ошибка чтения данных..");
               sb->SetStatusText(m_ss);
            }
            else
            {
                //перерисуем
                for(int i = 0; i < 20; i++){
                	dp->pl.a[i] = buffer[i];
                }
                if(dp->pl.turn == WHITE)
			dp->pl.turn = BLACK;
		else 
			dp->pl.turn = WHITE;	
                m_ss<<wxT("Получено..");
                sb->SetStatusText(m_ss);
                dp->Refresh();
            }
            break;
        }
        case wxSOCKET_LOST:
        {
            // Если дисконнект
            sock->GetLocal(addr);
            // Обнулили наш сокет
            if(sock == m_SocketClient)
            {
                m_SocketClient = NULL;
            }
            // Удалили
            sock->Destroy();            
            // Сказали что произошел дисконнект
            wxString s1;
            break;
        }
        default:;
  }
};

DrawPanel::DrawPanel(wxPanel *parent, wxStatusBar *sb, wxSocketClient *sc):wxPanel(parent, -1,wxPoint(50,100),wxSize(280,160),wxBORDER_SUNKEN){
	// подключили панель к событиям рисования
	Connect(wxEVT_PAINT,wxPaintEventHandler(DrawPanel::OnPaint));
	Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(DrawPanel::OnDclick));
	
	dpsb = sb;
	m_sc = sc;
	pl = check20();
};

void DrawPanel::OnPaint(wxPaintEvent& event){
			
	wxPaintDC dc(this);
	dc.SetBrush(wxBrush(wxColour(255,255,255)));
	dc.SetPen(wxPen(wxColour(0,0,0), 1));
	for(int i = 0; i < 20; i++)
		dc.DrawRectangle(pl.pos[i], wxSize(30,30));
	
	dc.SetBrush(wxBrush(wxColour(0,255,0)));
	for(int i = 0; i < 2; i++)
		if(pl.go[i].green != -1)
			dc.DrawRectangle(pl.pos[pl.go[i].green], wxSize(30,30));

	dc.SetBrush(wxBrush(wxColour(255,255,255)));
	for(int i = 0; i < 20; i++)
		if(pl.a[i] == 1){
			dc.SetPen(wxPen(wxColour(0,0,0), 2));
			dc.DrawCircle(pl.pos[i].x + e, pl.pos[i].y + e, rad);
		}
	
	dc.SetBrush(wxBrush(wxColour(100,100,100)));
	for(int i = 0; i < 20; i++)
		if(pl.a[i] == -1){
			dc.SetPen(wxPen(wxColour(0,0,0), 2));
			dc.DrawCircle(pl.pos[i].x + e, pl.pos[i].y + e, rad);
		}
	wxString ss;

	if(pl.turn == WHITE)
		ss<<wxT("Ход белых..");
	else
		ss<<wxT("Ход чёрных..");

	dpsb->SetStatusText(ss);
	
	

};

void DrawPanel::OnDclick(wxMouseEvent& event){      
	wxClientDC dc(this);
	dc.SetBrush(wxBrush(wxColour(0,255,0)));        

	int num = pl.getNum(event.GetPosition());
	if(num != -1 && pl.a[num] != 0 && pl.a[num] == pl.turn && !(pl.ingreen(num)))
		pl.stepPrep(num);
	else if(num != -1 && pl.ingreen(num)){
		pl.act(num);
		
		if(m_sc){
	    		// Отослать сообщение
	    		wxString m_ss;
	    		m_ss<<wxT("Отправлено..");
		        dpsb->SetStatusText(m_ss);
	    		
	    		m_sc->Write(pl.a, 20*sizeof(int));
		}

	} else 
		pl.stepClear();

	
	this->Refresh();
	if(pl.win_check())	
		wxMessageBox(wxT("Someone won!"), wxT("Tadam!!"));
	
};

///////////////// Iter /////////////////

check20::Iter::Iter(int* a){
	idx = 0;
	arr = a;	
};

check20::Iter* check20::Iter::operator=(int i){
	idx = (i + 20) % 20;
};

check20::Iter* check20::Iter::operator++(int){
	idx = (idx + 1) % 20;
};

int check20::Iter::operator[](int i){		
	return (i + 20) % 20;
};


////////////////////////////////////////////////

IMPLEMENT_APP(Pril);

bool Pril::OnInit()
{
  AddE *ae=new AddE(wxT("Странные шашки"));
  ae->Show(true);
  return true;
};





