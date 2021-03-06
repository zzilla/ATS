/**
 *  xm_sdk.c
 *
 */


#include "sdk_layer.h"
#include "sdk_log.h"

// XiongMai H264 header
#include "H264Play.h"
#include "netsdk.h"

#ifdef __cplusplus
extern "C"
{
#endif


#ifdef __OSA_OS_LINUX__
#error "This library only support Windows yet!"
#endif


// private variables
static osa_int32_t      loginID = -1;
static osa_int32_t      playID = -1;

// private function
static osa_err_t    xmSDK_Init(void);
static void         xmSDK_Exit(void);
static osa_err_t    xmSDK_LoginDevice(osa_char_t *ip, osa_uint32_t port, osa_char_t *user, osa_char_t *passwd);
static osa_err_t    xmSDK_PlayLiveMedia(osa_uint32_t playNum, osa_uint32_t chNum, void *priv);

static void         _realDataCB(long h, long dtype, char *out_buf, long size, long priv);


// XiongMai (HangZhou China) Co. Ltd. SDK
static SDK_Layer    xm_sdk;


SDK_Layer   *SDK_PluginGet()
{
    xm_sdk.nameSpace = "XiongMai_SDK";
    xm_sdk.info = "XiongMai (HangZhou china) Co. Ltd. All right reserved";
    xm_sdk.version = 1;
    osa_list_init(&xm_sdk.list);
    xm_sdk.SDK_Init = xmSDK_Init;
    xm_sdk.SDK_Exit = xmSDK_Exit;
    xm_sdk.SDK_LoginDevice = xmSDK_LoginDevice;
    xm_sdk.SDK_PlayLiveMedia = xmSDK_PlayLiveMedia;

    return &xm_sdk;
}



//-------------------------------------priv---------------------
static osa_err_t   xmSDK_Init(void)
{
    SDK_LogInfo("initialize Xiong Mai SDK!\n");

    return OSA_ERR_OK;
}


static void        xmSDK_Exit(void)
{
    SDK_LogInfo("finalize Xiong Mai SDK!\n");
}

static osa_err_t    xmSDK_LoginDevice(osa_char_t *ip, osa_uint32_t port, osa_char_t *user, osa_char_t *passwd)
{
    if (loginID > 0)
    {
        return OSA_ERR_OK;
    }

    H264_DVR_DEVICEINFO devInfo;
    int nerror = 0;

    if ((loginID = H264_DVR_Login(ip, port, user, passwd, &devInfo, &nerror)) <= 0)
    {
        SDK_LogError("Failed to login device: ip(%s), port(%d), user(%s), passwd(%s)!\n", ip, port, user, passwd);
        return OSA_ERR_ERR;
    }

    return OSA_ERR_OK;
}


static osa_err_t    xmSDK_PlayLiveMedia(osa_uint32_t playNum, osa_uint32_t chNum, void *priv)
{
    if (loginID < 0)
    {
        SDK_LogError("Please login device first!\n");
        return OSA_ERR_ERR;
    }

    if (H264_PLAY_InitDDrawDevice() != TRUE)
    {
        SDK_LogError("Failed to initialize draw device!\n");
        return OSA_ERR_ERR;
    }

    if (H264_PLAY_OpenStream(playNum, NULL, 0, SOURCE_BUF_MIN*50) != TRUE)
    {
        SDK_LogError("Failed to open stream!\n");
        return OSA_ERR_ERR;
    }

    if (H264_PLAY_SetStreamOpenMode(playNum, STREAME_REALTIME) != TRUE)
    {
        SDK_LogError("Failed to set stream mode!\n");
        return OSA_ERR_ERR;
    }

    if (H264_PLAY_Play(playNum, (HWND)priv) != TRUE)
    {
        SDK_LogError("Failed to play stream!\n");
        return OSA_ERR_ERR;
    }

    H264_DVR_CLIENTINFO clientInfo;
    clientInfo.nStream = 0;
    clientInfo.nMode = 0;
    clientInfo.nChannel = chNum;

    if ((playID = H264_DVR_RealPlay(loginID, &clientInfo)) == 0)
    {
        SDK_LogError("Failed to play real-time stream!\n");
        return OSA_ERR_ERR;
    }

    H264_DVR_SetRealDataCallBack(playID, (fRealDataCallBack)_realDataCB, 0);

    return OSA_ERR_OK;
}

static void _realDataCB(long h, long dtype, char *out_buf, long size, long priv)
{
    // 只有一个通道，所以第一个参数为1
    H264_PLAY_InputData(1, (PBYTE)out_buf, size);
}



#ifdef __cplusplus
}
#endif
