/* ---------------------------------------------------------------------- *
 * pottery.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "pottery.h"

PotteryConstructionGroup potteryConstructionGroup(
    "Pottery",
    FALSE,                     /* need credit? */
    GROUP_POTTERY,
    GROUP_POTTERY_SIZE,
    GROUP_POTTERY_COLOUR,
    GROUP_POTTERY_COST_MUL,
    GROUP_POTTERY_BUL_COST,
    GROUP_POTTERY_FIREC,
    GROUP_POTTERY_COST,
    GROUP_POTTERY_TECH,
    GROUP_POTTERY_RANGE
);

Construction *PotteryConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new Pottery(x, y, type);
}

void Pottery::update()
{
    if (total_time % 100 == 0)
    {
        busy = working_days;
        working_days = 0;
    }

    if (pauseCounter++ < 0)
    {   return;}

    if ((commodityCount[STUFF_GOODS] + POTTERY_MADE_GOODS <= MAX_GOODS_AT_POTTERY)
     && (commodityCount[STUFF_ORE] >= POTTERY_ORE_MAKE_GOODS)
     && (commodityCount[STUFF_COAL] >= POTTERY_COAL_MAKE_GOODS)
     && (commodityCount[STUFF_JOBS] >= POTTERY_JOBS))
    {
        commodityCount[STUFF_GOODS] += POTTERY_MADE_GOODS;
        commodityCount[STUFF_ORE] -= POTTERY_ORE_MAKE_GOODS;
        commodityCount[STUFF_COAL] -= POTTERY_COAL_MAKE_GOODS;
        commodityCount[STUFF_JOBS] -= POTTERY_JOBS;

        animate = true;
        if(!((working_days++)%10))
        {   world(x,y)->pollution++;}
    }
    else
    {
        type = CST_POTTERY_0;
        pauseCounter = -POTTERY_CLOSE_TIME;
        return;
    }
    if (animate && real_time > anim)
    {
        anim = real_time + POTTERY_ANIM_SPEED;
        switch (type)
        {
            case (CST_POTTERY_0):
                type = CST_POTTERY_1;
                break;
            case (CST_POTTERY_1):
                type = CST_POTTERY_2;
                break;
            case (CST_POTTERY_2):
                type = CST_POTTERY_3;
                break;
            case (CST_POTTERY_3):
                type = CST_POTTERY_4;
                break;
            case (CST_POTTERY_4):
                type = CST_POTTERY_5;
                break;
            case (CST_POTTERY_5):
                type = CST_POTTERY_6;
                break;
            case (CST_POTTERY_6):
                type = CST_POTTERY_7;
                break;
            case (CST_POTTERY_7):
                type = CST_POTTERY_8;
                break;
            case (CST_POTTERY_8):
                type = CST_POTTERY_9;
                break;
            case (CST_POTTERY_9):
                type = CST_POTTERY_10;
                break;
            case (CST_POTTERY_10):
                type = CST_POTTERY_1;
                animate = false;
                break;
        }
    }
}

void Pottery::report()
{
    int i = 0;

    mps_store_sd(i++, constructionGroup->name,ID);
    i++;
    mps_store_sfp(i++, "busy", (float) busy);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/pottery.cpp */

