/**
 * Copyright (C) 2021 ADVACAM
 * @author     Pavel Hudecek <pavel.hudecek@advacam.com>
 * 
 * Examples of frame-based measuring
 */

/* Used API functions:

pxcGetLastError
pxcInitialize
pxcGetDevicesCount
pxcGetDeviceName
pxcGetDeviceChipID

pxcSetTimepix3Mode

pxcMeasureSingleFrameTpx3
pxcGetMeasuredFrameTpx3
pxcMeasureMultipleFrames
pxcMeasureMultipleFramesWithCallback

pxcRegisterAcqEvent
pxcMeasureContinuousTest
pxcAbortMeasurement

requires: pxcapi.h, pxcore.dll, minipix.dll, pixet.ini, link with pxcore.lib
*/

#include "pxcapi.h"
#include <cstring>

#define ERRMSG_BUFF_SIZE    512
#define ENTER_ON            true
#define ENTER_OFF           false

typedef struct { // structure for userData, that is using in mmfCallback and mcCallback
    unsigned    di;     // device index
    int         opm;    // operation mode
    unsigned    cnt=0;  // frames count (only for stop of pxcMeasureContinuous in mcCallback)
    char const* f;
} tMmfClbData;

// primary use to show function name, return code, last error message and optional enter
void printErrors(const char* fName, int rc, bool enter) { // ===================================
    char errorMsg[ERRMSG_BUFF_SIZE];
    pxcGetLastError(errorMsg, ERRMSG_BUFF_SIZE);
    if (errorMsg[0]>0) {
        printf("%s %d err: %s", fName, rc, errorMsg);
    } else {
        printf("%s %d err: ---", fName, rc);
    }
    if (enter) printf("\n");
}

// callback for pxcMeasureMultipleFramesWithCallbackTest, reused for other frame read+view
// (Tpx3 only, for other chips must use other pxcGetMeasuredFrame... and data formats)
void mmfCallback(intptr_t acqCount, intptr_t userData) { // ====================================
    int rc; // return codes
    tMmfClbData usrData = *((tMmfClbData*)userData);
    const unsigned cSize = 65536;
    unsigned short frameTotEvent[cSize];
    unsigned size = cSize;

    printf("mmfCallback acqCount=%lu, di=%d\n", (unsigned)acqCount, usrData.di);

    rc = pxcGetMeasuredFrame(usrData.di, (unsigned)acqCount - 1, frameTotEvent, &size);
    printErrors("pxcGetMeasuredFrame", rc, ENTER_OFF); printf(", size=%d\n", size);

    rc = pxcSaveMeasuredFrame(usrData.di, (unsigned)acqCount - 1, usrData.f);
    printErrors("pxcSaveMeasuredFrame-txt", rc, ENTER_ON);

    /* file extensions:
    #define PX_EXT_ASCII_FRAME          "txt"
    #define PX_EXT_BINARY_FRAME         "pbf"
    #define PX_EXT_MULTI_FRAME          "pmf"
    #define PX_EXT_BINARY_MULTI_FRAME   "bmf"
    #define PX_EXT_COMP_TPXSTREAM       "pcts"
    #define PX_EXT_TPX3_PIXELS          "t3p"
    #define PX_EXT_TPX3_PIXELS_ASCII    "t3pa"
    #define PX_EXT_TPX3_RAW_DATA        "t3r"
    #define PX_EXT_FRAME_DESC           "dsc"
    #define PX_EXT_INDEX                "idx"
    #define PX_EXT_CLUSTER_LOG          "clog"
    #define PX_EXT_PIXEL_LOG            "plog"
    #define PX_EXT_PNG                  "png"
    #define PX_EXT_PIXET_RAW_DATA       "prd"  */

    if (rc==0) {
        printf("Measured frame index %lu, ", (unsigned)acqCount - 1);
        printf("count %d\n", pxcGetMeasuredFrameCount(usrData.di));
    }
}

// test of pxcMeasureMultipleFramesWithCallback - starts the measure with callback and wait for all frames arrive
void pxcMeasureMultipleFramesWithCallbackTest(unsigned deviceIndex, unsigned frameCount, double frameTime, char const* outFile) { // =======================
    int rc; // return codes
    tMmfClbData usrData;

    usrData.di = deviceIndex;
    usrData.opm = PXC_TPX_MODE_TOT;
    usrData.f = outFile;
    rc = pxcSetTimepixMode(deviceIndex, usrData.opm);
    printErrors("pxcSetTimepixMode", rc, ENTER_ON);

    // Measure multiple frames and receive callback after each measured frame
    rc = pxcMeasureMultipleFramesWithCallback(deviceIndex, frameCount, frameTime, PXC_TRG_NO, mmfCallback, (intptr_t)&usrData);
    printErrors("pxcMeasureMultipleFramesWithCallback", rc, ENTER_ON);
}

int main (int argc, char const* argv[]) { // ###################################################
	int i = 0;
	for (i=0;i<argc;i++) {
		printf("%d = %s\n", i, argv[i]);
	}
	
	unsigned frameCount = atoi(argv[1]);
	double frameTime = atof(argv[2]);
	//char outFile[] = argv[3];
	
    // Initialize Pixet
    int rc = pxcInitialize();
    if (rc) {
        printf("Could not initialize Pixet:\n");
        printErrors("pxcInitialize", rc, ENTER_ON);
        return -1;
    }

    int connectedDevicesCount = pxcGetDevicesCount();
    printf("Connected devices: %d\n", connectedDevicesCount);

    if (connectedDevicesCount == 0) return pxcExit();

    for (unsigned devIdx = 0; (signed)devIdx < connectedDevicesCount; devIdx++){
        char deviceName[256];
        memset(deviceName, 0, 256);
        pxcGetDeviceName(devIdx, deviceName, 256);

        char chipID[256];
        memset(chipID, 0, 256);
        pxcGetDeviceChipID(devIdx, 0, chipID, 256);
        printf("Device %d: Name %s, (first ChipID: %s)\n", devIdx, deviceName, chipID);
    }
    printf("=================================================================\n");

    int deviceIndex = 0;
    int err = 0;
    
    const char fileConfig[] = "/home/pi/Documents/PixetPro/configs/H08-W0276_New.xml";
    printf(fileConfig);
    printf("\n");
    
    err = pxcLoadDeviceConfiguration(deviceIndex, fileConfig);
    printf("err %d \n", err);
    
    double bias = 80.0;
    double bias_check;
    
    err = pxcSetBias(deviceIndex, bias);
    err = pxcGetBias(deviceIndex, &bias_check);
    printf("bias: %lf \n", bias_check);
    
    pxcMeasureMultipleFramesWithCallbackTest(deviceIndex, frameCount, frameTime, argv[3]);
    printf("err %d \n", err);

    
    return pxcExit(); // Exit Pixet
}
