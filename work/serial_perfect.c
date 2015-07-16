/*
 * =====================================================================================
 *
 *       Filename:  serial_demon.c
 *
 *    Description: this file is for open a thread to listen a GPIO COM  
 *
 *        Version:  1.0
 *        Created:  2015-07-13 01:43:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Mike) 
 *        Company: 	RunWell 
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memmap.h"
#include "hi.h"
#include "strfunc.h"

#include <pthread.h>
#include <string.h>
#include <stdarg.h>
#include "serial_demon.h"


//#####################   Logger ############################### 
#ifdef ENABLE_LOG
#define TRACE_LOG			trace_log
#else
#define TRACE_LOG     1 ? (void)0 : trace_log
#endif
#define LOG_FILE			"./serial.log"
#define BKUP_FILE		"./log.log.bak"
#define MAX_LOGSIZE		4194304		// 4 MB

unsigned long GetTickCount()
{
	static signed long long begin_time = 0;
	static signed long long now_time;
	struct timespec tp;
	unsigned long tmsec = 0;
	if ( clock_gettime(CLOCK_MONOTONIC, &tp) != -1 )
	{
		now_time = tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
	}
	if ( begin_time = 0 )
		begin_time = now_time;
	tmsec = (unsigned long)(now_time - begin_time);
	return tmsec;
}

static const char* time_stamp()
{
	static char timestr[64];
#ifdef linux
	char *p = timestr;
	struct timeval tv;
	gettimeofday( &tv, NULL );

	strftime( p, sizeof(timestr), "%y/%m/%d %H:%M:%S", localtime( &tv.tv_sec ) );
	sprintf( p + strlen(p), ".%03lu", tv.tv_usec/1000 );

#else
		SYSTEMTIME	tnow;
		GetLocalTime( &tnow );
		sprintf( timestr, "%04d/%02d/%02d %02d:%02d:%02d.%03d", 
					tnow.wYear, tnow.wMonth, tnow.wDay, 
					tnow.wHour, tnow.wMinute, tnow.wSecond, tnow.wMilliseconds );
#endif
	return timestr;
}

static void trace_log(const char *fmt,...)
{
	va_list		va;
	char		str[1024 ] = "";
	char		file[128];
	FILE *fp;
	
	va_start(va, fmt);
	vsprintf(str, fmt, va);
	va_end(va);

	fp = fopen( LOG_FILE, "a" );
	if ( fp!=NULL && ftell(fp) > MAX_LOGSIZE )
	{
		fclose(fp);
		remove( BKUP_FILE );
		rename( LOG_FILE, BKUP_FILE );
		fp = fopen( LOG_FILE, "a" );
	}
	if ( fp != NULL )
	{
		if ( fmt[0] != '\t' )
			fprintf(fp, "[%s] %s", time_stamp(), str );
		else
			fprintf(fp, "%26s%s", " ", str+1 );
		fclose(fp);
	}
}

//#####################  End Of Logger ############################### 

typedef unsigned char BYTE;
typedef void (*pcallback)(int,int);

//#typedef enum{
//#	FALSE = 0,
//#	TRUE = 1
//#}BOOL;

#define GPION_0 (1<<2)
#define GPION_1 (1<<3)
#define GPION_2 (1<<4)
#define GPION_3 (1<<5)
#define GPION_4 (1<<6)
#define GPION_5 (1<<7)
#define GPION_6 (1<<8)
#define GPION_7 (1<<9)
#define GPIO_DATA_OFFSET(x) (1<<(2+x))
// what you should do is mm

//typedef status_t enum Status; 


typedef struct NodeList serialNode;
typedef struct Serial_Ops serial_ops;
//  对于GPIO来说，功能：
//  0.初始化一个端口方向
//  1.状态获取(读轮训) setdir input
//  2.设置GPIO值
//   但是在实际使用中，我们可能需要输出端口状态后，再读取端口反馈
//   该部分接口可以随便添加，添加在Serial_ops，这些操作需要在硬件相关的
//   接口去实现
typedef int (*_set_init)(serialNode *m);
typedef int (*_set_ops)(serialNode *m);
typedef int (*_set_output_on)(serialNode *m);
typedef int (*_set_output_off)(serialNode *m);
typedef int (*_get_status)(serialNode *m);
typedef int (*logic_func)(serialNode *m);

#define GPIO_NORMAL			0x00 // GPIO status stay same
#define GPIO_DIFFERENT		0x01 // GPIO statue different

#define GPIO_FOR_READ			0		// the node is for the check the GPIO Status
#define GPIO_FOR_SET			1       // the node is for set the output fuse

#define NODE_BUSY				1
#define NODE_NO_BUSY			0
//这部分内容以后可能被状态为替代
typedef enum CMDTYOE{
	CMD_NORMAL=0,  // normal Run
	CMD_ACTIVE, // acive a command 
	CMD_READY, // begin the command and run
	CMD_RUN, // run the cmd and ready to close
}cmd_type_et;


struct Serial_Ops{
	// 初始化  
	_set_init	set_init;
	// 获取设置
	_set_ops	run_cmd;
	// 设置端口开
	_set_output_on set_on;
	// 设置端口关
	_set_output_off set_off;
	//  获取端口状态
	_get_status	get_status;
};

typedef unsigned long   DWORD;

// 与延时相关的逻辑处理代码，如判断GPIO状态（消除电流扰动）
// 若GPIO用来放按键，按键的处理，短按，长按 ，GPIO输出状态的处理，
// 如输出延时一段时间等操做，这里面最好用的就是状态机制

struct NodeList{
	// 暂时没用 	
	int id;

	// 可以用于该节点动能的描述
	char cmd_name[128];

    // 若该节点处于使用中状态，则该节点不可用。
	//   0 --- normal
	//   1 --- busy
	int b_busy; 

	//this coule be check  node分为输出型和输入型
	//使用宏 GPIO_FOR_READ 和 GPIO_FOR_SET
	int node_type; 


	DWORD lastTickCount;
	// 用于表示是否发送消息 该状态可能后期被去除
	int b_send;

	// 这里还需要一个定时器类型
	
	// GPIO 状态，这个状态可能也会被去除，使用一个状态来表示节点状态
	int gpio_status;  // kepp the gpio status

	//  命令中状态， 该类型为enum类型，状态为 
	//  NORMAL --> ACTIVE --> RUN
	//  这里后期修改所有的操作都放在状态位中，通过logic逻辑来控制
	//  因为GPIO操作基本上都有延时操作。
	//  0 ---- NORMAL
	//  1 ---- COMMAND
	cmd_type_et cmd_type; 	

	// 逻辑操作的回掉函数
	logic_func lg_fuc;

	serial_ops ops_p;
	serialNode *next;
};

// 链表头指针
serialNode *p_node_head = NULL;

#define ASC2NUM(ch) (ch - '0')
#define HEXASC2NUM(ch) (ch - 'A' + 10)
#define IOCONFIG_BASE				0x200F0000
#define GPIO0_BASE					0x20150000
#define GPIO_EACH_OFFSET			0x00010000

// 设备节点通过该函数注册到链表中
static int register_read_node(serialNode *node)
{
	serialNode *tmp;
	if(p_node_head == NULL){
		// add the node to the head
		p_node_head = node;
		node->next = NULL;
	}else{
		tmp = p_node_head;
		// here we need to check if the node list have 
		// the same node by the function
		while(tmp->next != NULL)
		{
			tmp = tmp->next;
		}
		tmp->next = node;
		node->next = NULL;
	}
	return 0;
}
// 调用节点中的init函数来进行端口的初始化，
// 初始化操作包含了所有的设置动作。
static int com_gpio_init()
{
	serialNode *tmp;
	if(p_node_head == NULL){
		printf("com read NULL\n");
		return -1;
	}
	tmp = p_node_head;
	while(tmp->next != NULL)
	{
		tmp->ops_p.set_init(tmp);
		tmp = tmp->next;
	}
}


int	gpio1_0_set_init(serialNode *m)
{
	// set mulit reg
	VOID *pMem = NULL;
	U32 ulOld,ulNew; 
	// set multi register
	pMem = memmap((0x200F0000+0xB8),16);
	ulOld = *(U32*)pMem;
	ulOld = ulNew | (0<<0); // set out put 
	*(U32*)pMem = ulNew;
	// set dir register
	pMem = memmap((0x20160000+0x400),16);
	ulOld = *(U32*)pMem;
	ulOld = ulNew | (1<<0);
	*(U32*)pMem = ulNew;
	// set data register	
	pMem = memmap((0x20160000+0x400),16);
	ulOld = *(U32*)pMem;
	ulOld = ulNew | (1<<2); // set data for 1
	*(U32*)pMem = ulNew;
	return 0;
}

int gpio1_0_run_cmd(serialNode *m)
{

	return 0;
}

int gpio1_0_get_status(serialNode *m)
{

	return 0;
}

int gpio1_0_callback_func(serialNode *m)
{
	// 逻辑完成的功能：当进入该命令时，从整个链表中寻找其他的node，并且关闭其他节点的任务，
	// 并且其余节点的状态变为 OFF 该节点转化为 ON，状态改为RUN， 到时间后状态转化为NORMAL
	// busy 改为 nobusy	
	int id = m->id;
	serialNode *node = m;
	serialNode *tmp;
	tmp = p_node_head;
	if(node->cmd_type == CMD_ACTIVE){
		while((tmp->cmd_type == GPIO_FOR_SET) && (id != tmp->id) && (tmp->b_busy == 1))
		{ // disable all the node that have the command 
			tmp->ops_p.set_off(tmp);		// set the output off
			tmp->b_busy = 0; // change the busy status to nobusy
			tmp->cmd_type = CMD_NORMAL;
			tmp = tmp->next;
		}
		// 
		node->ops_p.set_on(node);
		node->b_busy = 1;   // set busy
		node->cmd_type = CMD_READY;
	}else if(node->cmd_type == CMD_READY){
		// check the time if it is delay 
		if(GetTickCount() - node->lastTickCount > 10000){
			node->ops_p.set_off(node);
			node->b_busy = 0;
			node->cmd_type == CMD_NORMAL;
		}

	}else{	
		return 0;
	}

	return 0;
}

extern int COM_API_INIT()
{
	// this is set for one readNode
	// that will be check by thethread
	int Ret = 1;
	serialNode node0,node1,node2,node3;

	char cmdstr[128]="TEST_CMD1";
	memset(node0.cmd_name,0,sizeof(node0.cmd_name));
	strncpy(cmdstr,node0.cmd_name,sizeof(cmdstr));
	node0.cmd_type = CMD_NORMAL;
	node0.node_type = GPIO_FOR_READ;   //  for check the GPIO status
	// node operation
	node0.ops_p.set_init = gpio1_0_set_init;
	node0.ops_p.set_init = gpio1_0_set_init;
	node0.ops_p.get_status = gpio1_0_get_status;
	// logc handle callback
	node0.lg_fuc = gpio1_0_callback_func;
	register_read_node(&node0);

	/*
	strncpy("CMD_CMD_",cmdstr,9);
	memset(node1.cmd_name,0,sizeof(node1.cmd_name));
	strncpy(cmdstr,node1.cmd_name,sizeof(cmdstr));
	node1.cmd_type = CMD_NORMAL;
	node1.node_type = GPIO_FOR_READ;   //  for check the GPIO status
	node1.ops_p.set_init = gpio1_0_set_init;
	register_read_node(&node1);

	
	strncpy("CMD_CMD_",cmdstr,9);
	memset(node2.cmd_name,0,sizeof(node2.cmd_name));
	strncpy(cmdstr,node2.cmd_name,sizeof(cmdstr));
	node2.cmd_type = CMD_NORMAL;
	node2.node_type = GPIO_FOR_SET;   //  for check the GPIO status
	node2.ops_p.set_init = gpio1_0_set_init;
	register_read_node(&node2);

	strncpy("CMD_CMD_",cmdstr,9);
	memset(node3.cmd_name,0,sizeof(node3.cmd_name));
	strncpy(cmdstr,node3.cmd_name,sizeof(cmdstr));
	node3.cmd_type = CMD_NORMAL;
	node2.node_type = GPIO_FOR_SET;   //  for check the GPIO status
	node3.ops_p.set_init = gpio1_0_set_init;
	register_read_node(&node3);
	*/

	Ret = com_gpio_init();
	if(Ret != 0){
		printf("com read node init Error");
		return -1;
	}

}

