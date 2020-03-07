#include <windows.h>
//#include <winable.h>
#include <stdio.h>
#include "resource.h"

#define PLAYER1 1	//白
#define PLAYER2 -1  //黑
#define BKCOLOR RGB(238,221,130)	//定義棋盤顏色
#define BOARD_WIDTH 40		    //棋盤格寬度
#define BOARD_SIZE 15
//-----------------------------------------------------------
struct Move		                //定義結構儲存棋子的位置
{
    int x;
    int y;
    int Score;
};

extern int Playable(int Y,int X,int Player,unsigned char* ReverseRecord,char MyTestTable[8][8]);
extern int Reverse(int Y,int X,int Player,unsigned char ReverseRecord,char MyTestTable[8][8]);
extern int GetBlackNumber();
extern int GetWhiteNumber();
extern void  RestoreMyTable();
extern void  BackUpMyTable();
extern void ResetGame();
extern int referee(int X,int Y,int C);
extern void LoadWeightTable();
extern int Evaluate(int X,int Y,int C,char TestBoard[15][15]);
extern int PossibleMap(int X,int Y,char MapBoard[15][15]);
extern void Marker(int X,int Y,char MarkBoard[15][15]);
extern Move NextMove(int C);
extern void MakeZTable();

//-----------------------------------------------------------
extern unsigned long long CurrentZobrist;
extern unsigned long long ZobristTable[2][255];
extern int DepthLimit;
extern char MyTable[15][15];
extern int  Round;
extern char MyMap[15][15];
extern int Order[15][15];
extern char MarkTable[15][15];
extern int Scores[15][15];
//-----------------------------------------------------------
int who_play;			//判斷現在誰玩

