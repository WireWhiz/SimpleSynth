//
// Created by wirewhiz on 12/09/22.
//

#ifndef SIMPLESYNTH_SAMPLE_H
#define SIMPLESYNTH_SAMPLE_H

struct Sample
{
    float left;
    float right;
    Sample& operator+=(const Sample&);
    Sample operator+(const Sample&) const;
    Sample operator*(float) const;
};

#endif //SIMPLESYNTH_SAMPLE_H
