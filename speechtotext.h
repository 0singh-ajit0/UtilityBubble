#ifndef SPEECHTOTEXT_H
#define SPEECHTOTEXT_H

#include <QObject>

#include <sndfile.h>


class QTimer;

class SpeechToText : public QObject
{
    Q_OBJECT
public:
    explicit SpeechToText(QObject *parent = nullptr, QString audioPath = "", QString fileName = "output.wav", int segmentTime = 3);

public slots:
    void startSpeechToText();
    void stopSpeechToText();
    void transcribeAudio();
    std::string getTranscribedString();

signals:
    void transcribingFinished(QString text);

private:
    QString m_audioPath;
    QString m_fileName;
    int m_segmentTime;

    bool toTranscribe;
    QTimer *timer;

    SNDFILE *infile, *outfile;
    SF_INFO readFile_sfinfo, writeFile_sfinfo;
    int currentOutFileIndex = 1;
    int *audioIn;
    int numOfFramesPerSec;
    sf_count_t numOfFramesToSeek;
    sf_count_t numOfFramesToRead;
};

#endif // SPEECHTOTEXT_H
