#pragma once

#include <obs.h>
#include <string>
#include <iostream>
#include <thread>
#include <util/config-file.h>
#include <util/dstr.h>
#include <util/platform.h>
#include <algorithm>
#include <sys/stat.h>
#include "nodeobs_api.h"
#include <map>
#include <mutex>

#include "nodeobs_audio_encoders.h"

#ifdef _WIN32
 
#ifdef _MSC_VER
#include <direct.h>
#define getcwd _getcwd
#endif 
#else
#include <unistd.h>
#endif

#define SIMPLE_ENCODER_X264                    "obs_x264"
#define SIMPLE_ENCODER_X264_LOWCPU             "obs_x264"
#define SIMPLE_ENCODER_QSV                     "obs_qsv11"
#define SIMPLE_ENCODER_NVENC                   "ffmpeg_nvenc"
#define SIMPLE_ENCODER_AMD                     "amd_amf_h264"

using namespace std;

class SignalInfo {
private: 
	std::string m_outputType;
	std::string m_signal;
	int m_code;
	std::string m_errorMessage;
public:

	SignalInfo() {};
	SignalInfo(std::string outputType, std::string signal) {
		m_outputType = outputType;
		m_signal = signal;
		m_code = 0;
		m_errorMessage = "";
	}
	std::string getOutputType(void) { return m_outputType; };
	std::string getSignal(void) { return m_signal; };

	int getCode(void) { return m_code; };
	void setCode(int code) { m_code = code; };
	std::string getErrorMessage(void) { return m_errorMessage; };
	void setErrorMessage(std::string errorMessage) { m_errorMessage = errorMessage; };
};

/*class ForeignWorker {
private:
	uv_async_t * async;

	static void AsyncClose(uv_handle_t *handle) {
		ForeignWorker *worker =
			reinterpret_cast<ForeignWorker*>(handle->data);

		worker->Destroy();
	}

	static NAUV_WORK_CB(AsyncCallback) {
		ForeignWorker *worker =
			reinterpret_cast<ForeignWorker*>(async->data);
		worker->Execute();
		uv_close(reinterpret_cast<uv_handle_t*>(async), ForeignWorker::AsyncClose);
	}

protected:
	Nan::Callback *callback;

	v8::Local<v8::Value> Call(int argc = 0, v8::Local<v8::Value> params[] = 0) {
		return callback->Call(argc, params);
	}

public:
	ForeignWorker(Nan::Callback *callback) {
		async = new uv_async_t;

		uv_async_init(
			uv_default_loop()
			, async
			, AsyncCallback
		);

		async->data = this;
		this->callback = callback;
	}

	void Send() {
		uv_async_send(async);
	}

	virtual void Execute() = 0;
	virtual void Destroy() {
		delete this;
	};

	virtual ~ForeignWorker() {
		delete async;
	}
};

class Worker : public ForeignWorker {
public:
	SignalInfo m_signalInfo;
	
	Worker(Nan::Callback *callback, SignalInfo signalInfo)
		: ForeignWorker(callback) {
		m_signalInfo = signalInfo;
	}

	virtual void Execute() {
		Isolate *isolate = v8::Isolate::GetCurrent();
		v8::Local<v8::Value> args[1];
		
		v8::Local<v8::Value> argv = v8::Object::New(isolate);
		argv->ToObject()->Set(String::NewFromUtf8(isolate, "type"), String::NewFromUtf8(isolate, m_signalInfo.getOutputType().c_str()));
		argv->ToObject()->Set(String::NewFromUtf8(isolate, "signal"), String::NewFromUtf8(isolate, m_signalInfo.getSignal().c_str()));
		argv->ToObject()->Set(String::NewFromUtf8(isolate, "code"), Number::New(isolate, m_signalInfo.getCode()));
		argv->ToObject()->Set(String::NewFromUtf8(isolate, "error"), String::NewFromUtf8(isolate, m_signalInfo.getErrorMessage().c_str()));
		args[0] = argv;

		Call(1, args);
	}

	virtual void Destroy() {
		delete this;
	}
};*/

class OBS_service
{
public:
	OBS_service();
	~OBS_service();

