void iniSCI(void)
{
    TX1M=1; 
    RX1M=1;
}

void Ini2571(void)
{    
    PAT=0X80;    
    PBT=0X05;     //0x85      
    PCT=0X10;  
}       


void IO_Init(void)
{
    //GIE=1;
    PBT=0x05;
    PAT=0x80;
    PCT=0x10;
    STA=1;
}