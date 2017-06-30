#include <wx/wx.h>
#include <wx/splitter.h>
#include <wx/socket.h>
#include <wx/list.h>
#include <wx/valgen.h>
#include <wx/listimpl.cpp>
#include <math.h>
#include <iostream>
#include <cstdlib>

// Будем отправлять клиентам
#define BLACK 99
#define WHITE 101
#define WIN 2001
#define LOSE 1999
#define READY 69
#define DRAW 2000
#define DISCONNECT 300
#define NEW 228

/// Создаем новый тип списков - список указателей на сокеты
WX_DECLARE_LIST(wxSocketBase, wxSocketList);
WX_DEFINE_LIST(wxSocketList);

// Структура с сокетом клиента
struct SockMess{
  wxSocketBase *sock;
  int a[20];
  int colour;
  int state;
};

class MyFrame : public wxFrame
{    
    // Здесь у нас будут отображаться сообщения
    wxTextCtrl * m_LOGTextCtrl;    
    // Здесь будет список клиентов
	wxListBox * m_ClientListBox;
	// Переменная для отправки сообщения (временный буфер)
    wxString m_MessageStr;
    SockMess sm[2];
    // Серверный сокет
    wxSocketServer * m_SocketServer;
    int users;
public:
    MyFrame(wxWindow * parent);    
    ~MyFrame();
    // Этот метод выводит список клиентов
    void ShowClientList();  
    void OnExit(wxCommandEvent & event);
    //void OnMessageInput(wxCommandEvent & event);
    void OnServerSocketEvent(wxSocketEvent & event);
    void OnClientSocketEvent(wxSocketEvent & event);
};

enum
{
    ID_LOG_TEXT_CTRL = 101,
    ID_MESSAGE_TEXT_CTRL,
    ID_CLIENT_LIST_BOX,
    ID_SOCKET_SERVER,
    ID_SOCKET_CLIENT
};

MyFrame::MyFrame(wxWindow * parent)
: wxFrame(parent, -1, _("Server"), wxDefaultPosition, wxSize(600, 450))
{        
   Connect(ID_SOCKET_SERVER, wxEVT_SOCKET, wxSocketEventHandler(MyFrame::OnServerSocketEvent));
   Connect(ID_SOCKET_CLIENT, wxEVT_SOCKET, wxSocketEventHandler(MyFrame::OnClientSocketEvent));
   Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(MyFrame::OnExit));
   //Connect(ID_MESSAGE_TEXT_CTRL,wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(MyFrame::OnMessageInput));

    wxMenuBar * menuBar = new wxMenuBar();
    SetMenuBar(menuBar);
    wxMenu * fileMenu = new wxMenu();
    fileMenu->Append(wxID_EXIT, _("Exit	Alt+F4"));
    menuBar->Append(fileMenu, _("File"));
    wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);        

    // Это у нас такой сплиттер. Позволяет изменять размеры поля сообщений и списка клиентов мышкой
    wxSplitterWindow * splitter_window = new wxSplitterWindow(this, wxID_ANY);
    sizer->Add(splitter_window, 1, wxEXPAND|wxALL, 5);

    m_LOGTextCtrl = new wxTextCtrl(splitter_window, ID_LOG_TEXT_CTRL, _("Hello!!!\n"), 
        wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_MULTILINE|wxNO_BORDER);

    m_LOGTextCtrl->SetBackgroundColour(*wxBLACK);
    m_LOGTextCtrl->SetForegroundColour(*wxGREEN);    

    wxArrayString dummy;
    m_ClientListBox = new wxListBox(splitter_window, ID_CLIENT_LIST_BOX, wxDefaultPosition, 
        wxSize(150, -1), dummy, wxNO_BORDER);

    splitter_window->SplitVertically(m_LOGTextCtrl, m_ClientListBox, splitter_window->GetSize().GetWidth()-150);
    splitter_window->SetMinimumPaneSize(50);
    splitter_window->SetSashGravity(1.0);

    wxTextCtrl * message_text_ctrl = new wxTextCtrl(this, ID_MESSAGE_TEXT_CTRL, wxEmptyString, 
        wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

    message_text_ctrl->SetValidator(wxGenericValidator(&m_MessageStr));
    sizer->Add(message_text_ctrl, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);
   
    users = 0;
    sm[0].state = 0;
    sm[1].state = 0;
    sm[0].colour = 0;
    sm[1].colour = 0;
     
    // Создаем серверный сокет
    // Сначала настраиваем порт 3000
    wxIPV4address addr;
    addr.Service(3000);
    // Потом создаем новый объект сокета с привязкой к конкретному порту
    m_SocketServer = new wxSocketServer(addr);
    // Если создалось, то говорим что "всё ОК"
    if (! m_SocketServer->Ok())
	  {
		m_LOGTextCtrl->AppendText(_("Could not listen at the specified port!!!\n\n"));
		return;
	  }
	else
	  {
		m_LOGTextCtrl->AppendText(_("Server listening.\n\n"));
	  }
    // Указываем, что сообщения от сокета будут обрабатываться нашей формой
    m_SocketServer->SetEventHandler(*this, ID_SOCKET_SERVER);
    // Указываем что обрабатываться будет толкьо соединение нового клиента
    m_SocketServer->SetNotify(wxSOCKET_CONNECTION_FLAG);
    // Говорим что уже можно обрабатывать события
    m_SocketServer->Notify(TRUE);
    CreateStatusBar();        
    Centre();
}

