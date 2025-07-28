// DataProc.c
#include "String.h"
#include "DataProc.h"
//#include "DP_GPS.h"
//#include "DP_Clock.h"

SystemConfig sysConfig;
SystemData sysData;
void DataProc_Init(void) {
    DP_InitDefaultData();
	DP_SysConfig_Init();
    // DP_GPS_Init();
    // DP_Clock_Init();
}

