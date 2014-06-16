#ifndef _MACRO_DEFINE_H
#define _MACRO_DEFINE_H


/*************************计量芯片相关************************************/
#define	WRITE		0x80
#define	READ		0x00

#define  U_60CONST   0x1430
#define  U_70CONST   0x1520
#define  U_80CONST   0x1600
#define  U_115CONST  0x1b00
//********************************************
//HG7220计量芯片  寄存器地址列表
//********************************************
#define	START			0x01//系统启动命令字
#define	ESW			0x03//计量状态字
#define	PGAC			0x05//PGA与HPF控制寄存器
#define	APHCAL		0x07//A线校准角差位
#define	EMOD			0x10//计量模式配置位
#define	FREQ			0x12 //电压频率
#define	UGAIN		    0x14//电压有效值增益
#define   UOFF          0x15//电压失调
#define	IAGAIN	    0x18//A线电流有效值增益
#define	IAOFF		    0x19//A线电流失调
#define	PAGAIN		0x1A//A线校准增益
#define	IBGAIN		0x20//B线电流有效值增益
#define	IBOFF		    0x21//B线电流失调
#define	PAOFF		    0x1B//A线有功功率失调
#define   AFAC           0x1C//A线功率因数
#define	PSTART		0x28//有功启动功率门限值(5A表)
#define	IARMS			0x30//A线电流有效值
#define	IBRMS			0x31//B线电流有效值
#define	URMS			0x32//电压有效值
#define	PA_EMU	    0x41//功率有效值
#define	PFSET			0x45//有功高频脉冲常数
#define   WPCMD         0x70//写保护寄存器




//******************************************************************************
//HG7220计量芯片  寄存器常数配置(以下配置数据不同表型可能不同，故在此宏定义)
//******************************************************************************

#define	STARTB		0x07     //b7~b5:XXX,b4:0无功计量不使能,b3:1选择功率失调模式,b2:1使能过零功能、欠压功能、频率和相角计算位,b1:使能有效值、视在功率和功率因数测量

#define	PSTARTHB		0x00   //设置启动功率为2.2W,则PSTART为   65535*2.2/(K_V/((2^6)*R_I*Gi*Gu))
#define	PSTARTLB		0x4d
#define	PFSETHHB		0x00   //PFSET=INT(6.03979776*10e14)*(Gia*R*Gu)/(Mc*k)=(6.03979776*10e14)*(24*350*10e(-6)*1)/(1200*1000)=4227858=0X408312
#define	PFSETHLB		0x40
#define	PFSETLHB		0x83
#define	PFSETLLB		0x12
#define	EMODHB		0x00  //b15~b11:XXXXX,b10:0,无功计量电流通道选择A路(b8禁止，此位无效),b9:0, CF2脉冲输出选择位(b8禁止，此位无效),b8:0,屏蔽无功电能绝对值计量
#define	EMODLB		0x9F  //b7~b4:1001,防窃电模式下A线和B线功率比较门限值(0.78125*10)%(ABSEL为固定A线模式此设置无效),b3~b2:11,固定A线模式同时打开B线,b1:1,正向或反向有功电能脉冲输出,b0:1,使能有功电能绝对值计量

#define	PGACB		0x85  //b7:1,使能HPF,b6~b5:00,电压通道增益为1倍，b4~b3:00,IB电流通道增益为1倍，b2~b0:101,IA电流通道增益为24倍

//计量参数
#define  K_V   1000     //电压通道分压比
#define  R_I   350      //锰铜阻值(单位微欧)
#define  RO_B  (10.0/2000)         //零线电流互感器输出端电阻/分流比

//计量芯片SPI操作定
#define  EMU_RST_EN   (PG0 = )
#define  EMU_RST_DIS  (PG0 = )

#define	SPICS_EN		(PG1 = 0)
#define	SPICS_DIS	(PG1 = 1)

#define	SPICLK1		(PD0 = 1)
#define	SPICLK0		(PD0 = 0)

#define	SPIMO0		(PD5 = 0)
#define	SPIMO1		(PD5 = 1)

#define  SPIMI        PD1
/*****函数执行结果******/
#define 	SUCCEED	   0X00        //成功
#define  FAILED	   0X01        //失败