MyFrame::~MyFrame()
{
    for(int i = 0 ; i < users; i++)
		{
			sm[i].sock->Destroy();
		}
    m_SocketServer->Destroy();
};

void MyFrame::ShowClientList()
{
    m_ClientListBox->Clear();
    wxIPV4address addr;
    wxSocketBase * sock;
    // Добавляем в список адреса всех наших клиентов
     for(int i = 0; i < users; i++)
		{
			sock = sm[i].sock;;
			sock->GetLocal(addr);
			m_ClientListBox->Append(addr.IPAddress());
		}
}


void MyFrame::OnServerSocketEvent(wxSocketEvent & event)
{
    wxSocketBase *sock;
    wxIPV4address addr;
    int x;
    switch(event.GetSocketEvent())
    {
    case wxSOCKET_CONNECTION:
        // Зацептить нового клиента
        sock = m_SocketServer->Accept(FALSE);
        if (!sock) return;   
        // Указать что обрабатывать сообшение от этого сокета будет наша форма
        sock->SetEventHandler(*this, ID_SOCKET_CLIENT);
        //Указать что обрабатываться будет получение, отправка данных и дисконнект
        sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG | wxSOCKET_OUTPUT_FLAG);
        // Говорим что уже можно обрабатывать события
        sock->Notify(TRUE);
        // Выводим в ЛОГ что новый клиент добавлен
        sock->GetLocal(addr);
        if(users < 3){
			sm[users].sock = sock;
			users++;
        }
        if (users == 1){
			x = rand() % 2;
			if (x == 0)
				sm[0].colour = WHITE;
			else 
				sm[0].colour = BLACK;
			sm[0].sock->Write(&sm[0].colour, 1*sizeof(int));
		}
		if (users == 2){
			if (sm[0].colour == WHITE) 
				sm[1].colour = BLACK;
			else 
				sm[1].colour = WHITE;
			sm[1].sock->Write(&sm[1].colour, 1*sizeof(int));
			sm[0].state = READY;
			sm[1].state = READY;
			sm[0].sock->Write(&sm[0].state, 1*sizeof(int));
			sm[1].sock->Write(&sm[1].state, 1*sizeof(int));
		}	
        wxString s;
        s << wxT("Новый клиент: ") << addr.IPAddress() << wxT("\n");
        m_LOGTextCtrl->AppendText(s);
        // Выводим список клиентов
        ShowClientList();
        break; 
    }
}

