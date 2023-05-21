#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;
bool lightbuld_handler(string payload, int status){
    if( payload == "ON" && status == 0){
        //Prender bombilla aqui
        return true;
    }else if(payload == "OFF" && status == 0){
        return true;
    }else if(payload == "OFF" && status == 1){
        //Apago la bombilla
        return true;
    }else if(payload == "OFF" && status == 1){
        return true;
    }

    return false;

}