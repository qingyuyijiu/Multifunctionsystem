#########################################################################
# File Name: make.sh
# Author: penglei
# mail: 8888888888@qq.com
# Created Time: 2024年07月19日 星期五 10时44分53秒
#########################################################################
#!/bin/bash


make clean;make


export LD_LIBRARY_PATH=$(pwd)/../LIB/lib


echo $LD_LIBRARY_PATH

