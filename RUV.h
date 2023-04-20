#ifndef MYSTRUCT_H
#define MYSTRUCT_H
struct RUV {

    // Remotely Updateable Vairables
    // Variables included here can be updated over a serial link from a remote computer

    int testInt = -1;   // [0]
    double testDouble;  // [1]
};

extern RUV globalRUV;
#endif