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
	bool forward = true;
	vector<double> pose1{ 3.327548, - 2.121084, - 1.349550, - 1.186976, 1.532444, 0.921528 };
	vector<double> pose2{ 3.357548, -2.121084, -1.349550, -1.186976, 1.532444, 0.921528 };
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
		Sleep(1000);
	}
}