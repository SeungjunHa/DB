#include "types.h"
#include "stat.h"
#include "user.h"
#include "memlayout.h"

int
main(int argc, char **argv)
{
            // stack growth test
            int arr[1]={0};
                    if(0)printf(1,"%d",arr[0]);
                            arr[-10001]=0;
                                    exit();
}
