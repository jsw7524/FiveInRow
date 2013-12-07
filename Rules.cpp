int Playable(int Y,int X,int Player,unsigned char *ReverseRecord,char MyTestTable[8][8]);
int Reverse(int Y,int X,int Player,unsigned char ReverseRecord,char MyTestTable[8][8]);

int Playable(int Y,int X,int Player,unsigned char *ReverseRecord,char MyTestTable[8][8])
{
    int I,J;
    (*ReverseRecord)=0;
    if(Y-1>=0)
    {
        if (MyTestTable[Y-1][X]==(Player==1?-1:1))
        {
            for (I=Y-1; I>=0; I--)
            {
                if (MyTestTable[I][X]==Player)
                {
                     (*ReverseRecord)|=0x01;
                     break;
                }
                else if (MyTestTable[I][X]==0)
                    break;
            }
        }
    }

    if(Y-1>=0&&X-1>=0)
    {
        if (MyTestTable[Y-1][X-1]==(Player==1?-1:1))
        {
            for (I=Y-1,J=X-1; J>=0&&I>=0; J--,I--)
            {
                if (MyTestTable[I][J]==Player)
                {
                     (*ReverseRecord)|=0x02;
                     break;
                }
                else if (MyTestTable[I][J]==0)
                    break;
            }

        }
    }

    if(X-1>=0)
    {
        if (MyTestTable[Y][X-1]==(Player==1?-1:1))
        {
            for (J=X-1; J>=0; J--)
            {
                if (MyTestTable[Y][J]==Player)
                {
                     (*ReverseRecord)|=0x04;
                     break;
                }
                else if (MyTestTable[Y][J]==0)
                    break;
            }
        }
    }

    if(Y+1<=7&&X-1>=0)
    {
        if (MyTestTable[Y+1][X-1]==(Player==1?-1:1))
        {
            for (I=Y+1,J=X-1; J>=0&&I<8; J--,I++)
            {
                if (MyTestTable[I][J]==Player)
                {
                     (*ReverseRecord)|=0x08;
                     break;
                }
                else if (MyTestTable[I][J]==0)
                    break;
            }
        }
    }

    if(Y+1<8)
    {
        if (MyTestTable[Y+1][X]==(Player==1?-1:1))
        {
            for (I=Y+1; I<8; I++)
            {
                if (MyTestTable[I][X]==Player)
                {
                     (*ReverseRecord)|=0x10;
                     break;
                }
                else if (MyTestTable[I][X]==0)
                    break;
            }
        }
    }

    if(Y+1<8&&X+1<8)
    {
        if (MyTestTable[Y+1][X+1]==(Player==1?-1:1))
        {
            for (I=Y+1,J=X+1; J<8&&I<8; J++,I++)
            {
                if (MyTestTable[I][J]==Player)
                {
                     (*ReverseRecord)|=0x20;
                     break;
                }
                else if (MyTestTable[I][J]==0)
                    break;
            }
        }
    }
    if(X+1<8)
    {
        if (MyTestTable[Y][X+1]==(Player==1?-1:1))
        {
            for (J=X+1; J<8; J++)
            {
                if (MyTestTable[Y][J]==Player)
                {
                     (*ReverseRecord)|=0x40;
                     break;
                }
                else if (MyTestTable[Y][J]==0)
                    break;
            }
        }
    }
    if(Y-1>=0&&X+1<8)
    {
        if (MyTestTable[Y-1][X+1]==(Player==1?-1:1))
        {
            for (I=Y-1,J=X+1; J<8&&I>=0; J++,I--)
            {
                if (MyTestTable[I][J]==Player)
                {
                     (*ReverseRecord)|=0x80;
                     break;
                }
                else if (MyTestTable[I][J]==0)
                    break;
            }
        }
    }

    return (*ReverseRecord)==0?0:1;
}

int Reverse(int Y,int X,int Player,unsigned char ReverseRecord,char MyTestTable[8][8])
{
    int I,J;
    if (ReverseRecord&0x01)
    {
        for (I=Y-1; I>=0; I--)
        {
            if (MyTestTable[I][X]==(Player==1?-1:1))
                MyTestTable[I][X]=Player;
            else
                break;
        }
    }

    if (ReverseRecord&0x02)
    {
        for (I=Y-1,J=X-1; J>=0&&I>=0; J--,I--)
        {
            if (MyTestTable[I][J]==(Player==1?-1:1))
                MyTestTable[I][J]=Player;
            else
                break;
        }
    }


    if (ReverseRecord&0x04)
    {
        for (J=X-1; J>=0; J--)
        {
            if (MyTestTable[Y][J]==(Player==1?-1:1))
                MyTestTable[Y][J]=Player;
            else
                break;
        }
    }

    if (ReverseRecord&0x08)
    {
        for (I=Y+1,J=X-1; J>=0&&I<8; J--,I++)
        {
            if (MyTestTable[I][J]==(Player==1?-1:1))
                MyTestTable[I][J]=Player;
            else
                break;
        }
    }

    if (ReverseRecord&0x10)
    {
        for (I=Y+1; I<8; I++)
        {
            if (MyTestTable[I][X]==(Player==1?-1:1))
                MyTestTable[I][X]=Player;
            else
                break;
        }
    }

    if (ReverseRecord&0x20)
    {
        for (I=Y+1,J=X+1; J<8&&I<8; J++,I++)
        {
            if (MyTestTable[I][J]==(Player==1?-1:1))
                MyTestTable[I][J]=Player;
            else
                break;
        }
    }

    if (ReverseRecord&0x40)
    {
        for (J=X+1; J<8; J++)
        {
            if (MyTestTable[Y][J]==(Player==1?-1:1))
                MyTestTable[Y][J]=Player;
            else
                break;
        }
    }

    if (ReverseRecord&0x80)
    {
        for (I=Y-1,J=X+1; J<8&&I>=0; J++,I--)
        {
            if (MyTestTable[I][J]==(Player==1?-1:1))
                MyTestTable[I][J]=Player;  // / //
            else
                break;
        }
    }
    return 0;
}
