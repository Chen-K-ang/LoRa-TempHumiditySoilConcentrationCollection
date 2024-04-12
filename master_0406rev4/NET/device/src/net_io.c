/**************************************************************

	*	�ļ����ƣ� 	    net_io.c
	*
	*	��    �ߣ� 		�շ�
	*
	*	��    �ڣ� 		2019-8
	*
	*	˵    ���� 		�����豸IO����
	*
	*	�޸ļ�¼��		
	
****************************************************************/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����IO
#include "net_io.h"
#include "onenet.h"

//Ӳ������
#include "delay.h"
#include "usart.h"
#include "stdlib.h"

NET_IO_INFO net_io_info = {{0}, {0}, 0, 0, 0};


const unsigned char tag[] = {0, 127, 255};			//�Զ��������
const unsigned char tag_num = sizeof(tag);


/*
************************************************************
*	�������ƣ�	NET_IO_Init
*
*	�������ܣ�	��ʼ�������豸IO������
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		�ײ�������շ�����
************************************************************
*/
void NET_IO_Init(void)
{

	Usart3_Init(9600);
#if(USART_DMA_RX_EN == 1)
	USARTx_ResetMemoryBaseAddr(NET_IO, (unsigned int)net_io_info.buf, RB_BUFFER_SIZE, USART_RX_TYPE);
#endif

}

/*
************************************************************
*	�������ƣ�	NET_IO_AddTag2End
*
*	�������ܣ�	��ÿһ֡����ĩβ�����Զ���Ľ�����
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		��ӽ��������Է����ϴ�����һ֡��������
************************************************************
*/
void NET_IO_AddTag2End(void)
{

	unsigned char i = 0;
	
	for(; i < tag_num; i++)								//��һ֡����β��ӽ�����
	{
		net_io_info.buf[net_io_info.write_pos++] = tag[i];
		net_io_info.write_pos %= RB_BUFFER_SIZE;
	}
	
#if(USART_DMA_RX_EN == 1)
	DMA_Cmd(DMA1_Channel3, DISABLE);												//�ر�DMA

	DMA_SetCurrDataCounter(DMA1_Channel3, RB_BUFFER_SIZE - net_io_info.write_pos);	//��������ʣ���ֽ�
	DMA1_Channel3->CMAR = (unsigned int)(net_io_info.buf + net_io_info.write_pos);	//�������õ�ǰDMAָ���ڴ��ָ��
	
	DMA_Cmd(DMA1_Channel3, ENABLE);													//ʹ��MDA
#endif
	
	net_io_info.data_packet++;							//���ݰ�ָ������

}

/*
************************************************************
*	�������ƣ�	NET_IO_CheckTag
*
*	�������ܣ�	��ȡ������һ֡���ݴ�С
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		���ݽ��������ж�һ֡���ݵĳ���
************************************************************
*/
unsigned short NET_IO_CheckTag(void)
{
	
	unsigned short bytes = 0, read_pos = net_io_info.read_pos;
	unsigned char i = 0;
	
	while(i < tag_num)											//���δ�ҵ�����������
	{
		if(net_io_info.buf[read_pos] == tag[i])					//�ҵ��˵�һ��
			i++;												//���
		else													//����ͽ�������ƥ��
		{
			i = 0;												//��ͷ���
			if(net_io_info.buf[read_pos] == tag[i])				//�Ƚϵ�ǰ����ǲ��ǵ�һ��������
				i++;
		}
		
		bytes++;
		read_pos++;
		
		bytes %= RB_BUFFER_SIZE;								//���������鷶Χ
		read_pos %= RB_BUFFER_SIZE;
	}

	return bytes;

}

/*
************************************************************
*	�������ƣ�	NET_IO_Send
*
*	�������ܣ�	��������
*
*	��ڲ�����	str����Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		�ײ�����ݷ�������
*
************************************************************
*/
void NET_IO_Send(unsigned char *str, unsigned short len)
{
	
#if(USART_DMA_TX_EN == 0)
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		while(USART_GetFlagStatus(NET_IO, USART_FLAG_TC) == RESET);		//�ȴ��������
		USART_SendData(NET_IO, *str++);									//��������
		
	}