void MyFrame::OnClientSocketEvent(wxSocketEvent & event)
{
       
    wxSocketBase *sock = event.GetSocket();
    wxIPV4address addr,addr2;
  
    sock->GetLocal(addr);
    wxString message;
    int buffer[20];
    int num_white = 0, num_black = 0;
    switch(event.GetSocketEvent())
    {
        // Если к нам пришло сообщение
        case wxSOCKET_INPUT:
        {        
            // Прочитали его в буфер
            sock->Read(buffer, 20*sizeof(int));
            // Если ошибка - выводим сообщение
            if(sock->Error())
            {
				message << wxT("Ошибка чтения \n");
				m_LOGTextCtrl->AppendText(message);
            }
            else
            {
                int who = 0;
                for(int i = 0; i < users; i++)
                {
					//sm[i].sock->GetLocal(addr2);
					if(sock == sm[i].sock){
						who = i;
		                break;
					}              
				}
				if (buffer[0] == NEW){
					if(users > 1){
					sm[(who + 1) % 2].sock->Write(buffer, 20*sizeof(int));		   
					if(sm[(who + 1)%2].sock->Error())
						m_LOGTextCtrl->AppendText(_("Error "));
					}
					message << wxT("Новая игра \n");
					m_LOGTextCtrl->AppendText(message);
					
				}
				for ( int j = 0; j < 20; j++){
					if (buffer[j] == 1)
						num_white++;
					if (buffer[j] == -1)
						num_black++;
				}
				if ((num_black == 1) && (num_white == 1)){
					sm[0].state = DRAW;
					sm[1].state = DRAW;
				}
				int another = (who + 1) % 2;
				
					if  (num_white == 0){
						if (sm[another].colour == WHITE) {
						   sm[another].state = LOSE;
						   sm[who].state = WIN;
					   }
						else{
							sm[another].state = WIN;
							sm[who].state = LOSE;
						}
						sm[another].sock->Write(&sm[another].state, 1*sizeof(int));	
						sm[who].sock->Write(&sm[who].state, 1*sizeof(int));		
					};
					
					if  (num_black == 0){
						if (sm[another].colour == WHITE){ 
						   sm[another].state = WIN;
						   sm[who].state = LOSE;
					   }
						else{
							sm[another].state = LOSE;
							sm[who].state = WIN;
						}
						sm[another].sock->Write(&sm[another].state, 1*sizeof(int));	
						sm[who].sock->Write(&sm[who].state, 1*sizeof(int));		
					};	
				if(users > 1){
					sm[(who + 1) % 2].sock->Write(buffer, 20*sizeof(int));		   
                if(sm[(who + 1)%2].sock->Error())
						m_LOGTextCtrl->AppendText(_("Error "));
				 }
            }
            break;
        }
        
        case wxSOCKET_LOST:
        {
		   int who;
           int cnt = users;
           for( int i = 0; i < users; i++){
               if(sock == sm[i].sock){
                 sm[i].sock = 0;
                 who = i;
                 cnt--;
               }
               users = (cnt > 0)? cnt : 0;
           }          
           sock->Destroy();
           sm[(who + 1) % 2].state = DISCONNECT;
           sm[(who + 1) % 2].sock->Write(&sm[(who + 1) % 2].state, 1*sizeof(int));	
           wxString s1;
           s1 << wxT("Соединение разорвано: ") << addr.IPAddress() << wxT("\n");
           m_LOGTextCtrl->AppendText(s1);
           break;
        }
        default:;
  }
};


void MyFrame::OnExit(wxCommandEvent & event)
{
    Close();
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MyFrame * frame = new MyFrame(NULL);    
    frame->Show();    
    SetTopWindow(frame);
    return true;
}

