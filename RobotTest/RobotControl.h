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


using namespace std;


class RobotControl
{
public:
	RobotControl();
	~RobotControl();
	void initial();
	double* Tool_vector_actual(char* data_received);
	double* JointSpeed(char* data_recieved);
	double*  JointAngle(char* data_recieved);
	void getEndEffectorPose(std::vector<float>& result);
	void getJointAngle(std::vector<float>& result);
	void movel(vector<float>& pose, float speed, float a);
	void movej(vector<float>& pose, float speed=1.05, float a=1.4);
	void IOControl(bool openFlag);
	void DigitalOut(int port, bool openFlag);
	void Stop();
	void moveDownAndUp(vector<float>& pose, float dist, float speed);
	void moveX(vector<float>& pose, float dist, float speed);
	bool IsMoving(float threshNum);
	bool isReachedT(vector<float>& target, float threshold_P, float threshold_O);
	bool isReachedJ(vector<float>& target, float threshold);
	void deleteLog();
	void printLog(string);

private:
	string floatToString(float input);
	string intToString(int input);
	void close();
	void route(vector<vector<float>> Route_points, float speed, float ac);
	
	SOCKET socketClient;
	sockaddr_in addrSrv;
	char recvBuf[DATA_LENGTH];
};


#endif