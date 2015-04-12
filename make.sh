g++ ./src/*.cc -o ./sprt -std=c++11 -Wall -Wextra -DNDEBUG -Ofast -flto -lpthread
strip ./sprt
