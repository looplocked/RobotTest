#pragma once
#ifndef ROBOTCONTROL_H
#define ROBOTCONTROL_H

#define DATA_LENGTH 1108
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <conio.h>
#include <fstream>
#include <thread>
#include <mutex>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::to_string;
using std::stringstream;
using std::ofstream;
using std::thread;
using std::mutex;


class RobotControl
{
public:
	RobotControl();
	~RobotControl();
	void initial();
	vector<double> getJointAngle();
	void movej(vector<double>& pose, double speed=1.05, double a=1.4);
	void Stop();
	bool isReachedJ(vector<double>& target, double threshold);
	void deleteLog();
	void printLog(string);

private:
	string doubleToString(double input);
	string intToString(int input);
	void poseReadThread();
	void close();

	vector<double> globalPose;
	mutex mtx;
	
	SOCKET socketClient;
	sockaddr_in addrSrv;
};


#endif