#include "speechtotext.h"
#include <QDateTime>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDir>

#include "openai.hpp"


SpeechToText::SpeechToText(QObject *parent, QString audioPath, QString fileName, int segmentTime)
    : QObject{parent}
{
    m_audioPath = audioPath;
    m_fileName = fileName;
    m_segmentTime = segmentTime;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&SpeechToText::transcribeAudio));
}

void SpeechToText::startSpeechToText()
{
    toTranscribe = true;

    timer->start(5000);
}

void SpeechToText::stopSpeechToText()
{
    toTranscribe = false;
    delete audioIn;
    timer->stop();
}

void SpeechToText::transcribeAudio()
{
    QString destFilePath = QDir::homePath() + "/Music/output-" + QString::number(currentOutFileIndex) + ".wav";
    QFile::copy("/home/ajit/Music/output.wav", destFilePath);

    memset(&readFile_sfinfo, 0, sizeof(readFile_sfinfo));

    QByteArray temp = QString(QDir::homePath() + "/Music/output-" + QString::number(currentOutFileIndex) + ".wav").toLocal8Bit();
    const char *in_filename = temp.data();

    if (!(infile = sf_open(in_filename, SFM_READ, &readFile_sfinfo))) {
        qDebug() << "Cannot open file";
        qFatal(sf_strerror(NULL));
        exit(1);
    }

    readFile_sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_MS_ADPCM;

    if (!sf_format_check(&readFile_sfinfo)) {
        sf_close(infile);
        qDebug() << "Invalid encoding";
        return;
    }

    writeFile_sfinfo.channels = 1;
    writeFile_sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_MS_ADPCM;
    writeFile_sfinfo.samplerate = readFile_sfinfo.samplerate;

    numOfFramesPerSec = readFile_sfinfo.samplerate;
    numOfFramesToSeek = ((currentOutFileIndex - 1) * m_segmentTime) * numOfFramesPerSec;
    sf_count_t usefulFrames = readFile_sfinfo.frames - numOfFramesToSeek;
    numOfFramesToRead = usefulFrames < (m_segmentTime * numOfFramesPerSec) ? usefulFrames : (m_segmentTime * numOfFramesPerSec);
    audioIn = new int[readFile_sfinfo.channels * numOfFramesToRead + 1];

    sf_seek(infile, numOfFramesToSeek, SEEK_SET);
    if (sf_read_int(infile, audioIn, numOfFramesToRead) != 0)
    {
        QByteArray tempdata = QString(QDir::homePath() + "/Music/upload.wav").toLocal8Bit();
        const char* out_filename = tempdata.data();
        outfile = sf_open(out_filename, SFM_WRITE, &writeFile_sfinfo);
        sf_write_int(outfile, audioIn, numOfFramesToRead);
        sf_close(outfile);
        delete audioIn;

        QString text = QString::fromStdString(getTranscribedString());
        emit transcribingFinished(text);

        QFile::remove(QDir::homePath() + "/Music/upload.wav");
        QFile::remove(QDir::homePath() + "/Music/output-" + QString::number(currentOutFileIndex) + ".wav");
    }

    currentOutFileIndex++;
}

std::string SpeechToText::getTranscribedString()
{
    openai::start("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");

    auto transcription = openai::audio().transcribe(R"({
        "file": "/home/ajit/Music/upload.wav",
        "model": "whisper-1"
    })"_json);

    std::string result = transcription.dump(2);

    QString resultStr = QString::fromStdString(result);
    QJsonDocument doc = QJsonDocument::fromJson(resultStr.toUtf8());
    QJsonObject obj = doc.object();
    QJsonValue val = obj.value(QString("text"));

    return (val.toString().toStdString());
}


















