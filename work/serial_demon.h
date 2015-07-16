/*
 * =====================================================================================
 *
 *       Filename:  serial_demon.h
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


#ifndef __SERIAL_THREAD_
#define __SERIAL_THREAD_

#ifdef __cplusplus 
extern "C" {
#endif



// 端口状态检测返回，若端口无变化，返回0，若有变化则返回相应的端口消息号
#define GPIO_NORMAL_VALUE 	0  
// 定义的发送消息端口号
#define GPIO_MSG_1_1_ON		1
#define GPIO_MSG_1_1_OFF	2
#define GPIO_MSG_1_2_ON		3
#define GPIO_MSG_1_2_OFF	4

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


typedef unsigned long   DWORD;

// 与延时相关的逻辑处理代码，如判断GPIO状态（消除电流扰动）
// 若GPIO用来放按键，按键的处理，短按，长按 ，GPIO输出状态的处理，
// 如输出延时一段时间等操做，这里面最好用的就是状态机制

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
	
	// 记录寄存器当前状态
	int reg_value;

	// 记录当前时间
	DWORD lastTickCount;
	// 用于表示是否发送消息 该状态可能后期被去除
	int b_send;

	
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









void GPIO_Init();
int GPIO_MonitorStart( void (*notice)(int, int), int period );
void ALB_Open();
void ALB_Close();
void Siren_On();
void Siren_Off();
void GPIO_MonitorStop();

#ifdef __cplusplus 
}
#endif

#endif 


