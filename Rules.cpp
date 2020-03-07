#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char MyTable[15][15];
int Order[15][15];
int  Round;
int Scores[15][15];

int GetBlackNumber();
int GetWhiteNumber();
void ResetGame();
int referee(int X,int Y,int C);

int referee(int X,int Y,int C)
{
    char Direct[4][2]={{1,0},{1,1},{0,1},{-1,1}};  //YX
    int I,J,K;
    int Sum;
    for (I=0;I<4;I++)
    {
        for (J=-4;J<=0;J++)
        {
            Sum=0;
            for (K=0;K<5;K++)
            {
                if ( (Y+J*Direct[I][0] + K*Direct[I][0]>=0) && (Y+J*Direct[I][0]+ K*Direct[I][0]<15) && (X+J*Direct[I][1]+ K*Direct[I][1]>=0) && (X+J*Direct[I][1]+ K*Direct[I][1]<15) )
                {
                    Sum+=MyTable[Y+J*Direct[I][0]+K*Direct[I][0]][X+J*Direct[I][1]+K*Direct[I][1]];
                }
            }
            if (Sum == C*5)
            {
                return 1;
            }
        }
    }
    return 0;
}

void ResetGame()
{
    int i,j;
    for(i=0; i<15; i++)
    {
        for(j=0; j<15; j++)
        {
            MyTable[i][j]=0;
            Order[i][j]=0;
        }
    }
    Round=0;

}


int GetBlackNumber()
{
    int B,I,J;
    for (B=0,I=0; I<8; I++)
        for (J=0; J<8; J++)
            if (MyTable[I][J]==-1)
                B++;
    return B;
}

int GetWhiteNumber()
{
    int W,I,J;
    for (W=0,I=0; I<8; I++)
        for (J=0; J<8; J++)
            if (MyTable[I][J]==1)
                W++;
    return W;
}