#else
	unsigned int mAddr = (unsigned int)str;
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET);					//�ȴ�ͨ��7�������
	DMA_ClearFlag(DMA1_FLAG_TC2);										//���ͨ��7������ɱ�־
	
	USARTx_ResetMemoryBaseAddr(NET_IO, mAddr, len, USART_TX_TYPE);
#endif

}

/*
************************************************************
*	�������ƣ�	NET_IO_Read
*
*	�������ܣ�	��ȡ���յ�����
*
*	��ڲ�����	��
*
*	���ز�����	��ȡ��֡����ָ��
*
*	˵����		
************************************************************
*/
unsigned char *NET_IO_Read(void)
{
	
	unsigned short read_num = 0, i = 0;
	unsigned char *buf = NULL;
	
	if(net_io_info.data_packet > 0)													//�����������ݰ��ɶ�ʱ
	{
		if(--net_io_info.data_packet == 65535)
			net_io_info.data_packet = 0;
		
		read_num = NET_IO_CheckTag();												//������Ҫ��ȡ�����ݳ���
		
		if(net_io_info.write_pos <= net_io_info.read_pos)							//�������ݡ���ѭ����һȦ*********************
		{
			if(net_io_info.read_pos + read_num < RB_BUFFER_SIZE)					//��ǰ��֡���ݻ�δ����rb
			{
				buf = net_io_info.buf + net_io_info.read_pos;
			}
			else																	//�������ǰ�γ�����rbβ����γ�����rb��
			{
				unsigned short num = RB_BUFFER_SIZE - net_io_info.read_pos;			//����rbβ��Ҫ�������ݳ���
				
				if(read_num >= RB_BUFFER2_SIZE)										//������ڶ�����������
					return NULL;
				
				for(i = 0; i < num; i++)											//����rbβ����Ч���ݵ���������
					net_io_info.buf_tp[i] = net_io_info.buf[i + net_io_info.read_pos];
				
				for(i = 0; i < read_num - num; i++)									//����rb�׵���Ч���ݵ���������
					net_io_info.buf_tp[i + num] = net_io_info.buf[i];
				
				buf = net_io_info.buf_tp;
			}
		}
		else																		//�������ݡ��һ�δѭ��һȦ*******************
		{
			buf = net_io_info.buf + net_io_info.read_pos;
		}
		
		net_io_info.read_pos += read_num;											//�ƶ���ָ��
		net_io_info.read_pos %= RB_BUFFER_SIZE;										//ȷ����ָ�벻������rb��Χ
	}
	
	return buf;

}

/*
************************************************************
*	�������ƣ�	DMA1_Channel6_IRQHandler
*
*	�������ܣ�	DMA1_ͨ��6_�жϺ���
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void DMA1_Channel3_IRQHandler(void)
{

	if(DMA_GetITStatus(DMA1_IT_TC3) == SET)
	{

#if(USART_DMA_RX_EN == 1)
		net_io_info.write_pos = 0;
		USARTx_ResetMemoryBaseAddr(NET_IO, (unsigned int)net_io_info.buf, RB_BUFFER_SIZE, USART_RX_TYPE);
#endif
		DMA_ClearFlag(DMA1_IT_TC3);
	}

}

/*
************************************************************
*	�������ƣ�	USART2_IRQHandler
*
*	�������ܣ�	USART2�ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
extern DATA_STREAM data_stream[];
extern u8 data_stream_cnt;
void USART3_IRQHandler(void)
{
	
#if(USART_DMA_RX_EN == 0)
	if(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET)			//�����ж�
	{
		USART_ClearFlag(USART3, USART_FLAG_RXNE);
		
		net_io_info.buf[net_io_info.write_pos++] = USART3->DR;				//д��rb
		net_io_info.write_pos %= RB_BUFFER_SIZE;							//������rb��Χ��
		
		//OneNET_CmdHandle(data_stream, data_stream_cnt);
	}
#endif
	
	if(USART_GetFlagStatus(USART3, USART_FLAG_IDLE) != RESET)			//�����ж�
	{
		USART3->DR;														//��ȡ����ע�⣺������Ҫ�������ܹ�����жϱ�־λ
		USART_ClearFlag(USART3, USART_IT_IDLE);
		
#if(USART_DMA_RX_EN == 1)
		net_io_info.write_pos = RB_BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);
#endif
		
		NET_IO_AddTag2End();											//�յ�һ֡�������ݣ�������β��ӽ�����
	}
	


}
