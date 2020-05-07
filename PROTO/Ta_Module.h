#ifndef _TA_MODULE_H_
#define _TA_MODULE_H_



#define MAC_SIZE 					4

#define CIRCUIT_NORMAL				0//æ­£å¸¸çŠ¶æ€
#define CIRCUIT_SHORTCUT			1//çŸ­è·¯
#define CIRCUIT_BREAK				2//å¼€è·¯
#define CIRCUIT_SEC_SEPARATE		4//äºŒæ¬¡åˆ†æµ


typedef struct TAG_FormatCode
{
    u8  IVcode:1;   //1Ê±£¬ ESAM Ê¹ÓÃÍâ²¿ IV£»¡°0¡±Ê±£¬ESAM Ê¹ÓÃÄÚ²¿ IV¡£
    u8  DirFlag:1;  //¡°1¡± Ê±£¬ Êı¾İÓÉÖ÷Õ¾ÏÂ·¢£»µ±ÏÂ´«±êÊ¶Îª¡°0¡± Ê±£¬ Êı¾İÓÉÖÕ¶ËÏÂ·¢
    u8  EncryFlag:1;//¡°1¡±Ê±£¬ 645 Ö¡ÖĞÊı¾İÇøµÄ DATA ÎªÃÜÎÄ£»µ±ÃÜÎÄ±êÊ¶Îª¡°0¡±Ê±£¬ 645 Ö¡ÖĞÊı¾İÇøµÄ DATA ÎªÃ÷ÎÄ
    u8  Reserve:5;
}__attribute__((packed))sTAG_FormatCode;

typedef struct Format_TA_Status
{
    sTAG_FormatCode FormatTag;
    u8  Phase_A_ststus;
    u8  Phase_B_ststus;
    u8  Phase_C_ststus;
    u8  Mac[MAC_SIZE];
}__attribute__((packed))sFormat_TA_Status;


sFormat_TA_Status GetTaStatus(void);
void TaModuleInit(void);


#endif
