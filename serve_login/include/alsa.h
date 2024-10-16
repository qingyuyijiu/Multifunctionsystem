#ifndef __ALSA__H_
#define __ALSA__H_

#define SAMPLE_RATE_16K (16000)
#define SAMPLE_RATE_8K (8000)
#define MAX_GRAMMARID_LEN (32)
#define MAX_PARAMS_LEN (1024)

typedef struct _UserData
{
  int build_fini;                     // 标识语法构建是否完成
  int update_fini;                    // 标识更新词典是否完成
  int errcode;                        // 记录语法构建或更新词典回调错误码
  char grammar_id[MAX_GRAMMARID_LEN]; // 保存语法构建返回的语法ID
} UserData;

const char *get_audio_file(void);    // 选择进行离线语法识别的语音文件
int build_grammar(UserData *udata);  // 构建离线识别语法网络
int update_lexicon(UserData *udata); // 更新离线识别语法词典
int run_asr(UserData *udata);        // 进行离线语法识别
void *alsa_init(void *arg);          // 初始化离线语音

#endif