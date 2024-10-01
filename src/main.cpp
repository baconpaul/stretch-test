#include <iostream>
#include <sndfile.hh>

#include "signalsmith-stretch.h"

int main(int argc, char **argv)
{
    std::cout << "Hello World" << std::endl;
    auto h = SndfileHandle("test-data/testin.wav");

    std::cout << h.samplerate() << " " << h.channels() <<  " " << h.frames() << std::endl;

    if (h.format() & SF_FORMAT_PCM_16)
    {
        std::cout << "I'm PCM 16" << std::endl;
    }
    else
    {
        std::cout << "PCM 16 me please";
        exit(2);
    }

    // hideously inefficient of course
    std::vector<short> D;
    D.resize(h.frames() * h.channels());
    h.read(D.data(), h.frames() * h.channels());


    float **d = new float* [2];
    d[0] = new float[h.frames()];
    d[1] = new float[h.frames()];
    for (int i=0; i<h.frames(); i += 2)
    {
        d[0][i] = D[2*i] * 1.f / (1<<15);
        d[1][i] = D[2*i+1] * 1.f / (1<<15);
    }

    float **dob = new float* [2];
    dob[0] = new float[h.frames()];
    dob[1] = new float[h.frames()];


    for (auto semi = -12; semi <= 12; semi += 3)
    {
        using Stretch = signalsmith::stretch::SignalsmithStretch<float>;
        Stretch stretch;
        stretch.presetDefault(h.channels(), h.samplerate());
        // transform

        stretch.setTransposeSemitones(semi, 8000/h.samplerate());
        stretch.process(d, h.frames(), dob, h.frames());



        // Re-assemble and write
        for (int i=0; i<h.frames(); ++i)
        {
            D[2*i] = (short)(dob[0][i] * (1<<15));
            D[2*i+1] = (short)(dob[1][i] * (1<<15));
        }

        auto sn = std::string("test-data/testout_semi") + std::to_string(semi) + ".wav";
        auto w = SndfileHandle(sn.c_str(), SFM_WRITE, h.format(), h.channels(), h.samplerate());
        w.write(D.data(), D.size());
    }

    delete[] dob[0];
    delete[] dob[1];
    delete[] dob;
    delete[] d[0];
    delete[] d[1];
    delete[] d;
}
