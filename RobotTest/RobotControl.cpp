#include "RobotControl.h"

RobotControl::RobotControl(void)
{

}

RobotControl::~RobotControl(void)
{
	close();
	// system("taskkill /f /im ConnectTest.exe");
}

//DWORD WINAPI threadFun(LPVOID lpParameter)
//{
//	system(".\\ConnectTest.exe");
//	return 0;
//}

void  RobotControl::initial()
{
	//加载套接字库
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
		return;
	if ((LOBYTE(wsaData.wVersion) != 1) || HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup(); return;
	}
	//HANDLE hThread1;
	//hThread1 = CreateThread(NULL, 0, threadFun, NULL, 0, NULL);

	//客户端初始化
	socketClient = socket(AF_INET, SOCK_DGRAM, 0);
	addrSrv.sin_addr.S_un.S_addr = inet_addr("88.88.88.89");
	//addrSrv.sin_addr.S_un.S_addr = inet_pton("88.88.88.89");//ip地址
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(30003);
	bind(socketClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	int fail = connect(socketClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));//开始连接
	if (fail) {
		cout << "与服务端连接失败！程序将退出..." << endl;
		_getch();
		return;
	}
}


double*  RobotControl::Tool_vector_actual(char* data_recieved)
{
	int Tool_vector_actual_StartAddress = 444;  //机器人末端位姿起始地址
	BYTE temp[8];
	double TCP_vector[6]; //返回的6个参数

	for (int j = 0; j < 6; j++) //6个参数
	{
		for (int i = 0; i < 8; i++) //每个参数8个字节
		{
			temp[i] = *(data_recieved + Tool_vector_actual_StartAddress + 7 - i + 8 * j); //注意逆序

		}
		memcpy(&TCP_vector[j], temp, sizeof(TCP_vector[j]));//字节数组转double
	}
	return TCP_vector;
}

//读取机器人关节速度
double*  RobotControl::JointSpeed(char* data_recieved)
{
	int ActualJointVelocities_StartAddress = 300;        //joint速度地址
	BYTE temp[8];
	double TCP_vector[6]; //返回的6个参数

	for (int j = 0; j < 6; j++) //6个参数
	{
		for (int i = 0; i < 8; i++) //每个参数8个字节
		{
			temp[i] = *(data_recieved + ActualJointVelocities_StartAddress + 7 - i + 8 * j); //注意逆序

		}
		memcpy(&TCP_vector[j], temp, sizeof(TCP_vector[j]));//字节数组转double
	}
	return TCP_vector;
}

//读取机器人关节速度
double*  RobotControl::JointAngle(char* data_recieved)
{
	int ActualJointVelocities_StartAddress = 252;        //joint速度地址
	BYTE temp[8];
	double TCP_vector[6]; //返回的6个参数

	for (int j = 0; j < 6; j++) //6个参数
	{
		for (int i = 0; i < 8; i++) //每个参数8个字节
		{
			temp[i] = *(data_recieved + ActualJointVelocities_StartAddress + 7 - i + 8 * j); //注意逆序

		}
		memcpy(&TCP_vector[j], temp, sizeof(TCP_vector[j]));//字节数组转double
	}
	return TCP_vector;
}

void RobotControl::getEndEffectorPose(std::vector<float>& result)
{
	result.clear();
	result.resize(6);
	int len = sizeof(SOCKADDR);
	recvfrom(socketClient, recvBuf, 1044, 0, (SOCKADDR *)&addrSrv, &len);
	char sendBuf[55] = { '0' };
	//机器人末端位姿
	for (int i = 0; i < 6; i++)
		result[i] = *(Tool_vector_actual(recvBuf) + i);
	sendto(socketClient, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR *)&addrSrv, len);
}

void RobotControl::getJointAngle(std::vector<float>& result)
{
	result.clear();
	result.resize(6);
	int len = sizeof(SOCKADDR);
	recvfrom(socketClient, recvBuf, 1044, 0, (SOCKADDR *)&addrSrv, &len);
	char sendBuf[55] = { '0' };
	//机器人末端位姿
	for (int i = 0; i < 6; i++)
		result[i] = *(JointAngle(recvBuf) + i);
	sendto(socketClient, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR *)&addrSrv, len);
}

void  RobotControl::movel(vector<float>& pose, float speed, float a)
{
	int len = sizeof(SOCKADDR);
	const char* sendBuf;
	string command = "movel(p[" + floatToString((float)pose[0]) + "," + floatToString((float)pose[1]) + "," + floatToString(pose[2]) + "," + floatToString(pose[3]) + "," + floatToString(pose[4]) + "," + floatToString(pose[5]) + "],a = " + floatToString(a) + ", v = " + floatToString(speed) + ")\n";
	sendBuf = command.c_str();
	sendto(socketClient, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR *)&addrSrv, len);
}

void  RobotControl::movej(vector<float>& pose, float speed, float a)
{
	int len = sizeof(SOCKADDR);
	const char* sendBuf;
	string command = "movej([" + floatToString((float)pose[0]) + "," + floatToString((float)pose[1]) + "," + floatToString(pose[2]) + "," + floatToString(pose[3]) + "," + floatToString(pose[4]) + "," + floatToString(pose[5]) + "],a = " + floatToString(a) + ", v = " + floatToString(speed) + ")\n";
	sendBuf = command.c_str();
	sendto(socketClient, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR *)&addrSrv, len);
}

