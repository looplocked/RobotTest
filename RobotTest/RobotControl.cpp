#include "RobotControl.h"

RobotControl::RobotControl(void)
{
	globalPose.resize(6);
}

RobotControl::~RobotControl(void)
{
	close();
}


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

	socketClient = socket(AF_INET, SOCK_STREAM, 0);
	addrSrv.sin_addr.S_un.S_addr = inet_addr("88.88.88.89");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(30003);
	bind(socketClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	int fail = connect(socketClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));//开始连接
	if (fail) {
		cout << "与服务端连接失败！程序将退出..." << endl;
		_getch();
		return;
	}

	thread pt(&RobotControl::poseReadThread, this);
	pt.detach();
}

vector<double> RobotControl::getJointAngle()
{
	while (true) {
		if (mtx.try_lock()) {
			vector<double> pose = globalPose;
			mtx.unlock();
			return pose;
		}
	}
}

void  RobotControl::movej(vector<double>& pose, double speed, double a)
{
	int len = sizeof(SOCKADDR);
	const char* sendBuf;
	string command = "movej([" + doubleToString((double)pose[0]) + ", " + doubleToString((double)pose[1]) + ", " + doubleToString(pose[2]) + ", " + doubleToString(pose[3]) + ", " + doubleToString(pose[4]) + ", " + doubleToString(pose[5]) + "], a=" + doubleToString(a) + ", v=" + doubleToString(speed) +  ")\n";
	//string command = "movej([" + doubleToString((double)pose[0]) + "," + doubleToString((double)pose[1]) + "," + doubleToString(pose[2]) + "," + doubleToString(pose[3]) + "," + doubleToString(pose[4]) + "," + doubleToString(pose[5]) + "]" + ")\n";
	sendBuf = command.c_str();
	int state = sendto(socketClient, sendBuf, strlen(sendBuf), 0, (SOCKADDR *)&addrSrv, len);
	//int state = send(socketClient, sendBuf, strlen(sendBuf) + 1, 0);
	printLog("move to " + command);
	printLog("the sent state is " + to_string(state) + ", " + "the sent data length is" + to_string(strlen(sendBuf)));
}

void RobotControl::Stop()
{
	int len = sizeof(SOCKADDR);
	char sendBuf[55] = { '\n' };
	sendBuf[0] = 'S';
	sendBuf[1] = 't';
	sendBuf[2] = '\n';
	sendto(socketClient, sendBuf, strlen(sendBuf), 0, (SOCKADDR *)&addrSrv, len);
}

bool RobotControl::isReachedJ(vector<double>& target, double threshold)
{
	std::vector<double> pose;
	pose = getJointAngle();
	return (abs(pose[0] - target[0]) < threshold) && (abs(pose[1] - target[1]) < threshold)
		&& (abs(pose[2] - target[2]) < threshold) && (abs(pose[3] - target[3]) < threshold)
		&& (abs(pose[4] - target[4]) < threshold) && (abs(pose[5] - target[5]) < threshold) ? true : false;
}

void RobotControl::poseReadThread()
{
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
	SOCKET socketClient = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("88.88.88.89");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(30003);
	bind(socketClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	int fail = connect(socketClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));//开始连接
	if (fail) {
		cout << "与服务端连接失败！程序将退出..." << endl;
		_getch();
		return;
	}

	char recvBuf[DATA_LENGTH];
	while (true) {
		int len = sizeof(SOCKADDR);
		recvfrom(socketClient, recvBuf, DATA_LENGTH, 0, (SOCKADDR *)&addrSrv, &len);
		int ActualJointVelocities_StartAddress = 252;        //joint速度地址
		BYTE temp[8];
		double TCP_vector[6]; //返回的6个参数

		for (int j = 0; j < 6; j++) //6个参数
		{
			for (int i = 0; i < 8; i++) //每个参数8个字节
			{
				temp[i] = *(recvBuf + ActualJointVelocities_StartAddress + 7 - i + 8 * j); //注意逆序

			}
			memcpy(&TCP_vector[j], temp, sizeof(TCP_vector[j]));//字节数组转double
		}
		if (mtx.try_lock()) {
			for (int i = 0; i < 6; i++)
				globalPose[i] = *(TCP_vector + i);
			mtx.unlock();
		}
	}
}


string RobotControl::doubleToString(double input)
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

void RobotControl::printLog(string log) {
	ofstream fout;
	SYSTEMTIME sys;
	::GetLocalTime(&sys);

	fout.open("log.txt", std::ios_base::in | std::ios_base::app);
	fout << "[" << sys.wHour << ":" << sys.wMinute << ":" << sys.wSecond << "." << sys.wMilliseconds << "]\t" << log << endl;
	fout.close();
}

void RobotControl::deleteLog() {
	ofstream fout;
	fout.open("log.txt", ofstream::out | ofstream::trunc);
	fout.close();
}
