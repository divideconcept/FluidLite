#include "mainwindow.h"
#include "ui_mainwindow.h"

#define SAMPLE_RATE 44100
#define SAMPLE_SIZE 2 //4: Float Buffer   2: Signed Int Buffer
#define NUM_FRAMES SAMPLE_RATE
#define NUM_CHANNELS 1
#define NUM_SAMPLES (NUM_FRAMES * NUM_CHANNELS)


Generator::Generator(const QAudioFormat &format
    , qint64 durationUs
    , int sampleRate)
{
    settings = new_fluid_settings();
    synth = new_fluid_synth(settings);
    fluid_synth_sfload(synth, "soundfont.sf2", 1);

    if (format.isValid())
        generateData(format, durationUs, sampleRate);
}

Generator::~Generator()
{
    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
}

void Generator::start()
{
    open(QIODevice::ReadOnly);
}

void Generator::stop()
{
    m_pos = 0;
    close();
}

void Generator::generateData(const QAudioFormat &format, qint64 durationUs, int nKey)
{
    const int channelBytes = format.sampleSize() / 8;
    const int sampleBytes = format.channelCount() * channelBytes;
    qint64 length = (format.sampleRate() * format.channelCount() * (format.sampleSize() / 8))
                        * durationUs / 1000000;

    m_buffer.resize(length);
    unsigned char *ptr = reinterpret_cast<unsigned char *>(m_buffer.data());
    memset(ptr, 0, length);

    fluid_synth_noteon(synth, 0, nKey, 127);

    if( format.sampleType() == QAudioFormat::Float )
        fluid_synth_write_float(synth, format.sampleRate(), m_buffer.data(), 0, NUM_CHANNELS, m_buffer.data(), 1, NUM_CHANNELS);
    else if(format.sampleType() == QAudioFormat::SignedInt)
        fluid_synth_write_s16(synth, format.sampleRate(), m_buffer.data(), 0, NUM_CHANNELS, m_buffer.data(), 1, NUM_CHANNELS);


    fluid_synth_noteoff(synth, 0, nKey);

//    FILE* file;
//    fopen_s(&file, "test.pcm", "wb");
//    fwrite(m_buffer.data(), 1, m_buffer.length(), file);
//    fclose(file);
}

qint64 Generator::readData(char *data, qint64 len)
{
    qint64 total = 0;
    if (!m_buffer.isEmpty()) {
        while (len - total > 0) {
            const qint64 chunk = qMin((m_buffer.size() - m_pos), len - total);
            memcpy(data + total, m_buffer.constData() + m_pos, chunk);
            m_pos = (m_pos + chunk) % m_buffer.size();
            total += chunk;
        }
    }
    return total;
}

qint64 Generator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 Generator::bytesAvailable() const
{
    return m_buffer.size() + QIODevice::bytesAvailable();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initializeAudio(QAudioDeviceInfo::defaultOutputDevice());
}

MainWindow::~MainWindow()
{
    delete ui;    
}

void MainWindow::initializeAudio(const QAudioDeviceInfo &deviceInfo)
{
    QAudioFormat format;
    format.setSampleRate(SAMPLE_RATE);
    format.setChannelCount(NUM_CHANNELS);
    format.setSampleSize(SAMPLE_SIZE*8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);

    if( SAMPLE_SIZE==4 )
        format.setSampleType(QAudioFormat::Float);
    else if( SAMPLE_SIZE==2 )
        format.setSampleType(QAudioFormat::SignedInt);

    if (!deviceInfo.isFormatSupported(format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        format = deviceInfo.nearestFormat(format);
    }

    int nKey = 60 + rand() % 30;
    m_generator.reset(new Generator(format, 1500000, nKey)); //1500000:duration us
    m_audioOutput.reset(new QAudioOutput(deviceInfo, format));
    m_generator->start();

    QAudio::convertVolume(m_audioOutput->volume(),
                                                QAudio::LinearVolumeScale,
                                                QAudio::LogarithmicVolumeScale);

    m_audioOutput->start(m_generator.data());
}

void MainWindow::on_pushButton_clicked()
{
    m_audioOutput->stop();
    initializeAudio(QAudioDeviceInfo::defaultOutputDevice());
}

