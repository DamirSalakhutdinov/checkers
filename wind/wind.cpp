#include "wind.h"


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
	
	ready = 0; //NOT READY
	my_color = 0; //don't know 

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
	if(m_SocketClient){
		m_SocketClient->Destroy();
	}

}


void AddE::OnQuit(wxCommandEvent& event){
	Close(true);
};

void AddE::OnNew(wxCommandEvent& event){
	for(int i = 0; i < 20; i++)
		dp->pl.a[i] = FREE;
	for(int i = 2; i < 6; i++)
		dp->pl.a[i] = WHITE;
	for(int i = 12; i < 16; i++)
		dp->pl.a[i] = BLACK;
	dp->pl.turn = 1;
	if(dp->m_sc){
		if(dp->pl.my_color == WHITE){
			wxMessageBox(wxT("Starting new game?\nYour color is WHITE"), wxT("GOCHECKERSGO"));
		}else{
			wxMessageBox(wxT("Starting new game?\nYour color is BLACK"), wxT("GOCHECKERSGO"));
		}
		int new_game = 228;
		dp->m_sc->Write(&new_game, sizeof(int));
	
	}
	dp->pl.stepClear();
	dp->Refresh();
	
};

void AddE::OnConnect(wxCommandEvent & event)
{
    // Если мы вдруг нажали на кнопку КОННЕКТ когда соединение уже установлено, то выход (воoбще, такого не должно случиться...)
    if(m_SocketClient) 
    {
    	wxMessageBox(wxT("You are connected!"), wxT("CHECKERS 1.0"));
    	return;
    }
    // Показываем диаложек для ввода адреса (или хоста) сервера
    wxString addr_str = wxGetTextFromUser(_("Введите адрес сервера:"), _("Соединение"), wxT("192.168.43.32"));//localhost
    // Настраиваем адрес для подключения
    wxIPV4address addr;
    addr.Service(3000);
    addr.Hostname(addr_str);
    // Создаем сокет
    m_SocketClient = new wxSocketClient;
    // Привязываем его к нашей форме
    m_SocketClient->SetEventHandler(*this, ID_SOCKET_CLIENT);
    m_SocketClient->SetNotify(wxSOCKET_CONNECTION_FLAG|wxSOCKET_INPUT_FLAG|wxSOCKET_OUTPUT_FLAG|wxSOCKET_LOST_FLAG);
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
            wxMessageBox(wxT("Соединение установлено!"), wxT("CHECKERS 1.0"));
            if(dp->pl.ready == 0)
          	m_ss<<wxT("Ждите соперника..");
       
        }
        else
        {
        wxMessageBox(wxT("Соединение не установлено!"), wxT("CHECKERS 1.0"));
          m_ss<<wxT("Соединение не установлено..");
        }
        dp->dpsb->SetStatusText(m_ss);
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
    dp->m_sc = m_SocketClient;
    dp->pl.ready = 0;
    wxString ms;
    wxMessageBox(wxT("Соединение прервано!"), wxT("CHECKERS 1.0"));
    ms<<wxT("Соединение прервано..");
    sb->SetStatusText(ms);
};

