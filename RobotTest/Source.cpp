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
	bool forward = true;
	vector<float> pose1{ float(2.32), float(-2.12), float(-1.34), float(-1.18), float(1.53), float(0.92) };
	vector<float> pose2{ float(3.39), float(-2.12), float(-1.34), float(-1.18), float(1.53), float(0.92) };
	while (true)
	{
		vector<float> pose;
		
		robot.getJointAngle(pose);
		if (forward) {
			robot.movej(pose1);
			forward = false;
		}
		else {
			robot.movej(pose2);
			forward = true;
		}
		stringstream posestream;
		copy(pose.begin(), pose.end(), ostream_iterator<float>(posestream, ", "));
		string posestr = posestream.str();
		cout << posestr << endl;
		Sleep(3000);
		//break;
	}
}