/*****二进制数据定义*****/
#define B0000_0000 0x00
#define B0000_0001 0x01
#define B0000_0010 0x02
#define B0000_0011 0x03
#define B0000_0100 0x04
#define B0000_0101 0x05
#define B0000_0110 0x06
#define B0000_0111 0x07
#define B0000_1000 0x08
#define B0000_1001 0x09
#define B0000_1010 0x0A
#define B0000_1011 0x0B
#define B0000_1100 0x0C
#define B0000_1101 0x0D
#define B0000_1110 0x0E
#define B0000_1111 0x0F

#define B0001_0000 0x10
#define B0001_0001 0x11
#define B0001_0010 0x12
#define B0001_0011 0x13
#define B0001_0100 0x14
#define B0001_0101 0x15
#define B0001_0110 0x16
#define B0001_0111 0x17
#define B0001_1000 0x18
#define B0001_1001 0x19
#define B0001_1010 0x1A
#define B0001_1011 0x1B
#define B0001_1100 0x1C
#define B0001_1101 0x1D
#define B0001_1110 0x1E
#define B0001_1111 0x1F

#define B0010_0000 0x20
#define B0010_0001 0x21
#define B0010_0010 0x22
#define B0010_0011 0x23
#define B0010_0100 0x24
#define B0010_0101 0x25
#define B0010_0110 0x26
#define B0010_0111 0x27
#define B0010_1000 0x28
#define B0010_1001 0x29
#define B0010_1010 0x2A
#define B0010_1011 0x2B
#define B0010_1100 0x2C
#define B0010_1101 0x2D
#define B0010_1110 0x2E
#define B0010_1111 0x2F

#define B0011_0000 0x30
#define B0011_0001 0x31
#define B0011_0010 0x32
#define B0011_0011 0x33
#define B0011_0100 0x34
#define B0011_0101 0x35
#define B0011_0110 0x36
#define B0011_0111 0x37
#define B0011_1000 0x38
#define B0011_1001 0x39
#define B0011_1010 0x3A
#define B0011_1011 0x3B
#define B0011_1100 0x3C
#define B0011_1101 0x3D
#define B0011_1110 0x3E
#define B0011_1111 0x3F

#define B0100_0000 0x40
#define B0100_0001 0x41
#define B0100_0010 0x42
#define B0100_0011 0x43
#define B0100_0100 0x44
#define B0100_0101 0x45
#define B0100_0110 0x46
#define B0100_0111 0x47
#define B0100_1000 0x48
#define B0100_1001 0x49
#define B0100_1010 0x4A
#define B0100_1011 0x4B
#define B0100_1100 0x4C
#define B0100_1101 0x4D
#define B0100_1110 0x4E
#define B0100_1111 0x4F

#define B0101_0000 0x50
#define B0101_0001 0x51
#define B0101_0010 0x52
#define B0101_0011 0x53
#define B0101_0100 0x54
#define B0101_0101 0x55
#define B0101_0110 0x56
#define B0101_0111 0x57
#define B0101_1000 0x58
#define B0101_1001 0x59
#define B0101_1010 0x5A
#define B0101_1011 0x5B
#define B0101_1100 0x5C
#define B0101_1101 0x5D
#define B0101_1110 0x5E
#define B0101_1111 0x5F

#define B0110_0000 0x60
#define B0110_0001 0x61
#define B0110_0010 0x62
#define B0110_0011 0x63
#define B0110_0100 0x64
#define B0110_0101 0x65
#define B0110_0110 0x66
#define B0110_0111 0x67
#define B0110_1000 0x68
#define B0110_1001 0x69
#define B0110_1010 0x6A
#define B0110_1011 0x6B
#define B0110_1100 0x6C
#define B0110_1101 0x6D
#define B0110_1110 0x6E
#define B0110_1111 0x6F

#define B0111_0000 0x70
#define B0111_0001 0x71
#define B0111_0010 0x72
#define B0111_0011 0x73
#define B0111_0100 0x74
#define B0111_0101 0x75
#define B0111_0110 0x76
#define B0111_0111 0x77
#define B0111_1000 0x78
#define B0111_1001 0x79
#define B0111_1010 0x7A
#define B0111_1011 0x7B
#define B0111_1100 0x7C
#define B0111_1101 0x7D
#define B0111_1110 0x7E
#define B0111_1111 0x7F

