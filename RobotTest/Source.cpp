#include "RobotControl.h"
#include <sstream>
#include <vector>
#include <string>
#include <iterator>
#include <iostream>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

int main()
{
	RobotControl robot;
	robot.initial();
	while (true)
	{
		vector<float> pose;
		robot.getJointAngle(pose);
		stringstream posestream;
		copy(pose.begin(), pose.end(), ostream_iterator<float>(posestream, ", "));
		string posestr = posestream.str();
		cout << posestr << endl;
		_sleep(300);
	}
}