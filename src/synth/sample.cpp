//
// Created by wirewhiz on 12/09/22.
//

#include "sample.h"


Sample& Sample::operator+=(const Sample& o)
{
    left += o.left;
    right += o.right;
    return *this;
}

Sample Sample::operator+(const Sample& o) const
{
    return {left + o.left, right + o.right};
}

Sample Sample::operator*(float v) const
{
    return {left * v, right * v};
}