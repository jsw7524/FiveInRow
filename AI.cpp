#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Move
{
    int X;
    int Y;
    int Score;
};

struct ZT
{
    int Value;
    unsigned long long Hash;
};

extern char MyTable[15][15];
extern int who_play;

unsigned long long ZobristTable[2][255];
unsigned long long CurrentZobrist=0;
ZT HashTable[10000000]; //999983

int SSS;

int MinMax(char TestBoard[15][15], int Side, int Depth,int Alpha,int Beta, unsigned long long Zobrist);


int WeightTable[3][3][3][3][2][3][3][3][3];
FILE *Data;
int DepthLimit=6;
char MarkTable[15][15];

int CMP_Max(const void* A,const void* B)
{
    return ((Move*)B)->Score-((Move*)A)->Score;
}

int CMP_Min(const void* A,const void* B)
{
    return ((Move*)A)->Score-((Move*)B)->Score;
}

void MakeZTable()
{
    srand(7524);
    int I,J,K;
    unsigned long long L,R,S;
    for (I=0; I<2; I++)
    {
        for (J=0; J<255; J++)
        {
            L=(rand()<<15)+rand();
            R=(rand()<<15)+rand();
            S=(rand()<<60);
            ZobristTable[I][J]=S|(R<<30)|L;
        }
    }
    return;
}

void LoadWeightTable()
{
    int I,J,K;
    char Pattern[10];
    int Index[9];
    int Weight;
    Data=fopen("JS.dll","r");
    while(EOF!=fscanf(Data,"%s %d",Pattern,&Weight))
    {
        for (I=0; I<4; I++)
        {
            Index[I]=0;
            if (Pattern[I]!='_')
            {
                Index[I]=(Pattern[I]=='w')?1:2;
            }
        }
        Index[I]=(Pattern[I]=='w')?0:1;
        for (I=5; I<9; I++)
        {
            Index[I]=0;
            if (Pattern[I]!='_')
            {
                Index[I]=(Pattern[I]=='w')?1:2;
            }
        }
        WeightTable[Index[0]][Index[1]][Index[2]][Index[3]][Index[4]][Index[5]][Index[6]][Index[7]][Index[8]]=Weight;
    }
    fclose(Data);
}

