#include "Rs232Linux.h"
#include <iostream>
#include <string.h>
#include <iomanip>
#include "protocol.h"
#include "util.h"

void *FuncThread(void *arg)
{
	Rs232Linux *rsinstance  = (Rs232Linux *)arg;
	rsinstance->uart_pthread(NULL);
}
void    Rs232Linux::Init(unsigned char*strname)
{          

}
void    Rs232Linux::SetNotifyHandler(OnCommNotifyFunc notifyfunc,void*tag)
{
        notfy  = notifyfunc;
        tempa  = tag;
}
int     Rs232Linux::WriteBuffer(char* buf,int len)
{
        write(fd,buf,len);
        return 0;
}
//read until len = need data len;
int     Rs232Linux::ReadBuffer(char* buf,int len)
{
	int no = 0;
	int rc = 0;
	int rcnum=len;
	return read(fd,buf,len);
	/*
	while(len)
	{
		rc=read(fd,&buf[no],1);
		if(rc>0)
			no=no+1;
		len=len-1;   
		//printf("buf = %d\n",buf[no]);
	}
        */
	//if(no!=rcnum)
	//	return -1;      
	return rcnum;       
}
int     Rs232Linux::Open(int port,int baud)
{
        openport("/dev/ttyUSB0");
        setport(fd,baud,8,1,'n');
//		pthread_create(&id1,NULL,FuncThread,this);
        int rc = 0;
        clock_gettime(CLOCK_MONOTONIC,&time1);
        clock_gettime(CLOCK_MONOTONIC,&time2);
        while(1)
        {
        rc=readport(fd,20); //10 haomiao
//        printf("11111\n");
        usleep(100);
        }
        return true;
}
 
int     Rs232Linux::close()
{
        return 0;
}
unsigned char   Rs232Linux::IsActive()
{
        return 0;
}
int     Rs232Linux::Length()
{
        return 0;
}

void Rs232Linux::uart_pthread(void* arg)
{
    int rc = 0;
    while(1)
    {      
    rc=readport(fd,4); //10 haomiao
	usleep(100);
    }
}

void Rs232Linux::parse() {
    static unsigned char res = 0xFF;
    static bool frame_head = false, frame_receive_start_flag = false;
    static unsigned short count = 0;
    static unsigned char frame_data[32] = {0};
    auto buf = rq_com_rcv_buff2;
    for (int i = 0; i < buff_length; i++) {
        res = buf[i];
        if (res == 0xAA && !frame_head) {
            //接收帧头0xAA
            frame_data[count] = res;
            count++;
            frame_head = true;
            continue;
        }

        if (frame_head) {
            if (res == 0x55 && !frame_receive_start_flag) {
                //接收帧头0x55
                frame_data[count] = res;
                count++;
                frame_receive_start_flag = true;
                continue;
            }

            if (frame_receive_start_flag) {
                // 判断是否为接收串口通讯协议格式定义的最后一个字节
                if ((count > 4) && (count == *((unsigned short *)&frame_data[3]) - 1)) {
                    frame_data[count] = res;
                    if(CRC16(frame_data, count-1) != *((unsigned short *)&frame_data[30]))
                    {
//                        qDebug("Error: cal_crc: 0x%04X, recv_crc: 0x%04X,", CRC16(frame_data, count-1), *((unsigned short *)&frame_data[30]));
                        count = 0;
                        frame_head = false;
                        frame_receive_start_flag = false;
                        continue;
                    }
                    // 判断命令号
                    switch (frame_data[5]) {
                    case 0x10: {
                        MSGO_FORCE_CONTROL *data = (MSGO_FORCE_CONTROL *)frame_data;
//                        qDebug("address: 0x%02X", data->header);
//                        qDebug("address: 0x%02X", data->header2);
//                        qDebug("address: 0x%02X", data->address);
//                        qDebug("length: 0x%04X", data->length);
//                        qDebug("cmd: 0x%01X", data->cmd);
//                        qDebug("Fx: %d", data->channel);
//                        qDebug("Fy: %d", data->channel2);
//                        qDebug("Fz: %d", data->channel3);
//                        qDebug("Mx: %d", data->channel4);
//                        qDebug("My: %d", data->channel5);
//                        qDebug("Mz: %d", data->channel6);
//                        qDebug("crc: 0x%04X", data->crc);
//                        qDebug("icount: %d", ++icount);
                        {
                            std::stringstream buffer;
                            buffer << std::left << std::setw(8) << data->channel / 1000.0 << "," << std::setw(8) << data->channel2 / 1000.0
                                   << "," << std::setw(8) << data->channel3 / 1000.0 << "," << std::setw(8) << data->channel4 / 1000.0
                                   << "," << std::setw(8) << data->channel5 / 1000.0 << "," << std::setw(8) << data->channel6 / 1000.0
                                   << ",";

                            while (Demo::isrun) {
                                //                                std::cout << "raw : " << buffer.str() << "\n";
                                while (control_algorithm.serial_data.enqueue({data->channel / 1000.0, data->channel2 / 1000.0,
                                                                             data->channel3 / 1000.0, data->channel4 / 1000.0,
                                                                             data->channel5 / 1000.0, data->channel6 / 1000.0}))
                                {
                                    break;
                                }
                                while (logger.serial_buffer.enqueue(buffer.str())) {
                                    count = 0;
                                    frame_head = false;
                                    frame_receive_start_flag = false;
                                    return;
                                }
                            }
                            //                            time1=time2;
                        }
                        break;
                    }
                    default:
                        break;
                    }
                    count = 0;
                    frame_head = false;
                    frame_receive_start_flag = false;
                } else {
                    frame_data[count] = res;
                    count++;
                    // 判断数据帧长度是否为串口通讯协议格式定义的长度
                    if (count == 5) {
                        switch (*((unsigned short *)&frame_data[3])) {
                        case 0x0020:
                        case 0x0026:
                            break;
                        default:
                            count = 0;
                            frame_head = false;
                            frame_receive_start_flag = false;
                            continue;
                        }
                    }
                }
            } else {
                count = 0;
                frame_head = false;
                continue;
            }
        }
    }
}

