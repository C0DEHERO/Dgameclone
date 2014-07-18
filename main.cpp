#include <iostream>
#include <curses.h>

#include "dgameclone.h"
#include "user.h"
#include "game.h"

using namespace std;

int main(int argc, char **argv) {
    Dgamelaunch dgameclone;
    dgameclone.setOption("defterm", "xterm");

    dgameclone.init();
    while (true) {
        dgameclone.showMenu();
    }

    return 0;
}
