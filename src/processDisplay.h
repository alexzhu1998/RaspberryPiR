#include <string.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <vector>

const char separator    = ' ';
const int nameWidth     = 15;
const int numWidth      = 8;
#define MAX_SENSOR      10

template <std::size_t R,std::size_t C> struct monitor_obj {
    const char* data[R][C];
    int num_sensor = 0;
};

int initiate_monitor();
bool check_monitor_created();
bool add_process(std::vector<std::string> new_data);
int display_monitor();