//----------------------------------------------------------
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);//視窗訊息處理程式
void PaintBoard(HDC,RECT,POINT);//繪製棋盤的函式
void PaintPiece(HWND,RECT,Move,int);//繪製棋子的函式
void PaintCircle(HWND hwnd,RECT rt,Move pt,int who);
void PaintINFO(HWND hwnd,RECT rt,Move pt,int who,int INFO[15][15]);
//----------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
    static TCHAR AppName[]=TEXT("Gomoku");
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
    hwnd=CreateWindowEx(WS_EX_TOPMOST,AppName,"JSW's Gomoku - Ruby&Waku",
                        WS_CAPTION|WS_BORDER|WS_SYSMENU|WS_MINIMIZEBOX,
                        0,0,610,660,
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
    static HMENU hMenu;
    static RECT rt_board,//儲存棋盤大小的變數
           rt_MyPiece;//儲存棋子大小的變數
    static POINT pt_board;//儲存棋盤格線初始座標的變數
    Move pchoice,ComputerMove;
    int check_win,i,j,w,b;
    TCHAR text[20];
    RECT rt_text;
    unsigned char ReverseRecord=0;
    Move PreviousMove;
    int B=0;
    switch(msg)
    {
    case WM_CREATE:
        MakeZTable();
        CurrentZobrist=0;
        LoadWeightTable();
        hMenu=GetMenu(hwnd);
        ResetGame();
        who_play=PLAYER2;
        SetRect(&rt_board,0,0,800,800);//設定棋盤大小
        SetRect(&rt_MyPiece,0,0,BOARD_WIDTH,BOARD_WIDTH);
        pt_board.x=BOARD_WIDTH/2;//設定第一調棋盤格線的起始座標位置
        pt_board.y=BOARD_WIDTH/2;
        CheckMenuItem(hMenu,IDC_BLACK,MF_CHECKED);
        CheckMenuItem(hMenu,IDC_WHITE,MF_UNCHECKED);
        break;
    case WM_PAINT:
        hdc=BeginPaint(hwnd,&ps);
        PaintBoard(hdc,rt_board,pt_board);//繪製棋盤
        for(i=0; i<BOARD_SIZE; i++)
        {
            for(j=0; j<BOARD_SIZE; j++)
            {
                pchoice.x=j;
                pchoice.y=i;
                switch(MyTable[i][j])
                {
                case PLAYER1:
                    PaintPiece(hwnd,rt_MyPiece,pchoice,PLAYER1);
                    break;
                case PLAYER2:
                    PaintPiece(hwnd,rt_MyPiece,pchoice,PLAYER2);
                    break;
                }
            }
        }
        EndPaint(hwnd,&ps);
        break;
    case WM_RBUTTONDOWN:
        /**/
        pchoice.x=LOWORD(lParam)-pt_board.x+0.5*BOARD_WIDTH;
        pchoice.y=HIWORD(lParam)-pt_board.y+0.5*BOARD_WIDTH;
        pchoice.x=pchoice.x/BOARD_WIDTH;//轉換為棋盤格交線位置
        pchoice.y=pchoice.y/BOARD_WIDTH;
        if(MyTable[pchoice.y][pchoice.x]==0&&(pchoice.x>=0&&pchoice.x<=(BOARD_SIZE-1))&&(pchoice.y>=0&&pchoice.y<=(BOARD_SIZE-1)))
        {
            PaintPiece(hwnd,rt_MyPiece,pchoice,who_play);//畫棋子
            MyTable[pchoice.y][pchoice.x]=who_play;
            Round++;
            Order[pchoice.y][pchoice.x]=Round;
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            PaintCircle(hwnd,rt_MyPiece,pchoice,who_play);
            CurrentZobrist=CurrentZobrist^ZobristTable[(who_play==1)?1:0][pchoice.y*16+pchoice.x];
            who_play=(who_play==PLAYER1?PLAYER2:PLAYER1);
        }
        printf("%lld\n",CurrentZobrist);

        break;
    case WM_LBUTTONDOWN:
        pchoice.x=LOWORD(lParam)-pt_board.x+0.5*BOARD_WIDTH;
        pchoice.y=HIWORD(lParam)-pt_board.y+0.5*BOARD_WIDTH;
        pchoice.x=pchoice.x/BOARD_WIDTH;//轉換為棋盤格交線位置
        pchoice.y=pchoice.y/BOARD_WIDTH;
        if(MyTable[pchoice.y][pchoice.x]==0&&(pchoice.x>=0&&pchoice.x<=(BOARD_SIZE-1))&&(pchoice.y>=0&&pchoice.y<=(BOARD_SIZE-1)))
        {
            PaintPiece(hwnd,rt_MyPiece,pchoice,who_play);//畫棋子
            CurrentZobrist=CurrentZobrist^ZobristTable[(who_play==1)?1:0][pchoice.y*16+pchoice.x];
            MyTable[pchoice.y][pchoice.x]=who_play;
            Round++;
            Order[pchoice.y][pchoice.x]=Round;
            if (referee(pchoice.x,pchoice.y,who_play))
            {
                CurrentZobrist=0;
                SendMessage(hwnd,WM_COMMAND,(WPARAM)IDC_ORDER,NULL);
                MessageBox(hwnd,"You win.","Game over",0);
                SendMessage(hwnd,WM_CREATE,NULL,NULL);
                who_play=PLAYER2;
                InvalidateRect(hwnd,NULL,1);
                UpdateWindow(hwnd);
                return 0;
            }
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            PaintCircle(hwnd,rt_MyPiece,pchoice,who_play);
            who_play=(who_play==PLAYER1?PLAYER2:PLAYER1);

            ///////////////AI////////////////////
            hdc=GetDC(hwnd);
            SetRect(&rt_text,0,0,400,600);
            SetTextColor(hdc,RGB(255,0,0));
            SetBkColor(hdc,BKCOLOR);
            sprintf(text,"Please wait, AI is thinking...");
            DrawText(hdc,text,-1,&rt_text,DT_CENTER);
            ReleaseDC(hwnd,hdc);

            if (Round==1)
            {
                ComputerMove=pchoice;
                if (ComputerMove.x==14)
                    ComputerMove.x-=1;
                else
                    ComputerMove.x+=1;
            }
            else
            {
                ComputerMove=NextMove(who_play);
            }


            MyTable[ComputerMove.y][ComputerMove.x]=who_play;
            CurrentZobrist=CurrentZobrist^ZobristTable[(who_play==1)?1:0][pchoice.y*16+pchoice.x];
            PaintPiece(hwnd,rt_MyPiece,ComputerMove,who_play);
            Round++;
            Order[ComputerMove.y][ComputerMove.x]=Round;
            if (referee(ComputerMove.x,ComputerMove.y,who_play))
            {
                CurrentZobrist=0;
                PaintCircle(hwnd,rt_MyPiece,ComputerMove,who_play);
                SendMessage(hwnd,WM_COMMAND,(WPARAM)IDC_ORDER,NULL);
                MessageBox(hwnd,"Computer wins.","Game over",0);
                SendMessage(hwnd,WM_CREATE,NULL,NULL);
                InvalidateRect(hwnd,NULL,1);
                UpdateWindow(hwnd);
                return 0;
            }
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            PaintCircle(hwnd,rt_MyPiece,ComputerMove,who_play);
            who_play=(who_play==PLAYER1?PLAYER2:PLAYER1);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_COMMAND:

        switch(LOWORD(wParam))
        {
        case IDC_WHITE:
            ResetGame();
            who_play=PLAYER1;
            MyTable[7][7]=PLAYER2;
            InvalidateRect(hwnd,NULL,1);
            Round=1;
            UpdateWindow(hwnd);
            SendMessage(hwnd,WM_PAINT,NULL,NULL);
            CheckMenuItem(hMenu,IDC_BLACK,MF_UNCHECKED);
            CheckMenuItem(hMenu,IDC_WHITE,MF_CHECKED);
            break;
        case IDC_BLACK:
            ResetGame();
            who_play=PLAYER2;
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            SendMessage(hwnd,WM_PAINT,NULL,NULL);
            CheckMenuItem(hMenu,IDC_BLACK,MF_CHECKED);
            CheckMenuItem(hMenu,IDC_WHITE,MF_UNCHECKED);
            break;
        case IDC_ORDER:
            for (i=0; i<15; i++)
            {
                for (j=0; j<15; j++)
                {
                    if (MyTable[j][i]!=0)
                    {
                        pchoice.x=i;
                        pchoice.y=j;
                        PaintINFO(hwnd,rt_MyPiece,pchoice,MyTable[j][i],Order);
                    }
                }
            }
            break;
        case IDC_SCORE:
            for (i=0; i<15; i++)
            {
                for (j=0; j<15; j++)
                {
                    if (MyTable[j][i]==0)
                    {
                        pchoice.x=i;
                        pchoice.y=j;
                        Scores[j][i]=0;
                        MyTable[j][i]=who_play;
                        Scores[j][i]=abs(Evaluate(i,j,who_play,MyTable));
                        MyTable[j][i]=(who_play==PLAYER1?PLAYER2:PLAYER1);
                        Scores[j][i]+=abs(Evaluate(i,j,(who_play==PLAYER1?PLAYER2:PLAYER1),MyTable));
                        MyTable[j][i]=0;
                        PaintINFO(hwnd,rt_MyPiece,pchoice,MyTable[j][i],Scores);
                    }
                }
            }
            break;
        case IDC_BENCHMARK:
            LARGE_INTEGER t1, t2, ts;
            QueryPerformanceFrequency(&ts);
            QueryPerformanceCounter(&t1);
            TCHAR text[20];
            SendMessage(hwnd,WM_COMMAND,(WPARAM)IDC_WHITE,NULL);
            for (int I=0,Round=1; I<30; I++)
            {
                ComputerMove=NextMove(who_play);
                MyTable[ComputerMove.y][ComputerMove.x]=who_play;
                CurrentZobrist=CurrentZobrist^ZobristTable[(who_play==1)?1:0][pchoice.y*16+pchoice.x];
                PaintPiece(hwnd,rt_MyPiece,ComputerMove,who_play);
                Round++;
                Order[ComputerMove.y][ComputerMove.x]=Round;
                if (referee(ComputerMove.x,ComputerMove.y,who_play))
                {
                    QueryPerformanceCounter(&t2);
                    CurrentZobrist=0;
                    PaintCircle(hwnd,rt_MyPiece,ComputerMove,who_play);
                    SendMessage(hwnd,WM_COMMAND,(WPARAM)IDC_ORDER,NULL);
                    sprintf(text,"Time: %.2lf s",(t2.QuadPart-t1.QuadPart)/(double)(ts.QuadPart));
                    MessageBox(hwnd,text,"Test Result",MB_OK);
                    SendMessage(hwnd,WM_CREATE,NULL,NULL);
                    InvalidateRect(hwnd,NULL,1);
                    UpdateWindow(hwnd);

                    return 0;
                }
                InvalidateRect(hwnd,NULL,1);
                UpdateWindow(hwnd);
                PaintCircle(hwnd,rt_MyPiece,ComputerMove,who_play);
                who_play=(who_play==PLAYER1?PLAYER2:PLAYER1);
            }
            QueryPerformanceCounter(&t2);
            sprintf(text,"Time: %.2lf s",(t2.QuadPart-t1.QuadPart)/(double)(ts.QuadPart));
            MessageBox(hwnd,text,"Test Result",MB_OK);
            break;
        case IDC_VISION:
            //ShellExecute(NULL,"open","ReadMe.txt",NULL,NULL,SW_SHOW);
            MessageBox(hwnd,TEXT("Author: Netsphere \nE-mail: jsw7524@gmail.com\n\n"),TEXT("Hello!"),MB_OK);
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
    int i=3;

    hbr=CreateSolidBrush(BKCOLOR);//定義棋盤使用畫刷顏色
    FillRect(hdc,&rt,hbr);//繪製棋盤
    DeleteObject(hbr);
    hbr=CreateSolidBrush(BLACK_BRUSH);
    SelectObject(hdc,hbr);
    Ellipse(hdc,BOARD_WIDTH*7+pt.x-i,BOARD_WIDTH*7+pt.y-i,BOARD_WIDTH*7+i+pt.x,BOARD_WIDTH*7+i+pt.y);
    Ellipse(hdc,BOARD_WIDTH*3+pt.x-i,BOARD_WIDTH*11+pt.y-i,BOARD_WIDTH*3+i+pt.x,BOARD_WIDTH*11+i+pt.y);
    Ellipse(hdc,BOARD_WIDTH*3+pt.x-i,BOARD_WIDTH*3+pt.y-i,BOARD_WIDTH*3+i+pt.x,BOARD_WIDTH*3+i+pt.y);
    Ellipse(hdc,BOARD_WIDTH*11+pt.x-i,BOARD_WIDTH*11+pt.y-i,BOARD_WIDTH*11+i+pt.x,BOARD_WIDTH*11+i+pt.y);
    Ellipse(hdc,BOARD_WIDTH*11+pt.x-i,BOARD_WIDTH*3+pt.y-i,BOARD_WIDTH*11+i+pt.x,BOARD_WIDTH*3+i+pt.y);
    DeleteObject(hbr);
    for(i=0; i<BOARD_SIZE; i++) //畫橫線
    {
        MoveToEx(hdc,pt.x,pt.y+BOARD_WIDTH*i,NULL);
        LineTo(hdc,BOARD_WIDTH*(BOARD_SIZE-1)+pt.x,pt.y+BOARD_WIDTH*i);
    }
    for(i=0; i<BOARD_SIZE; i++) //畫縱線
    {
        MoveToEx(hdc,pt.x+BOARD_WIDTH*i,pt.y,NULL);
        LineTo(hdc,pt.x+BOARD_WIDTH*i,BOARD_WIDTH*(BOARD_SIZE-1)+pt.y);
    }
}

void PaintPiece(HWND hwnd,RECT rt,Move pt,int who)//畫棋子
{
    HDC hdc;
    HBRUSH hbr;
    hdc=GetDC(hwnd);
    RECT rt_text;
    HPEN hPen;
    char text[20];
    /*
    if (!((pt.x >=0 && pt.x<=(BOARD_SIZE-1) ) && (pt.y >=0 && pt.y<=(BOARD_SIZE-1) )))
        return;
    */
    if(who==PLAYER1)
    {
        hbr=CreateSolidBrush(RGB(255,255,255));//黑棋
        SelectObject(hdc,hbr);
    }
    else if (who==PLAYER2)
    {
        hbr=CreateSolidBrush(RGB(0,0,0));//白棋
        SelectObject(hdc,hbr);
    }

    Ellipse(hdc,rt.left+BOARD_WIDTH*pt.x,rt.top+BOARD_WIDTH*pt.y,
            rt.right+BOARD_WIDTH*pt.x,rt.bottom+BOARD_WIDTH*pt.y);

    DeleteObject(hbr);
    DeleteObject(hPen);
    ReleaseDC(hwnd,hdc);

}


void PaintINFO(HWND hwnd,RECT rt,Move pt,int who,int INFO[15][15])
{
    HDC hdc;
    HBRUSH hbr;
    hdc=GetDC(hwnd);
    RECT rt_text;
    HPEN hPen;

    COLORREF OriginBK,OriginTX;

    char text[20];
    /*
    if (!((pt.x >=0 && pt.x<=(BOARD_SIZE-1) ) && (pt.y >=0 && pt.y<=(BOARD_SIZE-1) )))
        return;

    if(who==PLAYER1)
    {
        hbr=CreateSolidBrush(RGB(255,255,255));//黑棋
        SelectObject(hdc,hbr);
        OriginBK=SetBkColor(hdc,RGB(255,255,255));
        OriginTX=SetTextColor(hdc,RGB(0,255,0));
    }
    else if (who==PLAYER2)
    {
        hbr=CreateSolidBrush(RGB(0,0,0));//白棋
        SelectObject(hdc,hbr);
        OriginBK=SetBkColor(hdc,RGB(0,0,0));
        OriginTX=SetTextColor(hdc,RGB(0,255,0));
    }

    Ellipse(hdc,rt.left+BOARD_WIDTH*pt.x,rt.top+BOARD_WIDTH*pt.y,
            rt.right+BOARD_WIDTH*pt.x,rt.bottom+BOARD_WIDTH*pt.y);
    */
    sprintf(text,"%d",INFO[pt.y][pt.x]);
    TextOut(hdc,6+rt.left+BOARD_WIDTH*pt.x,5+rt.top+BOARD_WIDTH*pt.y,text,strlen(text));
    SetBkColor(hdc,OriginBK);
    SetTextColor(hdc,OriginTX);
    DeleteObject(hbr);
    //DeleteObject(hPen);
    ReleaseDC(hwnd,hdc);

}

void PaintCircle(HWND hwnd,RECT rt,Move pt,int who)
{
    HDC hdc;
    HBRUSH hbr=NULL;
    hdc=GetDC(hwnd);
    RECT rt_text;
    HPEN hPen;
    if(who==PLAYER1)
    {
        hbr=CreateSolidBrush(RGB(255,255,255));//黑棋
        SelectObject(hdc,hbr);
    }
    else if (who==PLAYER2)
    {
        hbr=CreateSolidBrush(RGB(0,0,0));//白棋
        SelectObject(hdc,hbr);
    }
    hPen = CreatePen(PS_DOT, 2, RGB(255, 0, 0));
    SelectObject(hdc,hPen);
    Ellipse(hdc,rt.left+BOARD_WIDTH*pt.x,rt.top+BOARD_WIDTH*pt.y,
            rt.right+BOARD_WIDTH*pt.x,rt.bottom+BOARD_WIDTH*pt.y);
    DeleteObject(hbr);
    DeleteObject(hPen);
    ReleaseDC(hwnd,hdc);
}




