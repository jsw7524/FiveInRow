#include <stdio.h>
#include <windows.h>
#include "resource.h"
extern void ShowBoard();
extern void InitializeHotSpot();
extern void SetHotSpot(int X,int Y,int Side);
extern int Playable(int Y,int X,int Player,int ReverseRecord[9],int MyTestTable[8][8]);
extern int Reverse(int Y,int X,int Player,int ReverseRecord[9],int MyTestTable[8][8]);
//-----------------------------------------------------------

#define player1 1	//定義玩家1
#define player2 -1
#define VS_COM 1	//定義遊戲方式，對電腦
#define VS_PLAYER 2	//單機對戰
#define VS_CONNECT 3	//連線對戰
#define BkColor RGB(0,150,250)	//定義棋盤顏色
#define board_width 50		//棋盤格寬度

//-----------------------------------------------------------

int MyTable[8][8],	//棋盤陣列，判斷是否已下子
    who_play,			//判斷現在誰玩
    play_mode;			//判斷遊戲方式

int  Round;

//-----------------------------------------------------------

struct Move		//定義結構儲存棋子的位置
{
    int x;
    int y;
    int Score;
};

//----------------------------------------------------------
extern Move NextMove(int Side);

//----------------------------------------------------------

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);//視窗訊息處理程式
void PaintBoard(HDC,RECT,POINT);//繪製棋盤的函式
void PaintMyTable(HWND,RECT,Move,int);//繪製棋子的函式
void ResetGame();//重新設定遊戲資訊