void AddE::OnClientSocketEvent(wxSocketEvent & event)
{    
	wxSocketBase *sock = event.GetSocket();
	wxIPV4address addr;
	int buffer[20];
	wxString m_ss;
	
	wxString ss;
    switch(event.GetSocketEvent())
    {
        // Если пришло сообщение
        case wxSOCKET_INPUT:
        {       
        //buffer[0] == 101 - YOU ARE WHITE
        //buffer[0] == 99 - YOU ARE BLACK
        //buffer[0] == 2001 - YOU WIN
        //buffer[0] == 1999 - YOU LOOSE
        //buffer[0] == 2000 - DRAW
        //buffer[0] == 69 - READY
        //buffer[0] == 300 - DISCONNECT
            // Прочитали
            sock->Read(buffer, 20*sizeof(int));
            // Поругались, если ошибка
            if(sock->Error())
            {
            	m_ss<<wxT("Ошибка чтения данных..");
               dp->dpsb->SetStatusText(m_ss);
            }
            else
            {
            	switch(buffer[0])
            	{
            		case 101:
            		{
            			dp->pl.my_color = WHITE;
            			break;
            		}
            		case 99:
            		{
            			dp->pl.my_color = BLACK;
            			break;
            		}
            		case 2001:
            		{
            			wxMessageBox(wxT("You WON!"), wxT("Tadam!!"));
            			break;
            		}
            		case 1999:
            		{
            			wxMessageBox(wxT("You LOST :("), wxT("Do not worry!"));
            			break;
            		}
            		case 2000:
            		{
            			wxMessageBox(wxT("It's DRAW!"), wxT("Play again?"));
            			break;
            		}
            		case 69:
            		{
            			dp->pl.ready = 1;
           			if(dp->pl.turn == dp->pl.my_color){
					ss << wxT("Ваш ход..");
					wxMessageBox(wxT("Are you READY?\nYour color is WHITE"), wxT("GOCHECKERSGO"));
				}
				else{
					ss << wxT("Ход соперника..");
					wxMessageBox(wxT("Are you READY?\nYour color is BLACK"), wxT("GOCHECKERSGO"));
				}
				dp->dpsb->SetStatusText(ss);
            			break;
            		}
            		case 300:
            		{
            			wxCommandEvent ch;
            			this->OnDisconnect(ch);
				break;
            		}
            		case 228:
            		{
            			if(dp->pl.my_color == WHITE){
            				wxMessageBox(wxT("Starting new game?\nYour color is WHITE"), wxT("GOCHECKERSGO"));
            			}else{
            				wxMessageBox(wxT("Starting new game?\nYour color is BLACK"), wxT("GOCHECKERSGO"));
            			}
            			for(int i = 0; i < 20; i++)
					dp->pl.a[i] = FREE;
				for(int i = 2; i < 6; i++)
					dp->pl.a[i] = WHITE;
				for(int i = 12; i < 16; i++)
					dp->pl.a[i] = BLACK;
				dp->pl.turn = 1;

				dp->pl.stepClear();
				dp->Refresh();
            			break;
            		}
            		default:{
            			 //перерисуем
				for(int i = 0; i < 20; i++){
					dp->pl.a[i] = buffer[i];
				}
				if(dp->pl.turn == WHITE)
					dp->pl.turn = BLACK;
				else 
					dp->pl.turn = WHITE;	
				//m_ss<<wxT("Получено..");
				//sb->SetStatusText(m_ss);
				dp->Refresh();            		
            		}
            	}          	
            }
            break;
        }
        case wxSOCKET_LOST:
        {
            if(dp->m_sc != NULL)
            {
            wxMessageBox(wxT("Произошел разрыв соединения.."), wxT("CHECKERS 1.0"));
            m_ss<<wxT("Разрыв соединения..");
            sb->SetStatusText(m_ss);
            }
            // Если дисконнект
            sock->GetLocal(addr);
            // Обнулили наш сокет
            if(sock == m_SocketClient)
            {
                m_SocketClient = NULL;
                dp->m_sc = m_SocketClient;
            }
            // Удалили
            sock->Destroy();    
                    
            // Сказали что произошел дисконнект
           
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

	dc.SetPen(wxPen(wxColour(255,0,0), 2));
	int cross_pos;
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 4; j++){			
			cross_pos = pl.go[i].dead[j];

			if(cross_pos != -1){					
					dc.DrawLine(pl.pos[cross_pos] ,wxPoint(pl.pos[cross_pos].x + 2 * e, pl.pos[cross_pos].y + 2 * e));
					dc.DrawLine(wxPoint(pl.pos[cross_pos].x, pl.pos[cross_pos].y + 2 * e), 
						wxPoint(pl.pos[cross_pos].x + 2 * e, pl.pos[cross_pos].y));
			}
		}
	}

	wxString ss;

	
	if(m_sc){
		if(pl.ready == 1){
			if(pl.turn == pl.my_color)
				ss << wxT("Ваш ход..");
			else
				ss << wxT("Ход соперника..");
		}else
			ss << wxT("Ждите соперника..");
	}else{
		if(pl.turn == WHITE)
			ss << wxT("Ход белых..");
		else
			ss << wxT("Ход чёрных..");
	}
	dpsb->SetStatusText(ss);
	
	

};

void DrawPanel::OnDclick(wxMouseEvent& event){      
	wxClientDC dc(this);
	dc.SetBrush(wxBrush(wxColour(0,255,0)));        

	int num = pl.getNum(event.GetPosition());
	bool in_checker = num != -1 && pl.a[num] != 0 && pl.a[num] == pl.turn && !(pl.ingreen(num));
	bool in_green = num != -1 && pl.ingreen(num);

	if(m_sc && pl.ready){
		if(in_checker && pl.a[num] == pl.turn && pl.turn == pl.my_color)
			pl.stepPrep(num);
		else if(in_green) {
			pl.act(num);
	    		// Отослать сообщение
	    		wxString m_ss;
	    		//m_ss<<wxT("Отправлено..");
			//dpsb->SetStatusText(m_ss);
	    		m_sc->Write(pl.a, 20*sizeof(int));
		} else 
			pl.stepClear();
	} else if(m_sc == NULL) {
		if(in_checker && pl.a[num] == pl.turn)
			pl.stepPrep(num);
		else if(in_green) 
			pl.act(num);
	    	else 
			pl.stepClear();
	}

		
	this->Refresh();
	
};


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

IMPLEMENT_APP(Pril);

bool Pril::OnInit()
{
  AddE *ae=new AddE(wxT("Странные шашки"));
  ae->Show(true);
  return true;
};





