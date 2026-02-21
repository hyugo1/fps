#ifndef ITEMS_H
#define ITEMS_H

struct HealthItem {
    float x, y;
    float width, height;
    bool collected = false;
};

struct SpeedItem {
    float x, y;
    float width, height;
    bool collected = false;
};

#endif