//----------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
    static TCHAR AppName[]=TEXT("Reversi");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass= {CS_SAVEBITS,WndProc,0,0,hInstance,
                        LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON)),LoadCursor(NULL,IDC_ARROW),
                        (HBRUSH)GetStockObject(BLACK_BRUSH),AppName,AppName
                       };
    if(!RegisterClass(&wndclass))
    {
        MessageBox(NULL,TEXT("註冊視窗失敗"),AppName,MB_ICONERROR);
        return 0;
    }
    hwnd=CreateWindowEx(WS_EX_TOPMOST,AppName,"黑白棋 by Netsphere",
                        WS_CAPTION|WS_BORDER|WS_SYSMENU|WS_MINIMIZEBOX,
                        0,0,500,500,
                        NULL,LoadMenu(hInstance,MAKEINTRESOURCE(IDR_MENU)),hInstance,NULL);
    ShowWindow(hwnd,nShowCmd);
    UpdateWindow(hwnd);
    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    HMENU hMenu;
    static RECT rt_board,//儲存棋盤大小的變數
           rt_MyTable;//儲存棋子大小的變數
    static POINT pt_board;//儲存棋盤格線初始座標的變數
    Move pchoice;
    int check_win,i,j,w,b;
    TCHAR text[20];
    RECT rt_text;
    int ReverseRecord[9]= {0};
    Move PreviousMove;
    switch(msg)
    {
    case WM_CREATE:
        ResetGame();
        Round=0;
        return 0;
    case WM_SIZE:

        SetRect(&rt_board,0,0,400,400);//設定棋盤大小
        SetRect(&rt_MyTable,0,0,50,50);
        pt_board.x=25;//設定第一調棋盤格線的起始座標位置
        pt_board.y=25;
        return 0;
    case WM_PAINT:
        hdc=BeginPaint(hwnd,&ps);
        PaintBoard(hdc,rt_board,pt_board);//繪製棋盤
        for(i=0; i<8; i++)
        {
            for(j=0; j<8; j++)
            {
                pchoice.x=j;
                pchoice.y=i;
                switch(MyTable[i][j])
                {
                case player1:
                    PaintMyTable(hwnd,rt_MyTable,pchoice,player1);
                    break;
                case player2:
                    PaintMyTable(hwnd,rt_MyTable,pchoice,player2);
                    break;
                }
            }
        }
        if(who_play==-1)
        {
            SetRect(&rt_text,400,70,500,110);
            SetTextColor(hdc,RGB(255,255,255));
            SetBkColor(hdc,BkColor);
            DrawText(hdc,"黑子",-1,&rt_text,DT_CENTER);
        }
        else
        {
            SetRect(&rt_text,400,70,500,110);
            SetTextColor(hdc,RGB(0,0,0));
            SetBkColor(hdc,BkColor);
            DrawText(hdc,"白子",-1,&rt_text,DT_CENTER);
        }
        for (w=0,b=0,i=0; i<8; i++)
        {
            for (j=0; j<8; j++)
            {
                if (MyTable[i][j]==1)
                {
                    b++;
                }
                else if (MyTable[i][j]==-1)
                {
                    w++;
                }
            }
        }
        SetRect(&rt_text,400,200,500,240);
        SetTextColor(hdc,RGB(0,0,0));
        SetBkColor(hdc,BkColor);
        sprintf(text,"白子數:%d",b);
        DrawText(hdc,text,-1,&rt_text,DT_CENTER);

        SetRect(&rt_text,400,250,500,290);
        sprintf(text,"黑子數:%d",w);
        DrawText(hdc,text,-1,&rt_text,DT_CENTER);

        EndPaint(hwnd,&ps);
        return 0;
    case WM_LBUTTONDOWN:
        pchoice.x=LOWORD(lParam)-pt_board.x+0.5*board_width;
        pchoice.y=HIWORD(lParam)-pt_board.y+0.5*board_width;
        pchoice.x=pchoice.x/board_width;//轉換為棋盤格交線位置
        pchoice.y=pchoice.y/board_width;

        if(who_play==-1&&MyTable[pchoice.y][pchoice.x]==0&&(pchoice.x>=0&&pchoice.x<=7)&&(pchoice.y>=0&&pchoice.y<=7))
        {
            if (Playable(pchoice.y,pchoice.x,who_play,ReverseRecord,MyTable)!=0)
            {
                PaintMyTable(hwnd,rt_MyTable,pchoice,who_play);//畫棋子
                MyTable[pchoice.y][pchoice.x]=who_play;
                Reverse(pchoice.y,pchoice.x,who_play,ReverseRecord,MyTable);
                Round++;
                who_play=(who_play==player1?player2:player1);
                InvalidateRect(hwnd,NULL,1);
                UpdateWindow(hwnd);
                /**/
                hdc=GetDC(hwnd);
                SetRect(&rt_text,0,0,400,400);
                SetTextColor(hdc,RGB(255,0,0));
                SetBkColor(hdc,BkColor);
                sprintf(text,"AI思考中...");
                DrawText(hdc,text,-1,&rt_text,DT_CENTER);
                ReleaseDC(hwnd,hdc);
                /**/
                memset(ReverseRecord,0,4*9);
                PreviousMove=NextMove((who_play==1)?1:-1);
                PaintMyTable(hwnd,rt_MyTable,PreviousMove,3);
                _sleep(1000);
                Round++;
                who_play=(who_play==player1?player2:player1);
            }
            InvalidateRect(hwnd,NULL,0);
        }
        return 0;
    case WM_RBUTTONDOWN:
        pchoice.x=LOWORD(lParam)-pt_board.x+0.5*board_width;
        pchoice.y=HIWORD(lParam)-pt_board.y+0.5*board_width;
        pchoice.x=pchoice.x/board_width;//轉換為棋盤格交線位置
        pchoice.y=pchoice.y/board_width;
        return 0;
    case WM_MOUSEMOVE:
        pchoice.x=LOWORD(lParam)-pt_board.x+0.5*board_width;
        pchoice.y=HIWORD(lParam)-pt_board.y+0.5*board_width;
        pchoice.x=pchoice.x/board_width;//轉換為棋盤格交線位置
        pchoice.y=pchoice.y/board_width;
        if((pchoice.x<0||pchoice.x>7)||(pchoice.y<0||pchoice.y>7))
            return 0;
        SetRect(&rt_text,400,140,500,180);
        hdc=GetDC(hwnd);
        SetBkColor(hdc,BkColor);
        wsprintf(text," ( %2d , %2d ) ",pchoice.x+1,pchoice.y+1);
        DrawText(hdc,text,-1,&rt_text,DT_CENTER);
        ReleaseDC(hwnd,hdc);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_COMMAND:
        hMenu=GetMenu(hwnd);
        switch(LOWORD(wParam))
        {
        case IDC_RESET:
            EnableMenuItem(hMenu,IDC_RESET,MF_GRAYED);
            EnableMenuItem(hMenu,IDC_SINGLE,MF_ENABLED);
            EnableMenuItem(hMenu,IDC_RESET,MF_ENABLED);
            EnableMenuItem(hMenu,IDC_MULTI,MF_ENABLED);
            InvalidateRect(hwnd,NULL,TRUE);
            ResetGame();
            break;
        case IDC_ABORT:
            who_play=(who_play==player1?player2:player1);
            NextMove((who_play==1)?1:-1);
            who_play=(who_play==player1?player2:player1);
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
        }
        return 0;
    }
    return DefWindowProc(hwnd,msg,wParam,lParam);
}

