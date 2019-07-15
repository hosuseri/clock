#include <windows.h>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <chrono>

const int helz = 50;
const int delta = 2000;

void proc(void);
void _osc(void);
void _divn(void);

LARGE_INTEGER systick;
LONGLONG proc_freq, proc_acc;
unsigned long tick = 0;

std::condition_variable cv_delta;
std::condition_variable cv_divn;
std::condition_variable cv_proc;

int main()
{
    LARGE_INTEGER freq;
    ::QueryPerformanceFrequency(&freq);
    proc_freq = freq.QuadPart / helz;
    std::cerr
	<< std::setw(20) << freq.QuadPart
	<< std::setw(20) << proc_freq
	<< std::endl;

    std::thread divn(_divn);
    std::thread osc(_osc);
    osc.detach();
    divn.detach();

    std::cerr << std::endl;

    std::mutex mtx;
    std::unique_lock<std::mutex> lk(mtx);
    proc();
    for (;;) {
	cv_proc.wait(lk);
	proc();
    }
    return 0;
}

void proc(void)
{
    std::cerr << std::setw(12) << tick << '\r';
}

void _osc(void)
{
    BOOL r = ::QueryPerformanceCounter(&systick);
    proc_acc = systick.QuadPart;
    if (!r)
	return;

    std::mutex mtx;
    std::unique_lock<std::mutex> lk(mtx);
    for (;;) {
	cv_delta.wait_for(lk, std::chrono::microseconds(delta));
	r = ::QueryPerformanceCounter(&systick);
	if (!r)
	    break;
	cv_divn.notify_one();
    }
}

void _divn(void)
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lk(mtx);
    for (;;) {
	cv_divn.wait(lk);
	if (systick.QuadPart - proc_acc < proc_freq)
	    continue;
	cv_proc.notify_one();
	tick++;
	proc_acc += proc_freq;
    }
}
