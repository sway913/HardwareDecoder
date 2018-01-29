#ifndef HWDECODER_H
#define HWDECODER_H

#include <QObject>
#include <QFile>
#include <QFuture>

#include "videosource.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class HWDecoder: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* source READ getPlayer NOTIFY playerChanged)
public:
    static enum AVPixelFormat getFormat(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts);

    HWDecoder(QObject * parent = nullptr);
    ~HWDecoder();

    bool init(AVCodecParameters* codecParameters);
    bool open();
    void close();
    void flush();

    Q_INVOKABLE void decodeVideo(const QUrl & input);

    QObject* getPlayer() const;

Q_SIGNALS:
    void playerChanged();
    void frameDecoded(const QtAV::VideoFrame & frame);

protected:
    QString m_deviceName;
    bool m_zeroCopy;
    QtAV::VideoSurfaceInteropPtr m_surfaceInterop;

private:
    int initHWContext(AVCodecContext *ctx, const enum AVHWDeviceType m_type);
    int decode(AVCodecContext *avctx, AVPacket *packet);
    void processStream(const QIODevice * buffer);
    void processFile(const QString & input);
    void sendFrame(const QtAV::VideoFrame & frame);

    virtual QtAV::VideoFrame createHWVideoFrame(const AVFrame * frame) = 0;

    AVHWDeviceType m_type;
    AVBufferRef *m_hwDeviceCtx;
    static AVPixelFormat m_hwPixFmt;

    AVCodec *m_decoder;
    AVFormatContext *m_inputCtx;
    AVCodecContext *m_decoderCtx;

    VideoSource m_videoSource;
    QFuture<void> m_processFuture;
};


#endif // HWDECODER_H