void PaintBoard(HDC hdc,RECT rt,POINT pt)//繪製棋盤
{
    HBRUSH hbr;
    RECT rt2;
    int i;
    TCHAR *szText[5]= {"黑子先手","現在為","滑鼠位置",
                       "勝利場數","本機IP"
                      };
    hbr=CreateSolidBrush(RGB(0,150,0));//定義棋盤使用畫刷顏色
    FillRect(hdc,&rt,hbr);//繪製棋盤
    DeleteObject(hbr);
    hbr=CreateSolidBrush(BkColor);
    SelectObject(hdc,hbr);
    SetBkColor(hdc,BkColor);
    SetTextColor(hdc,RGB(0,0,0));
    SetRect(&rt2,350,10,440,30);
    SetTextColor(hdc,RGB(255,255,255));
    for(i=1; i<3; i++)
    {
        SetRect(&rt2,400,70*i-30,500,40+70*i);
        DrawText(hdc,szText[i],-1,&rt2,DT_CENTER);
    }
    DeleteObject(hbr);
    hbr=CreateSolidBrush(RGB(0,0,0));//定義棋盤使用畫刷顏色
    SelectObject(hdc,hbr);

    DeleteObject(hbr);
    for(i=0; i<8; i++) //畫橫線
    {
        MoveToEx(hdc,pt.x,pt.y+board_width*i,NULL);
        LineTo(hdc,board_width*7+pt.x,pt.y+board_width*i);
    }
    for(i=0; i<8; i++) //畫縱線
    {
        MoveToEx(hdc,pt.x+board_width*i,pt.y,NULL);
        LineTo(hdc,pt.x+board_width*i,board_width*7+pt.y);
    }
}

void PaintMyTable(HWND hwnd,RECT rt,Move pt,int who)//畫棋子
{
    HDC hdc;
    HBRUSH hbr;
    hdc=GetDC(hwnd);
    RECT rt_text;
    HPEN hPen;
    if(who==player1)
    {
        hbr=CreateSolidBrush(RGB(255,255,255));//黑棋
        SelectObject(hdc,hbr);
    }
    else if (who==player2)
    {
        hbr=CreateSolidBrush(RGB(0,0,0));//白棋
        SelectObject(hdc,hbr);
    }
    else if (who==3)
    {
        hPen = CreatePen(PS_DASH, 2, RGB(255, 0, 0));
        SelectObject(hdc,hPen);
    }
    Ellipse(hdc,rt.left+board_width*pt.x,rt.top+board_width*pt.y,
            rt.right+board_width*pt.x,rt.bottom+board_width*pt.y);
    DeleteObject(hbr);
    DeleteObject(hPen);
    ReleaseDC(hwnd,hdc);
}

void ResetGame()	//初始化遊戲資料
{
    int i,j;
    who_play=player2;

    for(i=0; i<8; i++)
        for(j=0; j<8; j++)
            MyTable[i][j]=0;//陣列設為0，表示尚未放置棋子

    MyTable[3][3]=1;
    MyTable[3][4]=-1;
    MyTable[4][3]=-1;
    MyTable[4][4]=1;
}
