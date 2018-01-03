#include <MEL/Utility/Timer.hpp>
#include <MEL/Utility/mel_util.hpp>

using namespace mel;

int main() {
//enable_realtime();
   std::vector<int64> times(1000);
   Timer timer(milliseconds(1), Timer::Hybrid);
   for (auto i = 0; i < 1000; ++i) {
       timer.wait();
       times[i] = timer.get_elapsed_time().as_microseconds();
   }
   for (auto i = 1; i < 1000; ++i) {
       int64 delta = times[i] - times[i-1];
       print(delta);
   }
   print(times[999]);
   timer.restart();
   for (auto i = 0; i < 1000; ++i) {
       timer.wait();
       times[i] = timer.get_elapsed_time().as_microseconds();
   }
   for (auto i = 1; i < 1000; ++i) {
       int64 delta = times[i] - times[i-1];
       print(delta);
   }
   print(times[999]);
   return 0;
//disable_realtime();
}

