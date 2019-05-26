#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>


#include <QCoreApplication>
#include <aubio/aubio.h>
#include <iostream>
#include <stdio.h>

#include <unistd.h>

#include <settings/config_reader.h>


#include "AudioInput.h"

int main(int argc, char * argv[]) {
    ConfigReader::instance();

    ConfigReader::instance().setValue<uint>(CoreSettings::frame_size, 1024);
    ConfigReader::instance().setValue<uint>(CoreSettings::bit_rate, 16);
    ConfigReader::instance().setValue<uint>(CoreSettings::sample_rate, 0);
    ConfigReader::instance().setValue<QString>(CoreSettings::source_path, "/srv/download/gamma.wav");


    QCoreApplication app(argc, argv);
    AubioReader jopa;
    QObject::connect(&jopa, &AudioInput::finishAlgo, &app, QCoreApplication::quit);
    QTimer::singleShot(500, &jopa, &AudioInput::startRecord);
    QTimer::singleShot(800, &jopa, &AudioInput::stopRecord);
    return app.exec();
}


void fvec_copy_to_end(const fvec_t * src, fvec_t * dist, const uint_t ind_beg_dist) {

    if(src->length + ind_beg_dist > dist->length) {
        printf("trying to copy %d elements to %d elements \n",
            src->length + ind_beg_dist, dist->length);
        return;
    }

    uint_t i;
    for(i = 0; i < src->length; ++i) {

        dist->data[i + ind_beg_dist] = src->data[i];
    }
}

void fvec_copy_to_start(const fvec_t * src, const uint_t ind_beg_src, fvec_t * dist) {

    if(src->length != dist->length) {
        printf("length src %d != length dist %d \n",
            src->length , dist->length);
        return;
    }

    uint_t i;
    uint_t size = dist->length - ind_beg_src;
    for(i = 0; i < size; ++i) {

        dist->data[i] = src->data[i + ind_beg_src];
    }
}


#include <vector>
#include <complex>


int maijhn(int argc, char * argv[]) {


    uint_t samplerate = 0;
    uint_t err = 0;
    uint_t win_size = 8192; // window size
    uint_t hop_size = win_size / 32;
    uint_t n_frames = 0, read = 0;

    char_t * source_path = "/srv/download/gamma.wav";

    aubio_source_t * source = new_aubio_source(source_path, samplerate, hop_size);

    if (!source) {
         err = 1;
         aubio_cleanup();
         return err;
    }

    if ( samplerate == 0 ) samplerate = aubio_source_get_samplerate(source);

    fvec_t * in = new_fvec(hop_size);
    fvec_t * out = new_fvec(1);

    fvec_t * win = new_fvec(win_size);
    cvec_t * fftgrain = new_cvec(win_size);

    aubio_tempo_t * o = new_aubio_tempo("default", win_size, hop_size, samplerate);

    aubio_fft_t * fft = new_aubio_fft(win_size);

    uint_t fill_size = 0;
    do {

        aubio_source_do(source, in, &read);

        if(fill_size < win_size) {
            fvec_copy(win, win);
        } else {
            fvec_copy_to_start(win, hop_size, win);
            fill_size -= hop_size;
        }

        fvec_copy_to_end(in, win, fill_size);
        fill_size += hop_size;

        if(fill_size >= win->length) {
            aubio_fft_do(fft, win, fftgrain);
            //cvec_print(fftgrain);

            std::vector< std::pair<float, float> > spectrumPoints;

            uint_t halfRange = win_size / 2;
            spectrumPoints.reserve(halfRange);
            for(uint_t i = 0; i < halfRange; ++i) {
                float z = fftgrain->norm[i];

                spectrumPoints.push_back(std::make_pair((float)i / win_size * samplerate / 1.0, z));
            }
        }

        aubio_tempo_do(o, in, out);

        if (out->data[0] != 0) {
            printf("beat at %.3fms, %.3fs, frame %d, %.2fbpm with confidence %.2f\n",
            aubio_tempo_get_last_ms(o), aubio_tempo_get_last_s(o), aubio_tempo_get_last(o),
            aubio_tempo_get_bpm(o), aubio_tempo_get_confidence(o));
        }

        n_frames += read;

    } while ( read == hop_size );

    printf("read %.2fs, %d frames at %dHz (%d blocks) from %s\n",
        n_frames * 1. / samplerate,
        n_frames, samplerate,
        n_frames / hop_size, source_path);
    // clean up memory


    del_fvec(in);
    del_fvec(out);

    del_fvec(win);
    del_cvec(fftgrain);

    del_aubio_tempo(o);
    del_aubio_fft(fft);
    del_aubio_source(source);

    aubio_cleanup();

    return 0;
}
