#include "M5_EzData.h"

char * ezdatatoken = "";

int setupFinished = 0;
// void setupEzData(const char * ssid, const char * password, const char * token) {
//   ezdatatoken = const_cast<char*>(token);
void setupEzData(char * ssid, char * password, char * token) {
  ezdatatoken = token;
  if(setupWifi(ssid,password)){
    setupFinished = 1;
  }
}

void ezSetData(char * key, int i) {
  setData(ezdatatoken, key, i);
}

void ezAddToList(char * key, int i){
  addToList(ezdatatoken, key, i);
}