extern int COM_API_INT(int id)
{
	serialNode *tmp;
	serialNode *use;
	if(p_node_head == NULL){
		printf("There is no write node that woule set\n");
		return -1;
	}
	tmp = p_node_head;
	while(id != tmp->id)
	{
		if(tmp->next != NULL){
			tmp = tmp->next;
		}else{
			printf("Dit not find the node\n");
			return -1;
		}
	}
#ifdef RUN_WELL_DEBUG
	printf("Find the Node id:%d\n",tmp->id);
#endif
	use = tmp;
	use->cmd_type = CMD_ACTIVE;
	return 0;
}

extern int COM_API_STR(char cmd[])
{
	int len = strlen(cmd);		
	serialNode *tmp;
	serialNode *use;
	if(p_node_head == NULL){
		printf("There is no write node that woule set\n");
		return -1;
	}
	tmp = p_node_head;
	while(strncmp(cmd,tmp->cmd_name,strlen(cmd)) != 0)
	{
		if(tmp->next != NULL){
			tmp = tmp->next;
		}else{
			printf("Dit not find the node\n");
			return -1;
		}
	}
}


typedef struct{
	char head;
	BOOL b_run;
	int period;
	serialNode *p_reg_node;
	pthread_t  h_Thread;
	pthread_mutex_t h_Mutex;
	//pcallback pcbfuc;
	pcallback pcbfuc;
}HMYOBJ,*PHMYOBJ;

