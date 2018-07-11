//Copyright (c) Brigido Rodriguez, all rights reserved.
#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <memory>
#include <string>
#include <iostream>
#include <assert.h>

using namespace std;
typedef chrono::high_resolution_clock Clock;

struct timer_action
	{
	private:
	chrono::high_resolution_clock::time_point t1_;
	string action_;
	string file_;
	bool do_msg_;
	public:
	timer_action(string a, string f):
		t1_(Clock::now()),
		action_(a),
		file_(f),
		do_msg_(false)
		{
		}
	void action_success()
		{
		do_msg_=true;
		}
	~timer_action()
		{
		if(do_msg_)
			{
			chrono::high_resolution_clock::time_point t2 = Clock::now();
			cout << action_ + " " + file_ + " @ " << (float)(chrono::duration_cast<chrono::milliseconds>(t2 - t1_).count())*0.001f << " sec" << endl;
			}
		}
	};

#endif