void  RobotControl::IOControl(bool openFlag)
{
	int len = sizeof(SOCKADDR);
	char sendBuf[55] = { '\n' };
	if (openFlag)
	{
		sendBuf[0] = 'O';
		sendBuf[1] = 'n';
		sendBuf[2] = '\n';
		sendto(socketClient, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR *)&addrSrv, len);
	}
	else
	{
		sendBuf[0] = 'O';
		sendBuf[1] = 'f';
		sendBuf[2] = '\n';
		sendto(socketClient, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR *)&addrSrv, len);
	}
}

void RobotControl::DigitalOut(int port, bool openFlag)
{
	int len = sizeof(SOCKADDR);
	const char* sendBuf;
	string command_On = "set_digital_out(" + intToString(port) + ", True)\n";
	string command_Off = "set_digital_out(" + intToString(port) + ", False)\n";
	if (openFlag)
		sendBuf = command_On.c_str();
	else sendBuf = command_Off.c_str();
	sendto(socketClient, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR *)&addrSrv, len);
}

void RobotControl::Stop()
{
	int len = sizeof(SOCKADDR);
	char sendBuf[55] = { '\n' };
	sendBuf[0] = 'S';
	sendBuf[1] = 't';
	sendBuf[2] = '\n';
	sendto(socketClient, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR *)&addrSrv, len);
}

void RobotControl::moveDownAndUp(vector<float>& pose, float dist, float speed)
{
	string deltaPose = "p[0,0," + floatToString(dist) + ",0,0,0]";
	string command = "movel(pose_trans(p[" + floatToString((float)pose[0]) + "," + floatToString((float)pose[1]) + "," + floatToString(pose[2]) + "," + floatToString(pose[3]) + "," + floatToString(pose[4]) + "," + floatToString(pose[5]) + "]," + deltaPose + "),a = 1.2, v = " + floatToString(speed) + ")\n";
	const char* sendBuf;
	sendBuf = command.c_str();
	int len = sizeof(SOCKADDR);
	sendto(socketClient, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR *)&addrSrv, len);
}

void RobotControl::moveX(vector<float>& pose, float dist, float speed)
{
	string deltaPose = "p[0," + floatToString(dist) + ",0,0,0,0]";
	string command = "movel(pose_trans(p[" + floatToString((float)pose[0]) + "," + floatToString((float)pose[1]) + "," + floatToString(pose[2]) + "," + floatToString(pose[3]) + "," + floatToString(pose[4]) + "," + floatToString(pose[5]) + "]," + deltaPose + "),a = 1.2, v = " + floatToString(speed) + ")\n";
	const char* sendBuf;
	sendBuf = command.c_str();
	int len = sizeof(SOCKADDR);
	sendto(socketClient, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR *)&addrSrv, len);
}

bool RobotControl::IsMoving(float threshNum)
{
	int len = sizeof(SOCKADDR);
	char sendBuf[55] = { '0' };
	recvfrom(socketClient, recvBuf, 1044, 0, (SOCKADDR *)&addrSrv, &len);
	for (int i = 0; i < 6; i++)
	{
		if (abs(*(JointSpeed(recvBuf) + i)) > threshNum)
		{
			sendto(socketClient, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR *)&addrSrv, len);
			return true;
		}
	}
	sendto(socketClient, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR *)&addrSrv, len);
	return false;
}

bool RobotControl::isReachedT(vector<float>& target, float threshold_P, float threshold_O)
{
	std::vector<float> pose;
	getEndEffectorPose(pose);
	return (abs(pose[0] - target[0]) < threshold_P) && (abs(pose[1] - target[1]) < threshold_P)
		&& (abs(pose[2] - target[2]) < threshold_P) && (abs(pose[3] - target[3]) < threshold_O)
		&& (abs(pose[4] - target[4]) < threshold_O) && (abs(pose[5] - target[5]) < threshold_O) ? true : false;
}

bool RobotControl::isReachedJ(vector<float>& target, float threshold)
{
	std::vector<float> pose;
	getJointAngle(pose);
	return (abs(pose[0] - target[0]) < threshold) && (abs(pose[1] - target[1]) < threshold)
		&& (abs(pose[2] - target[2]) < threshold) && (abs(pose[3] - target[3]) < threshold)
		&& (abs(pose[4] - target[4]) < threshold) && (abs(pose[5] - target[5]) < threshold) ? true : false;
}

string RobotControl::floatToString(float input)
{
	std::stringstream ss;
	ss << input;
	return ss.str();
}

string RobotControl::intToString(int input)
{
	std::stringstream ss;
	ss << input;
	return ss.str();
}
void RobotControl::close()
{
	closesocket(socketClient);
}

void RobotControl::route(vector<vector<float>> Route_points, float speed, float ac)
{
	int len = sizeof(SOCKADDR);
	const char* sendBuf;
	string command = "Ad";
	for (int i = 0; i < Route_points.size(); i++)
	{
		if (i < Route_points.size() - 1)
		{
			command += "movel(p[" + floatToString((float)Route_points[i][0]) + "," + floatToString((float)Route_points[i][1]) + "," + floatToString(Route_points[i][2]) + ","
				+ floatToString(Route_points[i][3]) + "," + floatToString(Route_points[i][4]) + "," + floatToString(Route_points[i][5]) + "],a = " + floatToString(ac) + ",v = " + floatToString(speed) + ",r = 0.025)f";
		}
		else
			command += "movel(p[" + floatToString((float)Route_points[i][0]) + "," + floatToString((float)Route_points[i][1]) + "," + floatToString(Route_points[i][2]) + "," +
			floatToString(Route_points[i][3]) + "," + floatToString(Route_points[i][4]) + "," + floatToString(Route_points[i][5]) + "],a = " + floatToString(ac) + ",v = " + floatToString(speed) + ",r = 0.025)f\n";
	}
	sendBuf = command.c_str();
	sendto(socketClient, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR *)&addrSrv, len);
}