int Evaluate(int X,int Y,int C,char TestBoard[15][15])
{
    char Direct[4][2]= {{1,0},{1,1},{0,1},{-1,1}}; //YX
    int I,J,K;
    int Sum;
    int Index[9];

    for (I=0; I<9; I++)
    {
        Index[I]=0;
    }
    for (Sum=0,I=0; I<4; I++)
    {
        for (J=-4; J<=-4; J++)
        {
            for (K=0; K<4; K++)
            {
                if ( (Y+J*Direct[I][0] + K*Direct[I][0]>=0) && (Y+J*Direct[I][0]+ K*Direct[I][0]<15) && (X+J*Direct[I][1]+ K*Direct[I][1]>=0) && (X+J*Direct[I][1]+ K*Direct[I][1]<15) )
                {
                    if(0==TestBoard[Y+J*Direct[I][0]+K*Direct[I][0]][X+J*Direct[I][1]+K*Direct[I][1]])
                    {
                        Index[K]=0;
                    }
                    else if(1==TestBoard[Y+J*Direct[I][0]+K*Direct[I][0]][X+J*Direct[I][1]+K*Direct[I][1]])
                    {
                        Index[K]=1;
                    }
                    else if(-1==TestBoard[Y+J*Direct[I][0]+K*Direct[I][0]][X+J*Direct[I][1]+K*Direct[I][1]])
                    {
                        Index[K]=2;
                    }
                }
                else
                {
                    Index[K]=((C==1)?2:1);
                }
            }
            ///////////////////////
            if(1==TestBoard[Y+J*Direct[I][0]+K*Direct[I][0]][X+J*Direct[I][1]+K*Direct[I][1]])
            {
                Index[K]=0;
            }
            else if(-1==TestBoard[Y+J*Direct[I][0]+K*Direct[I][0]][X+J*Direct[I][1]+K*Direct[I][1]])
            {
                Index[K]=1;
            }
            ///////////////////////
            for (K=5; K<9; K++)
            {
                if ( (Y+J*Direct[I][0] + K*Direct[I][0]>=0) && (Y+J*Direct[I][0]+ K*Direct[I][0]<15) && (X+J*Direct[I][1]+ K*Direct[I][1]>=0) && (X+J*Direct[I][1]+ K*Direct[I][1]<15) )
                {
                    if(0==TestBoard[Y+J*Direct[I][0]+K*Direct[I][0]][X+J*Direct[I][1]+K*Direct[I][1]])
                    {
                        Index[K]=0;
                    }
                    else if(1==TestBoard[Y+J*Direct[I][0]+K*Direct[I][0]][X+J*Direct[I][1]+K*Direct[I][1]])
                    {
                        Index[K]=1;
                    }
                    else if(-1==TestBoard[Y+J*Direct[I][0]+K*Direct[I][0]][X+J*Direct[I][1]+K*Direct[I][1]])
                    {
                        Index[K]=2;
                    }
                }
                else
                {
                    Index[K]=((C==1)?2:1);
                }
            }
        }
        /*
        if (WeightTable[Index[0]][Index[1]][Index[2]][Index[3]][Index[4]][Index[5]][Index[6]][Index[7]][Index[8]]<0&&who_play==-1)
            Sum-=1;
        else if (WeightTable[Index[0]][Index[1]][Index[2]][Index[3]][Index[4]][Index[5]][Index[6]][Index[7]][Index[8]]>0&&who_play==1)
            Sum+=1;
        /**/
        Sum+=WeightTable[Index[0]][Index[1]][Index[2]][Index[3]][Index[4]][Index[5]][Index[6]][Index[7]][Index[8]];

    }
    return Sum;
}

Move NextMove(int C)
{
    SSS=0;
    int I,J,K,L,BB;
    int Index;
    Move MyTemp;
    int Max=-1073741824,Min=1073741824;
    Move MyMove[256];
    char NextBoard[15][15];
    memset(HashTable,0,sizeof(ZT)*10000000);
    for (I=0,Index=0; I<15; I++)
    {
        for (J=0; J<15; J++)
        {
            if (MyTable[J][I]==0 )
            {
                MyTable[J][I]=C;
                K=Evaluate(I,J,C,MyTable);
                MyTable[J][I]=(C==1)?-1:1;
                L=Evaluate(I,J,(C==1)?-1:1,MyTable);
                MyTable[J][I]=0;
                MyMove[Index].X=I;
                MyMove[Index].Y=J;
                MyMove[Index].Score=abs(K)+abs(L);
                /**/
                if ( abs(K)>=(BB=WeightTable[0][0][0][0][0][1][1][1][1]) )
                {
                    return MyMove[Index];
                }
                /**/
                Index++;
            }
        }
    }
    qsort(MyMove,Index,sizeof(Move),CMP_Max);  //  Unstable

    for (I=0; I<Index&&I<10; I++)
    {
        memcpy(NextBoard,MyTable,15*15);
        NextBoard[MyMove[I].Y][MyMove[I].X]=C;
        K=MyMove[I].Score=MinMax(NextBoard,(C==1?-1:1),0,Max,Min,(CurrentZobrist^(ZobristTable[(C==1)?1:0][MyMove[I].Y*16+MyMove[I].X])));
        //printf("*****%d\n",MyMove[I].Score);
        /**/
        if (C==1)
        {
            if (K>Max)
            {
                Max=K;
            }
        }
        else
        {
            if (K<Min)
            {
                Min=K;
            }
        }

    }
    MyTemp=MyMove[0];
    if (C==1)
    {
        MyTemp.Score=-1073741824;
        for (I = 0; I < 10; I++)
        {
            if (MyMove[I].Score>MyTemp.Score)
            {
                MyTemp=MyMove[I];
            }
        }
    }
    else
    {
        MyTemp.Score=1073741824;
        for (I = 0; I < 10; I++)
        {
            if (MyMove[I].Score<MyTemp.Score)
            {
                MyTemp=MyMove[I];
            }
        }
    }
    printf("%d\n",Index);
    return MyTemp;
}


