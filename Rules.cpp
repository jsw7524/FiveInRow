

int Playable(int Y,int X,int Player,int ReverseRecord[9],int MyTestTable[8][8])
{
    int I,J;
    if(Y-1>=0)
    {
        if (MyTestTable[Y-1][X]==(Player==1?-1:1))
        {
            for (I=Y-1; I>=0; I--)
            {
                if (MyTestTable[I][X]==Player)
                    ReverseRecord[1]=1;  // |
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
                    ReverseRecord[2]=1;  // \ //
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
                    ReverseRecord[3]=1;  // - //
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
                    ReverseRecord[4]=1;  // / //
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
                    ReverseRecord[5]=1;  // | //
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
                    ReverseRecord[6]=1;  // \ //
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
                    ReverseRecord[7]=1;  // - //
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
                    ReverseRecord[8]=1; // / //
                else if (MyTestTable[I][J]==0)
                    break;
            }
        }
    }

    return ReverseRecord[1]+ReverseRecord[2]+ReverseRecord[3]+ReverseRecord[4]
           +ReverseRecord[5]+ReverseRecord[6]+ReverseRecord[7]+ReverseRecord[8];
}

int Reverse(int Y,int X,int Player,int ReverseRecord[9],int MyTestTable[8][8])
{
    int I,J;
    if (ReverseRecord[1]==1)
    {
        for (I=Y-1; I>=0; I--)
        {
            if (MyTestTable[I][X]==(Player==1?-1:1))
                MyTestTable[I][X]=Player;
            else
                break;
        }
    }

    if (ReverseRecord[2]==1)
    {
        for (I=Y-1,J=X-1; J>=0&&I>=0; J--,I--)
        {
            if (MyTestTable[I][J]==(Player==1?-1:1))
                MyTestTable[I][J]=Player;
            else
                break;
        }
    }


    if (ReverseRecord[3]==1)
    {
        for (J=X-1; J>=0; J--)
        {
            if (MyTestTable[Y][J]==(Player==1?-1:1))
                MyTestTable[Y][J]=Player;
            else
                break;
        }
    }

    if (ReverseRecord[4]==1)
    {
        for (I=Y+1,J=X-1; J>=0&&I<8; J--,I++)
        {
            if (MyTestTable[I][J]==(Player==1?-1:1))
                MyTestTable[I][J]=Player;
            else
                break;
        }
    }

    if (ReverseRecord[5]==1)
    {
        for (I=Y+1; I<8; I++)
        {
            if (MyTestTable[I][X]==(Player==1?-1:1))
                MyTestTable[I][X]=Player;
            else
                break;
        }
    }

    if (ReverseRecord[6]==1)
    {
        for (I=Y+1,J=X+1; J<8&&I<8; J++,I++)
        {
            if (MyTestTable[I][J]==(Player==1?-1:1))
                MyTestTable[I][J]=Player;
            else
                break;
        }
    }

    if (ReverseRecord[7]==1)
    {
        for (J=X+1; J<8; J++)
        {
            if (MyTestTable[Y][J]==(Player==1?-1:1))
                MyTestTable[Y][J]=Player;
            else
                break;
        }
    }

    if (ReverseRecord[8]==1)
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