///check uart data receve or not
int Rs232Linux::readport(int fd,int maxwaittime)
{
	int rc = 0;
    int rc_len = 0;
//    struct timeval tv;
//    fd_set readfd;
//    tv.tv_sec=maxwaittime/1000;    //SECOND
//    tv.tv_usec=maxwaittime%1000*1000;  //USECOND
//    FD_ZERO(&readfd);
//    FD_SET(fd,&readfd);
//    rc=select(fd+1,&readfd,NULL,NULL,&tv);
//    if(rc>0)
	{

        clock_gettime(CLOCK_MONOTONIC,&period);

        period.tv_nsec += period_gap;
        while (period.tv_nsec >= 1000000000) {
            period.tv_nsec -= 1000000000;
            period.tv_sec++;
        }

        rc_len = read(fd,recbuff,buff_length);
        for(int i = 0; i < rc_len; i++)
        {
            //If the buffer overflows, set the index to the beginning
            if(rq_com_rcv_len2 == buff_length)
            {
                //Next bytes will overwrite the begining of the buffer
                rq_com_rcv_len2 = 0;
                break;
            }

            rq_com_rcv_buff2[rq_com_rcv_len2++] = recbuff[i];
        }

        memset(&recbuff,'\0',buff_length);

        //If there is enough characters,...
        if(rq_com_rcv_len2 >= package_length && rc_len >= 1)
        {
            parse();

            // 显示serial时间
//            clock_gettime(CLOCK_MONOTONIC,&time2);
//            printf("rec len %d, %lu ms\n",rc_len,(time2.tv_nsec-time1.tv_nsec)/1000/1000);
//            time1=time2;

        }
        clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&period,NULL);
	}
//    memset(&recbuff,'\0',buff_length);
//	else
//	{
//		return -1;
//	}
	return -1;
}
int	Rs232Linux::openport(char *Dev)   
{
    fd = open( Dev, O_RDONLY|O_NOCTTY );
	if (-1 == fd) 
	{    
		perror("Can''t Open Serial Port");
		return -1;  
	} 
	else 
		return fd;
} 
int Rs232Linux::setport(int fd, int baud,int databits,int stopbits,int parity)
{
	int baudrate;
    struct   termios   newtio;
    memset (&newtio, 0, sizeof (struct termios));

    baudrate=B460800;

    if (tcgetattr(fd,&newtio) != 0)
    {
        return -1;
    }
//	bzero(&newtio,sizeof(newtio));
	//setting   c_cflag 
	newtio.c_cflag   &=~CSIZE;     
	switch (databits) /*�����*/
	{   
		case 7:  
		newtio.c_cflag |= CS7; //7λ��
		break;
		case 8:     
		newtio.c_cflag |= CS8; //8λ��
		break;   
		default:    
		newtio.c_cflag |= CS8;
		break;    
	}
	switch (parity) //��У�
	{   
		case 'n':
		case 'N':    
		newtio.c_cflag &= ~PARENB;   /* Clear parity enable */
		newtio.c_iflag &= ~INPCK;     /* Enable parity checking */ 
		break;  
		case 'o':   
		case 'O':     
		newtio.c_cflag |= (PARODD | PARENB); /* ��Ϊ�Ч�*/  
		newtio.c_iflag |= INPCK;             /* Disnable parity checking */ 
		break;  
		case 'e':  
		case 'E':   
		newtio.c_cflag |= PARENB;     /* Enable parity */    
		newtio.c_cflag &= ~PARODD;   /* תΪżЧ�*/     
		newtio.c_iflag |= INPCK;       /* Disnable parity checking */
		break;
		case 'S': 
		case 's':  /*as no parity*/   
		newtio.c_cflag &= ~PARENB;
		newtio.c_cflag &= ~CSTOPB;break;  
		default:   
		newtio.c_cflag &= ~PARENB;   /* Clear parity enable */
		newtio.c_iflag &= ~INPCK;     /* Enable parity checking */ 
		break;   
	} 
	switch (stopbits)//��ֹͣλ
	{   
		case 1:    
		newtio.c_cflag &= ~CSTOPB;  //1
		break;  
		case 2:    
		newtio.c_cflag |= CSTOPB;  //2
		break;
		default:  
		newtio.c_cflag &= ~CSTOPB;  
		break;  
	} 
    newtio.c_cc[VMIN] = 0;
    newtio.c_cc[VTIME] = 1;
//	newtio.c_cc[VMIN] = 0;
	newtio.c_cflag   |=   (CLOCAL|CREAD);
	newtio.c_oflag|=OPOST; 
    newtio.c_iflag   &=~(IXON|IXOFF|IXANY);
    newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	cfsetispeed(&newtio,baudrate);   
	cfsetospeed(&newtio,baudrate);   
    tcflush(fd,   TCIOFLUSH);
	if (tcsetattr(fd,TCSANOW,&newtio) != 0)   
	{ 
		perror("SetupSerial 3");  
		return -1;  
	}  
	return 0;
}