HMYOBJ theObj;

// now the below code wiould the base code for the implement for the thread
// and all the code should be static mode .
// because these code should protect not show in the other appplication

// 如果设置回调的函数就调用
static void NoticeHostEvent(int num,int m)
{
	if(theObj.pcbfuc)
		theObj.pcbfuc(num,m);
}

static void *work_thread_fuc(void* p)
{
	PHMYOBJ myobj = (PHMYOBJ)p;
	BOOL b_Run = FALSE;
//	theObj.p_reg_node = p_node_head;
	serialNode *tmp = p_node_head;
	theObj.b_run = b_Run;		
	for(;!theObj.b_run;)
	{
		int nodetype = tmp->node_type;
		int status_t;
		int Ret = 0;
		cmd_type_et cmd_type;
		tmp = p_node_head;
		// working thread code 
		// we handle the data from the obj of theObj.p_node_head.ops_d.rea
		while(tmp->next != NULL)
		{
			switch (nodetype)
			{
				case GPIO_FOR_READ:
					// 处理读取端口的代码
					// 端口状态发生变化后 相应其回调的处理函数
					if((tmp->b_busy == 0) && (tmp->ops_p.get_status(tmp) == 1)) // if it is not busy and the status is changed				
					{
						tmp->lg_fuc(tmp); // use the logical fuction to handdle and send the mesg
					}
					break;

				case GPIO_FOR_SET:
					// 处理设置消息代码
					// 当存在命令后，进入回调的处理函数
					if((tmp->b_busy == 1) || (tmp->cmd_type != CMD_NORMAL )) // if it is not busy and we get command order
					{
						tmp->lg_fuc(tmp);
					}
					break;

				default:
					break;
			}
			tmp=tmp->next;
		}
			usleep(theObj.period*1000);	
	}
}


