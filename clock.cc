#include <iostream>
#include <iomanip>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <chrono>

using namespace std::chrono;

const int helz = 50;
const int freq = 1000000000;
const nanoseconds delta(2000000);

void proc(void);
void _osc(void);
void _divn(void);

time_point<steady_clock, nanoseconds> systick, proc_acc;
nanoseconds proc_freq;
unsigned long tick = 0;

std::condition_variable cv_delta;
std::condition_variable cv_divn;
std::condition_variable cv_proc;

int main()
{
    proc_freq = nanoseconds(freq / helz);
    std::cerr
	<< std::setw(20) << freq
	<< std::setw(20) << proc_freq.count()
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
    systick = steady_clock::now();
    proc_acc = systick;

    std::mutex mtx;
    std::unique_lock<std::mutex> lk(mtx);
    for (;;) {
	cv_delta.wait_for(lk, delta);
	cv_divn.notify_one();
    }
}

void _divn(void)
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lk(mtx);
    for (;;) {
	cv_divn.wait(lk);
	systick = steady_clock::now();
	if (systick - proc_acc < proc_freq)
	    continue;
	cv_proc.notify_one();
	tick++;
	proc_acc += proc_freq;
    }
}