	static void OBS_service_resetAudioContext(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_resetVideoContext(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_createAudioEncoder(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_createVideoStreamingEncoder(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_createVideoRecordingEncoder(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_createService(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_createRecordingSettings(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_createStreamingOutput(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_createRecordingOutput(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_startStreaming(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_startRecording(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_stopStreaming(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_stopRecording(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_associateAudioAndVideoToTheCurrentStreamingContext(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_associateAudioAndVideoToTheCurrentRecordingContext(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_associateAudioAndVideoEncodersToTheCurrentStreamingOutput(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_associateAudioAndVideoEncodersToTheCurrentRecordingOutput(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_setServiceToTheStreamingOutput(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_setRecordingSettings(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_isStreamingOutputActive(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);
	static void OBS_service_connectOutputSignals(void* data, const int64_t id, const std::vector<IPC::Value>& args, std::vector<IPC::Value>& rval);

private:
	static obs_data_t* createRecordingSettings(void);
	static bool startStreaming(void);
	static bool startRecording(void);
	static void stopStreaming(bool forceStop);
	static void stopRecording(void);
	static void setRecordingSettings(void);

	static void LoadRecordingPreset_h264(const char *encoder);
	static void LoadRecordingPreset_Lossless(void);
	// static void LoadRecordingPreset(void);

	static void UpdateRecordingSettings_x264_crf(int crf);
	static void UpdateRecordingSettings_qsv11(int crf);
	static void UpdateRecordingSettings_nvenc(int cqp);
	static void UpdateStreamingSettings_amd(obs_data_t *settings, int bitrate);
	static void UpdateRecordingSettings_amd_cqp(int cqp);
	static void UpdateRecordingSettings(void);


public:
	// Service
	static void 				createService();
	static obs_service_t* 		getService(void);
	static void 				setService(obs_service_t* newService);
	static void 				saveService(void);
	static void 				updateService(void);
	static void 				setServiceToTheStreamingOutput(void);

	// Encoders
	static void					createAudioEncoder(obs_encoder_t** audioEncoder);
	static void 				createVideoStreamingEncoder();
	static void 				createVideoRecordingEncoder();
	static obs_encoder_t* 		getStreamingEncoder(void);
	static void 				setStreamingEncoder(obs_encoder_t* encoder);
	static obs_encoder_t*		getRecordingEncoder(void);
	static void 				setRecordingEncoder(obs_encoder_t* encoder);
	static obs_encoder_t*		getAudioStreamingEncoder(void);
	static void 				setAudioStreamingEncoder(obs_encoder_t* encoder);
	static obs_encoder_t*		getAudioRecordingEncoder(void);
	static void 				setAudioRecordingEncoder(obs_encoder_t* encoder);

	// Outputs
	static void 				createStreamingOutput(void);
	static void 				createRecordingOutput(void);
	static obs_output_t*		getStreamingOutput(void);
	static void 				setStreamingOutput(obs_output_t* output);
	static obs_output_t*		getRecordingOutput(void);
	static void 				setRecordingOutput(obs_output_t* output);

	// Update settings
	static void updateStreamSettings(void);
	static void updateRecordSettings(void);

	// Update video encoders
	static void updateVideoStreamingEncoder(void);
	static void updateVideoRecordingEncoder(void);

	// Update outputs
	static void updateStreamingOutput(void);
	static void updateRecordingOutput(void);
	static void updateAdvancedRecordingOutput(void);
	static void UpdateFFmpegOutput(void);

	static std::string GetDefaultVideoSavePath(void);

	static bool isStreamingOutputActive(void);

	// Reset contexts
	static bool resetAudioContext(void);
	static bool resetVideoContext(const char* outputType);
	
	static void associateAudioAndVideoToTheCurrentStreamingContext(void);
	static void associateAudioAndVideoToTheCurrentRecordingContext(void);
	static void associateAudioAndVideoEncodersToTheCurrentStreamingOutput(void);
	static void associateAudioAndVideoEncodersToTheCurrentRecordingOutput(void);

	static int GetAudioBitrate(void);

	// Output signals
	static void connectOutputSignals(void);
	static void JSCallbackOutputSignal(void *data, calldata_t *);
};