//    now below code will be the API Functions;

int API_Init_Object()
{
	memset(&theObj,0,sizeof(theObj));
	theObj.p_reg_node = p_node_head;
	// the other struct init code ...
	
	//	pthread_mutexattr_destroy(&(theObj.h_Mutex));
	pthread_mutex_init(&(theObj.h_Mutex),NULL);
	pthread_create((pthread_t*)&(theObj.h_Thread),NULL,work_thread_fuc,&theObj);
}

//############################################BEGIN OF API #########################################

void GPIO_Init()
{
	COM_API_INIT();
}

void ALB_Open()
{
	COM_API_INT(1);
}

void ALB_Close()
{
	COM_API_INT(2);
}


void Siren_On()
{
	return 0;
}

void Siren_Off()
{

}

int GPIO_MonitorStart( void (*notice)(int, int), int period )
{
	API_setcallback(notice);
	theObj.period = period;
	API_Init_Object();
	return 0;
}

void GPIO_MonitorStop()
{
	
}

// #############################################END OF  API ########################################
#define STAT_EXIT TRUE

int API_TerminateOject()
{
     if(theObj.h_Thread)
     {
        theObj.b_run = STAT_EXIT;
        pthread_cancel(theObj.h_Thread);
        pthread_join((theObj.h_Thread),0);
     }
}

