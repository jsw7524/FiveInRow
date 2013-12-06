#include <windows.h>
#include <stdio.h>

int CC;
extern char MyTable[8][8];
extern int Playable(int Y,int X,int Player,unsigned char *ReverseRecord,char MyTestTable[8][8]);
extern int Reverse(int Y,int X,int Player,unsigned char ReverseRecord,char MyTestTable[8][8]);
int MinMax(char TestBoard[8][8], int Side, int Depth,int Alpha,int Beta);

int PriorityMap[8][8]=
{
    128,0,8,8,8,8,0,128,
    0,-32,1,1,1,1,-32,0,
    8,1,1,1,1,1,1,8,
    8,1,1,1,1,1,1,8,
    8,1,1,1,1,1,1,8,
    8,1,1,1,1,1,1,8,
    0,-32,1,1,1,1,-32,0,
    128,0,8,8,8,8,0,128,
};

struct Move
{
    int X;
    int Y;
    int Score;
};

int CMP_Max(const void* A,const void* B)
{
    return ((Move*)B)->Score-((Move*)A)->Score;
}
int CMP_Min(const void* A,const void* B)
{
    return ((Move*)A)->Score-((Move*)B)->Score;
}
int Referee(char TestBoard[8][8])
{
    CC++;
    int I,J,K;
    K=0;
    for (I=0; I<8; I++)
    {
        for (J=0; J<8; J++)
        {
            K+=TestBoard[I][J]*PriorityMap[I][J];
        }
    }
    return K;
}

Move NextMove(int Side)
{
    int I,J,K,L=0;
    unsigned char RR;
    char NextBoard[8][8];
    Move MyMove[64];
    int Index=0;
    for (I=0; I<8; I++)
    {
        for (J=0; J<8; J++)
        {
            if (MyTable[I][J]==0)
            {
                RR=0;
                if (0!=Playable(I,J,Side,&RR,MyTable))
                {
                    L++;
                    MyMove[Index].X=J;
                    MyMove[Index].Y=I;
                    MyMove[Index].Score=PriorityMap[I][J];
                    Index++;
                }
            }
        }
    }
    qsort(MyMove,Index,sizeof(Move),CMP_Max);
    for (I=0,L=0; I<Index; I++)
    {

        L++;
        Playable(MyMove[I].Y,MyMove[I].X,Side,&RR,MyTable);
        memcpy(NextBoard,MyTable,8*8);
        NextBoard[MyMove[I].Y][MyMove[I].X]=Side;
        Reverse(MyMove[I].Y,MyMove[I].X,Side,RR,NextBoard);
        MyMove[I].Score=MinMax(NextBoard,(Side==1?-1:1),0,-99999,99999);

    }
    if (L==0)
        return {-1,-1,0};
    qsort(MyMove,Index,sizeof(Move),(Side==1)?CMP_Max:CMP_Min);
    MyTable[MyMove[0].Y][MyMove[0].X]=Side;
    Playable(MyMove[0].Y,MyMove[0].X,Side,&RR,MyTable);
    Reverse(MyMove[0].Y,MyMove[0].X,Side,RR,MyTable);
    return MyMove[0];
}

int MinMax(char TestBoard[8][8], int Side, int Depth,int Alpha,int Beta)
{
    int I,J,K,L=0;
    unsigned char RR=0;
    char NextBoard[8][8];
    int Max=-99999,Min=99999;
    Move MyMove[64];
    int Index=0;
    if (Depth < 8)
    {
        for (I=0; I<8; I++)
        {
            for (J=0; J<8; J++)
            {
                if (TestBoard[I][J]==0)
                {
                    RR=0;
                    if (0!=Playable(I,J,Side,&RR,TestBoard))
                    {
                        L++;
                        MyMove[Index].X=J;
                        MyMove[Index].Y=I;
                        MyMove[Index].Score=PriorityMap[I][J];
                        Index++;
                    }
                }
            }
        }
        qsort(MyMove,Index,sizeof(Move),CMP_Max);
        for (I=0,L=0; I<Index; I++)
        {
            L++;
            RR=0;
            Playable(MyMove[I].Y,MyMove[I].X,Side,&RR,TestBoard);
            memcpy(NextBoard,TestBoard,8*8);
            NextBoard[MyMove[I].Y][MyMove[I].X]=Side;
            Reverse(MyMove[I].Y,MyMove[I].X,Side,RR,NextBoard);
            K=MinMax(NextBoard,(Side==1?-1:1),Depth+1,Alpha,Beta);
            if (Side==1)
            {
                if (K>Max)
                    Max=K;

                if (Alpha<Max)
                {
                    Alpha=Max;
                }
            }
            else
            {
                if (K<Min)
                    Min=K;

                if (Beta>Min)
                {
                    Beta=Min;
                }
            }

            if (Alpha>Beta)
                return (Side==1?Max:Min);

        }
        if (L==0)
            return MinMax(TestBoard,(Side==1?-1:1),Depth+1,Alpha,Beta);
        else
            return (Side==1?Max:Min);
    }
    else
    {
        return Referee(TestBoard);
    }
}

void ShowBoard()
{
    int I,J,K;
    printf("\n");
    for (I=0; I<8; I++)
    {
        for (J=0; J<8; J++)
        {
            printf("%2d",MyTable[I][J]);
        }
        printf("\n");
    }
}
