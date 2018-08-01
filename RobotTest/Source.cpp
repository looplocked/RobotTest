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
	vector<float> pose1{ float(3.22), float(-2.12), float(-1.34), float(-1.18), float(1.53), float(0.92) };
	vector<float> pose2{ float(3.39), float(-2.12), float(-1.34), float(-1.18), float(1.53), float(0.92) };
	vector<float> pose;
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
		robot.getJointAngle(pose);
		robot.printLog("toc");
		stringstream posestream;
		copy(pose.begin(), pose.end(), ostream_iterator<float>(posestream, ", "));
		string posestr = posestream.str();
		cout << posestr << endl;
		
		
	}
	return 0;
}