#define B1000_0000 0x80
#define B1000_0001 0x81
#define B1000_0010 0x82
#define B1000_0011 0x83
#define B1000_0100 0x84
#define B1000_0101 0x85
#define B1000_0110 0x86
#define B1000_0111 0x87
#define B1000_1000 0x88
#define B1000_1001 0x89
#define B1000_1010 0x8A
#define B1000_1011 0x8B
#define B1000_1100 0x8C
#define B1000_1101 0x8D
#define B1000_1110 0x8E
#define B1000_1111 0x8F

#define B1001_0000 0x90
#define B1001_0001 0x91
#define B1001_0010 0x92
#define B1001_0011 0x93
#define B1001_0100 0x94
#define B1001_0101 0x95
#define B1001_0110 0x96
#define B1001_0111 0x97
#define B1001_1000 0x98
#define B1001_1001 0x99
#define B1001_1010 0x9A
#define B1001_1011 0x9B
#define B1001_1100 0x9C
#define B1001_1101 0x9D
#define B1001_1110 0x9E
#define B1001_1111 0x9F

#define B1010_0000 0xA0
#define B1010_0001 0xA1
#define B1010_0010 0xA2
#define B1010_0011 0xA3
#define B1010_0100 0xA4
#define B1010_0101 0xA5
#define B1010_0110 0xA6
#define B1010_0111 0xA7
#define B1010_1000 0xA8
#define B1010_1001 0xA9
#define B1010_1010 0xAA
#define B1010_1011 0xAB
#define B1010_1100 0xAC
#define B1010_1101 0xAD
#define B1010_1110 0xAE
#define B1010_1111 0xAF

#define B1011_0000 0xB0
#define B1011_0001 0xB1
#define B1011_0010 0xB2
#define B1011_0011 0xB3
#define B1011_0100 0xB4
#define B1011_0101 0xB5
#define B1011_0110 0xB6
#define B1011_0111 0xB7
#define B1011_1000 0xB8
#define B1011_1001 0xB9
#define B1011_1010 0xBA
#define B1011_1011 0xBB
#define B1011_1100 0xBC
#define B1011_1101 0xBD
#define B1011_1110 0xBE
#define B1011_1111 0xBF

#define B1100_0000 0xC0
#define B1100_0001 0xC1
#define B1100_0010 0xC2
#define B1100_0011 0xC3
#define B1100_0100 0xC4
#define B1100_0101 0xC5
#define B1100_0110 0xC6
#define B1100_0111 0xC7
#define B1100_1000 0xC8
#define B1100_1001 0xC9
#define B1100_1010 0xCA
#define B1100_1011 0xCB
#define B1100_1100 0xCC
#define B1100_1101 0xCD
#define B1100_1110 0xCE
#define B1100_1111 0xCF

#define B1101_0000 0xD0
#define B1101_0001 0xD1
#define B1101_0010 0xD2
#define B1101_0011 0xD3
#define B1101_0100 0xD4
#define B1101_0101 0xD5
#define B1101_0110 0xD6
#define B1101_0111 0xD7
#define B1101_1000 0xD8
#define B1101_1001 0xD9
#define B1101_1010 0xDA
#define B1101_1011 0xDB
#define B1101_1100 0xDC
#define B1101_1101 0xDD
#define B1101_1110 0xDE
#define B1101_1111 0xDF

#define B1110_0000 0xE0
#define B1110_0001 0xE1
#define B1110_0010 0xE2
#define B1110_0011 0xE3
#define B1110_0100 0xE4
#define B1110_0101 0xE5
#define B1110_0110 0xE6
#define B1110_0111 0xE7
#define B1110_1000 0xE8
#define B1110_1001 0xE9
#define B1110_1010 0xEA
#define B1110_1011 0xEB
#define B1110_1100 0xEC
#define B1110_1101 0xED
#define B1110_1110 0xEE
#define B1110_1111 0xEF

#define B1111_0000 0xF0
#define B1111_0001 0xF1
#define B1111_0010 0xF2
#define B1111_0011 0xF3
#define B1111_0100 0xF4
#define B1111_0101 0xF5
#define B1111_0110 0xF6
#define B1111_0111 0xF7
#define B1111_1000 0xF8
#define B1111_1001 0xF9
#define B1111_1010 0xFA
#define B1111_1011 0xFB
#define B1111_1100 0xFC
#define B1111_1101 0xFD
#define B1111_1110 0xFE
#define B1111_1111 0xFF


#endif
