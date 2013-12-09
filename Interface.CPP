#include <windows.h>
#include <winable.h>
#include <stdio.h>
#include "resource.h"

#define player1 1	//白
#define player2 -1  //黑
#define BkColor RGB(0,150,250)	//定義棋盤顏色
#define board_width 50		//棋盤格寬度

//-----------------------------------------------------------
struct Move		//定義結構儲存棋子的位置
{
    int x;
    int y;
    int Score;
};

double Benchmark_Test();
extern int Playable(int Y,int X,int Player,unsigned char* ReverseRecord,char MyTestTable[8][8]);
extern int Reverse(int Y,int X,int Player,unsigned char ReverseRecord,char MyTestTable[8][8]);
extern int GetBlackNumber();
extern int GetWhiteNumber();
extern Move NextMove(int Side);
//-----------------------------------------------------------
extern int CC;
extern char MyTable[8][8];
//-----------------------------------------------------------
int who_play;			//判斷現在誰玩
int  Round;
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
        MessageBox(NULL,TEXT("fail to initialize"),AppName,MB_ICONERROR);
        return 0;
    }
    hwnd=CreateWindowEx(WS_EX_TOPMOST,AppName,"Reversi v0.3",
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
    unsigned char ReverseRecord=0;
    Move PreviousMove;
    static int BM=0;
    switch(msg)
    {
    case WM_CREATE:
        ResetGame();
        Round=0;
        who_play=player2;
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
            //SetTextColor(hdc,RGB(0,0,0));
            SetBkColor(hdc,BkColor);
            DrawText(hdc,"Black",-1,&rt_text,DT_CENTER);
        }
        else
        {
            SetRect(&rt_text,400,70,500,110);
            //SetTextColor(hdc,RGB(255,255,255));
            SetBkColor(hdc,BkColor);
            DrawText(hdc,"White",-1,&rt_text,DT_CENTER);
        }
        SetRect(&rt_text,400,200,500,240);
        //SetTextColor(hdc,RGB(255,255,255));
        SetBkColor(hdc,BkColor);
        sprintf(text,"White:%d",GetWhiteNumber());
        DrawText(hdc,text,-1,&rt_text,DT_CENTER);
        SetRect(&rt_text,400,250,500,290);
        //SetTextColor(hdc,RGB(0,0,0));
        sprintf(text,"Black:%d",GetBlackNumber());
        DrawText(hdc,text,-1,&rt_text,DT_CENTER);
        EndPaint(hwnd,&ps);
        return 0;
    case WM_LBUTTONDOWN:

        pchoice.x=LOWORD(lParam)-pt_board.x+0.5*board_width;
        pchoice.y=HIWORD(lParam)-pt_board.y+0.5*board_width;
        pchoice.x=pchoice.x/board_width;//轉換為棋盤格交線位置
        pchoice.y=pchoice.y/board_width;
        if(MyTable[pchoice.y][pchoice.x]==0&&(pchoice.x>=0&&pchoice.x<=7)&&(pchoice.y>=0&&pchoice.y<=7))
        {

            if (Playable(pchoice.y,pchoice.x,who_play,&ReverseRecord,MyTable)!=0)
            {
                CC=0;
                PaintMyTable(hwnd,rt_MyTable,pchoice,who_play);//畫棋子
                MyTable[pchoice.y][pchoice.x]=who_play;
                Reverse(pchoice.y,pchoice.x,who_play,ReverseRecord,MyTable);
                Round++;
                who_play=(who_play==player1?player2:player1);
                InvalidateRect(hwnd,NULL,1);
                UpdateWindow(hwnd);
                hdc=GetDC(hwnd);
                SetRect(&rt_text,0,0,400,400);
                SetTextColor(hdc,RGB(255,0,0));
                SetBkColor(hdc,BkColor);
                sprintf(text,"AI is thinking...");
                DrawText(hdc,text,-1,&rt_text,DT_CENTER);
                ReleaseDC(hwnd,hdc);
                ReverseRecord=0;
                PreviousMove=NextMove((who_play==1)?1:-1);
                PaintMyTable(hwnd,rt_MyTable,PreviousMove,3);
                //_sleep(1000);
                Round++;
                who_play=(who_play==player1?player2:player1);
                printf("%d %d\n",Round,CC);
                InvalidateRect(hwnd,NULL,0);
                UpdateWindow(hwnd);
                who_play=(who_play==player1?player2:player1);
                PaintMyTable(hwnd,rt_MyTable,PreviousMove,3);
                who_play=(who_play==player1?player2:player1);
            }
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
            InvalidateRect(hwnd,NULL,TRUE);
            ResetGame();
            Round=0;
            printf("=============\n");
            break;
        case IDC_ABORT:
            who_play=(who_play==player1?player2:player1);
            PreviousMove=NextMove((who_play==1)?1:-1);
            who_play=(who_play==player1?player2:player1);
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            who_play=(who_play==player1?player2:player1);
            PaintMyTable(hwnd,rt_MyTable,PreviousMove,3);
            who_play=(who_play==player1?player2:player1);
            break;
        case IDC_BENCHMARK:
            LARGE_INTEGER t1, t2, ts;
            RECT rt1;
            EnableWindow(hwnd,0);
            ResetGame();
            QueryPerformanceFrequency(&ts);
            QueryPerformanceCounter(&t1);
            for (i=0,j=1; i<64; i++,j*=-1)
            {
                PreviousMove=NextMove(j);
                InvalidateRect(hwnd,NULL,1);
                UpdateWindow(hwnd);
            }
            QueryPerformanceCounter(&t2);
            EnableWindow(hwnd,1);
            sprintf(text,"Time: %.2lf s",(t2.QuadPart-t1.QuadPart)/(double)(ts.QuadPart));
            MessageBox(hwnd,text,"Test Result",MB_OK);
            break;
        case IDC_WHITE:
            ResetGame();
            who_play=player1;
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            break;
        case IDC_BLACK:
            ResetGame();
            who_play=player2;
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            break;
        case IDC_VISION:
            ShellExecute(NULL,"open","ReadMe.txt",NULL,NULL,SW_SHOW);
            MessageBox(hwnd,TEXT("Author: Jia-Sen Wu(a.k.a Netsphere)\nE-mail:netsphere@xuite.net\n\n"),TEXT("Reversi v0.3"),MB_OK);
            break;
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
    TCHAR *szText[]= {"Color","Player","Location"};
    hbr=CreateSolidBrush(RGB(0,150,0));//定義棋盤使用畫刷顏色
    FillRect(hdc,&rt,hbr);//繪製棋盤
    DeleteObject(hbr);
    hbr=CreateSolidBrush(BkColor);
    SelectObject(hdc,hbr);
    SetBkColor(hdc,BkColor);
    SetRect(&rt2,350,10,440,30);
    SetTextColor(hdc,RGB(0,0,0));
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
    if (!((pt.x >=0 && pt.x<=7 ) && (pt.y >=0 && pt.y<=7 )))
        return;
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
        if (who_play==player1)
        {
            hbr=CreateSolidBrush(RGB(255,255,255));//黑棋
            SelectObject(hdc,hbr);
        }
        else
        {
            hbr=CreateSolidBrush(RGB(0,0,0));//白棋
            SelectObject(hdc,hbr);
        }
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
    for(i=0; i<8; i++)
        for(j=0; j<8; j++)
            MyTable[i][j]=0;//陣列設為0，表示尚未放置棋子

    MyTable[3][3]=1;
    MyTable[3][4]=-1;
    MyTable[4][3]=-1;
    MyTable[4][4]=1;
}
