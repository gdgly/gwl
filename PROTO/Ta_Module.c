#include "Ta_Module.h"
#include "Sgd_types.h"












/***************************************
*����:buf, len
*���:��
*����:sum����buf���ۼӺ�
*����:�����ۼӺ�
*����:
***************************************/
u8 SgdGetSum( u8 *buf, u8 len )
{
  u16 sum = 0x00;
  u16 i;
  for( i = 0; i < len; i ++)
  {
    sum += *( buf + i );
  }
  return (sum & 0xFF);
}



/***************************************
*����:addr, msg, msg_len
*���:addr -��Ӧ�ĵ�ַ
*����:pos -68��ʼλ��
*����:�������������Ƿ�Ϊ��ȷ��645��Ӧ֡
*����:
***************************************/
s8  SgdCheck645UpLineFrame( u8 *addr, u8 *buf,u8 len )
{
  s8 i,pos =-1;  
  
  for( i = 0; i < len; i++ )
  {
    sProHead_645_ex* p = (u8 *)&buf[i];
    
    if(p->startCode1 != START_CODE1 ) continue;
    s8 ret = i;
    if(p->startCode2 != START_CODE2 ) continue;    
    if(UP_DIRECTION != p->sCtrCode.direction) continue;

    sProEnd_645 sp = (&p->sCtrCode + p->dataLen);
    if( sp->sumCrc != SgdGetSum( p->dataArea, p->dataLen ) ) continue;
    if( sp->endCode != END_CODE ) continue;
    memcpy( addr, p->addr, 6 );
    pos = ret;
    break;
  }

  return pos;
}

/***************************************
*����:buf,addr, DI, data,datalen
*���:��
*����:pos -�����֡�ĳ���
*����:����645����֡
*����:
***************************************/
u8 SgdBulid645DownLineFrame(  u8 *buf,u8 *addr,u8 *DI,u8 *data ,u8 datalen)
{
    u8 pos = 0;    
    sProHead_645_ex* p = &buf;

    p->startCode1 = START_CODE1;
    memcpy(p->addr, addr,6);    
    p->startCode2 = START_CODE2;
    p->sCtrCode.direction = DOWN_DIRECTION;
    p->sCtrCode.functionCode = CONTROL_CODE_645_EX; 
    
    memcpy(p->dataArea.DI,DI, 4);
    memset(p->dataArea.opCode,0, 4);
    memcpy(p->dataArea.data,data, datalen);

    p->dataLen = sizeof(sDataArea_645) +datalen;
    pos += sizeof(sProHead_645_ex);
    pos += p->dataLen;

    ProEnd_645 sp = &buf[pos];
    sp->sumCrc = SgdGetSum( p->dataArea, p->dataLen );
    sp->endCode = END_CODE;
    pos += sizeof(sProEnd_645);

    return pos;
}

//��ȡ TA ר��ģ��״̬
u8 SgdGetTAstatus()
{


}









u8 SgdTimetoGetTADataProc()
{

    


}
