#include "RobotControl.h"
#include <sstream>
#include <vector>
#include <string>
#include <iterator>
#include <iostream>
#include <fstream>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

int main()
{
	RobotControl robot;
	robot.deleteLog();
	robot.initial();
	Sleep(100);
	bool forward = false;
	vector<double> pose1{ 3.22, -2.12, -1.34, -1.18, 1.53, 0.92 };
	vector<double> pose2{ 3.39, -2.12, -1.34, -1.18, 1.53, 0.92 };
	vector<double> pose;
	while (true)
	{
		/*if (forward) {
			robot.movej(pose1, 0.5, 1.4);
			forward = false;
		}
		else if(!forward) {
			robot.movej(pose2, 0.5, 1.4);
			forward = true;
		}*/
		//Sleep(200);
		robot.printLog("tic");
		pose = robot.getJointAngle();
		robot.printLog("toc");
		stringstream posestream;
		copy(pose.begin(), pose.end(), ostream_iterator<float>(posestream, ", "));
		string posestr = posestream.str();
		cout << posestr << endl;
		
		
	}
	return 0;
}