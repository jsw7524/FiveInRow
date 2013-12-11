#include <windows.h>
#include <winable.h>
#include <stdio.h>
#include "resource.h"

#define PLAYER1 1	//白
#define PLAYER2 -1  //黑
#define BKCOLOR RGB(0,150,250)	//定義棋盤顏色
#define BOARD_WIDTH 50		    //棋盤格寬度

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
extern Move NextMove(int Side);
extern void  RestoreMyTable();
extern void  BackUpMyTable();
extern void ResetGame();
//-----------------------------------------------------------
extern int CC;
extern int DepthLimit;
extern char MyTable[8][8];
extern int  Round;
//-----------------------------------------------------------
int who_play;			//判斷現在誰玩

//----------------------------------------------------------
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);//視窗訊息處理程式
void PaintBoard(HDC,RECT,POINT);//繪製棋盤的函式
void PaintMyTable(HWND,RECT,Move,int);//繪製棋子的函式
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
    hwnd=CreateWindowEx(WS_EX_TOPMOST,AppName,"Reversi - 13.12.11",
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
    static HMENU hMenu;
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
        hMenu=GetMenu(hwnd);
        EnableMenuItem(hMenu,IDC_RESTORATION,MF_GRAYED);
        ResetGame();
        who_play=PLAYER2;
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
                case PLAYER1:
                    PaintMyTable(hwnd,rt_MyTable,pchoice,PLAYER1);
                    break;
                case PLAYER2:
                    PaintMyTable(hwnd,rt_MyTable,pchoice,PLAYER2);
                    break;
                }
            }
        }
        if(who_play==-1)
        {
            SetRect(&rt_text,400,70,500,110);
            //SetTextColor(hdc,RGB(0,0,0));
            SetBkColor(hdc,BKCOLOR);
            DrawText(hdc,"Black",-1,&rt_text,DT_CENTER);
        }
        else
        {
            SetRect(&rt_text,400,70,500,110);
            //SetTextColor(hdc,RGB(255,255,255));
            SetBkColor(hdc,BKCOLOR);
            DrawText(hdc,"White",-1,&rt_text,DT_CENTER);
        }
        SetRect(&rt_text,400,140,500,180);
        SetBkColor(hdc,BKCOLOR);
        switch (DepthLimit)
        {
            case 6:
                sprintf(text,"%s","Easy");
                break;
            case 8:
                sprintf(text,"%s","Standard");
                break;
            case 10:
                sprintf(text,"%s","Hard");
                break;

        }
        DrawText(hdc,text,-1,&rt_text,DT_CENTER);
        SetRect(&rt_text,400,200,500,240);
        //SetTextColor(hdc,RGB(255,255,255));
        SetBkColor(hdc,BKCOLOR);
        sprintf(text,"White:%d",GetWhiteNumber());
        DrawText(hdc,text,-1,&rt_text,DT_CENTER);
        SetRect(&rt_text,400,250,500,290);
        //SetTextColor(hdc,RGB(0,0,0));
        sprintf(text,"Black:%d",GetBlackNumber());
        DrawText(hdc,text,-1,&rt_text,DT_CENTER);
        EndPaint(hwnd,&ps);
        return 0;
    case WM_LBUTTONDOWN:

        pchoice.x=LOWORD(lParam)-pt_board.x+0.5*BOARD_WIDTH;
        pchoice.y=HIWORD(lParam)-pt_board.y+0.5*BOARD_WIDTH;
        pchoice.x=pchoice.x/BOARD_WIDTH;//轉換為棋盤格交線位置
        pchoice.y=pchoice.y/BOARD_WIDTH;
        if(MyTable[pchoice.y][pchoice.x]==0&&(pchoice.x>=0&&pchoice.x<=7)&&(pchoice.y>=0&&pchoice.y<=7))
        {
            if (Playable(pchoice.y,pchoice.x,who_play,&ReverseRecord,MyTable)!=0)
            {

                CC=0;
                BackUpMyTable();
                EnableMenuItem(hMenu,IDC_RESTORATION,MF_ENABLED);
                PaintMyTable(hwnd,rt_MyTable,pchoice,who_play);//畫棋子
                MyTable[pchoice.y][pchoice.x]=who_play;
                Reverse(pchoice.y,pchoice.x,who_play,ReverseRecord,MyTable);
                Round++;
                who_play=(who_play==PLAYER1?PLAYER2:PLAYER1);
                InvalidateRect(hwnd,NULL,1);
                UpdateWindow(hwnd);
                hdc=GetDC(hwnd);
                SetRect(&rt_text,0,0,400,400);
                SetTextColor(hdc,RGB(255,0,0));
                SetBkColor(hdc,BKCOLOR);
                sprintf(text,"AI is thinking...");
                DrawText(hdc,text,-1,&rt_text,DT_CENTER);
                ReleaseDC(hwnd,hdc);
                ReverseRecord=0;
                PreviousMove=NextMove((who_play==1)?1:-1);
                PaintMyTable(hwnd,rt_MyTable,PreviousMove,3);
                //_sleep(100);
                Round++;
                who_play=(who_play==PLAYER1?PLAYER2:PLAYER1);
                printf("%d %d\n",Round,CC);
                InvalidateRect(hwnd,NULL,0);
                UpdateWindow(hwnd);
                who_play=(who_play==PLAYER1?PLAYER2:PLAYER1);
                PaintMyTable(hwnd,rt_MyTable,PreviousMove,3);
                who_play=(who_play==PLAYER1?PLAYER2:PLAYER1);
            }
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_COMMAND:

        switch(LOWORD(wParam))
        {
        case IDC_RESET:
            InvalidateRect(hwnd,NULL,TRUE);
            ResetGame();
            EnableMenuItem(hMenu,IDC_RESTORATION,MF_GRAYED);
            printf("=============\n");
            break;
        case IDC_ABORT:
            who_play=(who_play==PLAYER1?PLAYER2:PLAYER1);
            PreviousMove=NextMove((who_play==1)?1:-1);
            who_play=(who_play==PLAYER1?PLAYER2:PLAYER1);
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            who_play=(who_play==PLAYER1?PLAYER2:PLAYER1);
            PaintMyTable(hwnd,rt_MyTable,PreviousMove,3);
            who_play=(who_play==PLAYER1?PLAYER2:PLAYER1);
            break;
        case IDC_BENCHMARK:
            LARGE_INTEGER t1, t2, ts;
            int TD;
            TD=DepthLimit;
            DepthLimit=8;
            RECT rt1;
            EnableWindow(hwnd,0);
            ResetGame();
            who_play=PLAYER1;
            QueryPerformanceFrequency(&ts);
            QueryPerformanceCounter(&t1);
            for (i=0,j=1; i<64; i++,j*=-1)
            {
                PreviousMove=NextMove(j);
                InvalidateRect(hwnd,NULL,1);
                UpdateWindow(hwnd);
            }
            QueryPerformanceCounter(&t2);
            EnableMenuItem(hMenu,IDR_MENU,MF_ENABLED);
            EnableWindow(hwnd,1);
            DepthLimit=TD;
            sprintf(text,"Time: %.2lf s",(t2.QuadPart-t1.QuadPart)/(double)(ts.QuadPart));
            MessageBox(hwnd,text,"Test Result",MB_OK);
            break;
        case IDC_LEVELEASY:
            DepthLimit=6;
            CheckMenuItem(hMenu,IDC_LEVELEASY,MF_CHECKED);
            CheckMenuItem(hMenu,IDC_LEVELSTANDARD,MF_UNCHECKED);
            CheckMenuItem(hMenu,IDC_LEVELHARD,MF_UNCHECKED);
            ResetGame();
            EnableMenuItem(hMenu,IDC_RESTORATION,MF_GRAYED);
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            break;
        case IDC_LEVELSTANDARD:
            DepthLimit=8;
            CheckMenuItem(hMenu,IDC_LEVELEASY,MF_UNCHECKED);
            CheckMenuItem(hMenu,IDC_LEVELSTANDARD,MF_CHECKED);
            CheckMenuItem(hMenu,IDC_LEVELHARD,MF_UNCHECKED);
            ResetGame();
            EnableMenuItem(hMenu,IDC_RESTORATION,MF_GRAYED);
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            break;
        case IDC_LEVELHARD:
            DepthLimit=10;
            CheckMenuItem(hMenu,IDC_LEVELEASY,MF_UNCHECKED);
            CheckMenuItem(hMenu,IDC_LEVELSTANDARD,MF_UNCHECKED);
            CheckMenuItem(hMenu,IDC_LEVELHARD,MF_CHECKED);
            ResetGame();
            EnableMenuItem(hMenu,IDC_RESTORATION,MF_GRAYED);
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            break;
        case IDC_WHITE:
            ResetGame();
            who_play=PLAYER1;
            CheckMenuItem(hMenu,IDC_BLACK,MF_UNCHECKED);
            CheckMenuItem(hMenu,IDC_WHITE,MF_CHECKED);
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            break;
        case IDC_BLACK:
            ResetGame();
            who_play=PLAYER2;
            CheckMenuItem(hMenu,IDC_BLACK,MF_CHECKED);
            CheckMenuItem(hMenu,IDC_WHITE,MF_UNCHECKED);
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            break;
        case IDC_RESTORATION:
            RestoreMyTable();
            EnableMenuItem(hMenu,IDC_RESTORATION,MF_GRAYED);
            InvalidateRect(hwnd,NULL,1);
            UpdateWindow(hwnd);
            break;
        case IDC_VISION:
            ShellExecute(NULL,"open","ReadMe.txt",NULL,NULL,SW_SHOW);
            MessageBox(hwnd,TEXT("Author: Jia-Sen Wu(a.k.a. Netsphere)\nE-mail:netsphere@xuite.net\n\n"),TEXT("Hello!"),MB_OK);
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
    TCHAR *szText[]= {"Color","Player","Level"};
    hbr=CreateSolidBrush(RGB(0,150,0));//定義棋盤使用畫刷顏色
    FillRect(hdc,&rt,hbr);//繪製棋盤
    DeleteObject(hbr);
    hbr=CreateSolidBrush(BKCOLOR);
    SelectObject(hdc,hbr);
    SetBkColor(hdc,BKCOLOR);
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
        MoveToEx(hdc,pt.x,pt.y+BOARD_WIDTH*i,NULL);
        LineTo(hdc,BOARD_WIDTH*7+pt.x,pt.y+BOARD_WIDTH*i);
    }
    for(i=0; i<8; i++) //畫縱線
    {
        MoveToEx(hdc,pt.x+BOARD_WIDTH*i,pt.y,NULL);
        LineTo(hdc,pt.x+BOARD_WIDTH*i,BOARD_WIDTH*7+pt.y);
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
    else if (who==3)
    {
        if (who_play==PLAYER1)
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
    Ellipse(hdc,rt.left+BOARD_WIDTH*pt.x,rt.top+BOARD_WIDTH*pt.y,
            rt.right+BOARD_WIDTH*pt.x,rt.bottom+BOARD_WIDTH*pt.y);
    DeleteObject(hbr);
    DeleteObject(hPen);
    ReleaseDC(hwnd,hdc);
}


