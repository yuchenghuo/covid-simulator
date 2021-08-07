//
// Created by Yucheng Huo on 8/7/21.
//

#include <string>
#include <getopt.h>

class simulator {
private:

public:
  void get_options(int argc, char **argv) {
    int option_index = 0, option;

    struct option LongOpts[] ={
            { nullptr   , 0         , nullptr , '\0' }};

    while ((option = getopt_long(argc, argv, "", LongOpts, &option_index)) != -1) {
      switch (option) {
        default:
          break;
      } // end switch statement
    } // end while loop
  } // end get_options()
};

int main(int argc, char** argv) {
  simulator covid;
  covid.get_options(argc, argv);


  return 0;
}