void API_setcallback( void *call_back_pointer)
{
	if(theObj.pcbfuc)
	{
		theObj.pcbfuc = call_back_pointer;
	}
}

// ************************* for the Test Application **********************
#include <termios.h>
static int ttysetraw(int fd, struct termios *tio)
{
	struct termios ttyios;
	if ( tcgetattr (fd, tio) != 0 )
		return -1;
	memcpy(&ttyios, tio, sizeof(ttyios) );
	ttyios.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP
                      |INLCR|IGNCR|ICRNL|IXON);
	ttyios.c_oflag &= ~(OPOST|OLCUC|ONLCR|OCRNL|ONOCR|ONLRET);
	ttyios.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
	ttyios.c_cflag &= ~(CSIZE|PARENB);
	ttyios.c_cflag |= CS8;
	ttyios.c_cc[VMIN] = 1;
	ttyios.c_cc[VTIME] = 0;
	return tcsetattr (fd, TCSANOW, &ttyios);
}

static int ttyrestore(int fd, struct termios *tio)
{
	return tcsetattr(fd, TCSANOW, tio);
}

int tty_ready( int fd )
{
	int	n=0;
 	fd_set	set;
	struct timeval tval = {0, 10000};		// 10 msec timed out

	FD_ZERO(& set);
	FD_SET(fd, &set);
	n = select( fd+1, &set, NULL, NULL, &tval );

	return n;
}

void di_notice_handle( int di_last, int di_now )
{
	printf("DI CHANGED - WAS: 0x%x, IS: 0x%x\r\n", di_last, di_now);
	switch (di_last)
	{
		case 0:
			printf("Get Message 1\n");
			break;
		case 1:
			printf("Get Message 2\n");
			break;
		case 2:
			printf("Get Message 3\n");
			break;
		case 3:
			printf("Get Message 4\n");
			break;
		case 4:
			printf("Get Message 5\n");
			break;
		case 5:
			printf("Get Message 6\n");
			break;
		default:
			printf("Get Message 7\n");
			break;
	}
}

void print_help()
{
	printf("keystoke operation:\r\n"
		"'a' - trigger siren (on)\r\n"
		"'s' - siren off\r\n"
		"'A' - turn on siren for N seconds (default is 3)\r\n"
		"'o' - ALB open\r\n"
		"'c' - ALB close\r\n"
		"'1'~'9' - set siren on period (in seconds). will be used for 'A'\r\n"
		"'h' - print help message.\r\n" 
		"'q' - quit\r\n"
		);
}
	
int main( int argc, char *const argv[] )
{
	int ch;
	int bQuit = 0;
	int nPeriod = 3;
	int	fd;
	struct termios tios_save;
	
	fd = 0;		// stdin
	ttysetraw(fd, &tios_save);

	GPIO_MonitorStart(di_notice_handle, 5);
	for(;!bQuit;)
	{
		if ( tty_ready(fd) && read(fd, &ch, 1)==1 )
		{
			ch &= 0xff;
			switch (ch)
			{
			case 'q':
				printf("Quit test program...\r\n");
				bQuit = 1;
				break;
			case 'h':
				print_help();
				break;
			case 'a':
				printf("siren on\r\n");
				//Siren_On(0);
				break;
			case 's':
				printf("siren off\r\n");
				//Siren_Off();
				break;
			case 'A':
				printf("siren on for %d seconds and auto off\r\n", nPeriod);
			//Siren_On(nPeriod);
				break;
			case 'o':
				printf("ALB open\r\n", nPeriod);
				ALB_Open();
				break;
			case 'c':
				printf("ALB close\r\n", nPeriod);
				ALB_Close();
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				nPeriod = ch - '0';
				printf("siren auto off period now is %d seconds.\r\n", nPeriod );
				break;
			default:
				printf("ch=%c (%d), ignored.\r\n", ch, ch );
				break;
			}
		}
	}	
	GPIO_MonitorStop();
	ttyrestore(fd, &tios_save);
	return 0;
}