int MinMax(char TestBoard[15][15], int Side, int Depth,int Alpha,int Beta, unsigned long long Zobrist)
{
    int I,J,K,L,M;
    char NextBoard[15][15];
    int TA=Alpha,TB=Beta;
    int Max=-1073741824,Min=1073741824;
    Move MyMove[256];
    int Index;
    unsigned long long Z;
    if (Depth < DepthLimit)
    {
        Z=Zobrist;
        /**/
        while (HashTable[Z%7621583].Value!=0)
        {
            if (Zobrist==HashTable[Z%7621583].Hash)
            {
                return HashTable[Z%7621583].Value;
            }
            Z++;
        }
        /**/
        for (Index=0,I=0; I<15; I++)
        {
            for (J=0; J<15; J++)
            {
                if (TestBoard[J][I]==0 )
                {
                    TestBoard[J][I]=Side;
                    K=Evaluate(I,J,Side,TestBoard);  //xy
                    TestBoard[J][I]=(Side==1)?-1:1;
                    L=Evaluate(I,J,(Side==1)?-1:1,TestBoard);
                    TestBoard[J][I]=0;
                    MyMove[Index].X=I;
                    MyMove[Index].Y=J;
                    MyMove[Index].Score=abs(K)+abs(L);
                    /**/
                    if ( abs(K)>=WeightTable[0][0][0][0][0][1][1][1][1])
                    {
                        HashTable[Z%(7621583)].Value=(Side==1)?1073741824:-1073741824;
                        HashTable[Z%(7621583)].Hash=Zobrist;
                        return (Side==1)?1073741824:-1073741824;
                    }
                    /**/
                    Index++;
                }
            }
        }
        qsort(MyMove,Index,sizeof(Move),CMP_Max);
        for (I=0; I<Index&&I<10; I++)
        {

            TestBoard[MyMove[I].Y][MyMove[I].X]=Side;
            K=MinMax(TestBoard,(Side==1?-1:1),Depth+1,Alpha,Beta,(Zobrist^(ZobristTable[(Side==1)?1:0][MyMove[I].Y*16+MyMove[I].X])));
            TestBoard[MyMove[I].Y][MyMove[I].X]=0;
            if (Side==1)
            {
                if (K>Max)
                {
                    Max=K;
                    if (Alpha<Max)
                        Alpha=Max;
                }
            }
            else
            {
                if (K<Min)
                {
                    Min=K;
                    if (Beta>Min)
                        Beta=Min;
                }
            }
            if (Alpha>=Beta)
            {
                while (HashTable[Z%7621583].Value!=0)
                {
                    Z++;
                }
                HashTable[Z%(7621583)].Value=(Side==1?Max:Min);
                HashTable[Z%(7621583)].Hash=Zobrist;
                return (Side==1?Max:Min);
            }

        }
        while (HashTable[Z%7621583].Value!=0)
        {
            Z++;
        }
        HashTable[Z%(7621583)].Value=(Side==1?Max:Min);
        HashTable[Z%(7621583)].Hash=Zobrist;
        return (Side==1?Max:Min);
    }
    else
    {
        for (I=0,K=0,L=0; I<15; I++)
        {
            for (J=0; J<15; J++)
            {
                if (TestBoard[J][I]==0 )
                {
                    TestBoard[J][I]=(Side==1)?-1:1;
                    M=Evaluate(I,J,(Side==1)?-1:1,TestBoard);
                    L+=M;
                    TestBoard[J][I]=Side;
                    M=Evaluate(I,J,Side,TestBoard);
                    K+=M;
                    TestBoard[J][I]=0;
                    /**/
                    if (abs(M)==abs(WeightTable[0][0][0][0][0][1][1][1][1]))
                    {
                        return (Side==1)?1073741824:-1073741824;
                    }
                    /**/
                }
            }
            SSS++;
        }
        return L+K;
    }
}
