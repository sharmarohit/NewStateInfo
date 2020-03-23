#ifndef RECORDCOORDS_H
#define RECORDCOORDS_H

struct RecordCoords
{
    RecordCoords()
    {
        maxOwnerY = 0;
        prevStateY = 1000;
        reset();
    }
    int stateY, prevStateY;
    int maxOwnerY;

    void reset()
    {
        stateY = 1000;
    }
};

#endif // RECORDCOORDS_H
