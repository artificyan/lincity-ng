/* ---------------------------------------------------------------------- *
 * organic_farm.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "organic_farm.h"


Organic_farmConstructionGroup organic_farmConstructionGroup(
    "Farm",
    FALSE,                     /* need credit? */
    GROUP_ORGANIC_FARM,
    4,                         /* size */
    GROUP_ORGANIC_FARM_COLOUR,
    GROUP_ORGANIC_FARM_COST_MUL,
    GROUP_ORGANIC_FARM_BUL_COST,
    GROUP_ORGANIC_FARM_FIREC,
    GROUP_ORGANIC_FARM_COST,
    GROUP_ORGANIC_FARM_TECH
);

Construction *Organic_farmConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new Organic_farm(x, y, type);
}


void Organic_farm::update()
{
    int i = (total_time + crop_rotation_key * 1200 + month_stagger) % 4800;
    int used_jobs = 0;
    int used_power = 0;
    int used_water = 0;
    int foodprod = 0;
    
    max_foodprod = 0; 
    /* check jobs */
    used_jobs = (FARM_JOBS_USED<commodityCount[STUFF_JOBS]?FARM_JOBS_USED:commodityCount[STUFF_JOBS]);
    flags &= ~(FLAG_POWERED);
    /* check for power */
    if (commodityCount[STUFF_KWH] >= ORG_FARM_POWER_REC) 
    {
        used_power = ORG_FARM_POWER_REC;            
        flags |= FLAG_POWERED;
        used_water = commodityCount[STUFF_WATER] / WATER_FARM;
        if (used_water > (16 - ugwCount))
        {
            used_water = (16 - ugwCount);
        }      
        
        foodprod = (ORGANIC_FARM_FOOD_OUTPUT + tech_bonus) * (ugwCount+used_water) * used_jobs / (16 * FARM_JOBS_USED);
        max_foodprod = (ORGANIC_FARM_FOOD_OUTPUT + tech_bonus) * (ugwCount/*+used_water*/)  / (16);
// prod>100% if extra water is used         
    }
    else
    {
        foodprod = (ORGANIC_FARM_FOOD_OUTPUT) * ugwCount * used_jobs / (4 * 16 * FARM_JOBS_USED);
        max_foodprod = (ORGANIC_FARM_FOOD_OUTPUT) * ugwCount / (4 * 16);      
    } 
    if (foodprod < 30)
        foodprod = 30;
    if (max_foodprod < 30) //that could only matter if Fertiliy = 0
        max_foodprod = 30;  
    
    if (commodityCount[STUFF_FOOD] + foodprod > MAX_ORG_FARM_FOOD) 
    {   //we would produce too much so use less power, jobs and water
        used_jobs = used_jobs * (MAX_ORG_FARM_FOOD - commodityCount[STUFF_FOOD]) / foodprod;
        used_power = used_power * (MAX_ORG_FARM_FOOD - commodityCount[STUFF_FOOD]) / foodprod;
        if ((MAX_ORG_FARM_FOOD - commodityCount[STUFF_FOOD])*16 < ugwCount * foodprod)
        {
            used_water = 0;
        }
        foodprod = MAX_ORG_FARM_FOOD - commodityCount[STUFF_FOOD];
    } 
    /* Now apply changes */   
    commodityCount[STUFF_JOBS] -= used_jobs;
    commodityCount[STUFF_FOOD] += foodprod;
    commodityCount[STUFF_KWH] -= used_power;
    commodityCount[STUFF_WATER] -= (used_water * WATER_FARM);
    food_this_month += 100 * foodprod / max_foodprod;  
    // monthly update
    if ((total_time % 100) == 0)
    {
        food_last_month = food_this_month;
        food_this_month = 0;
    }
    //Every three month
    if (i % 300 == 0)
    {
        i /= 300;
        if ( food_last_month > MIN_FOOD_SOLD_FOR_ANIM) 
        {
            //Every year
            if (i % 4 == 0)  
            {
                month_stagger = rand() % 100;
            }
            if (commodityCount[STUFF_WASTE] > ORG_FARM_WASTE_GET)
                commodityCount[STUFF_WASTE] -= ORG_FARM_WASTE_GET;
            else
                commodityCount[STUFF_WASTE] = 0;                  
            switch (i)
            {
                case (0):
                    type = CST_FARM_O3;
                    break;
                case (1):
                    type = CST_FARM_O3;
                    break;
                case (2):
                    type = CST_FARM_O3;
                    break;
                case (3):
                    type = CST_FARM_O3;
                    break;
                case (4):
                    type = CST_FARM_O7;
                    break;
                case (5):
                    type = CST_FARM_O7;
                    break;
                case (6):
                    type = CST_FARM_O7;
                    break;
                case (7):
                    type = CST_FARM_O7;
                    break;
                case (8):
                    type = CST_FARM_O11;
                    break;
                case (9):
                    type = CST_FARM_O11;
                    break;
                case (10):
                   type = CST_FARM_O11;
                    break;
                case (11):
                    type = CST_FARM_O11;
                    break;
                case (12):
                    type = CST_FARM_O15;
                    break;
                case (13):
                    type = CST_FARM_O15;
                    break;
                case (14):
                    type = CST_FARM_O15;
                    break;
                case (15):
                    type = CST_FARM_O15;
                    break;
            }
        }
        else
        {
            type = CST_FARM_O0;
        }
    }
}

void Organic_farm::report()
{
    int i = 0;

    mps_store_sd(i++, constructionGroup->name,ID);
    i++;
    mps_store_sddp(i++, "Fertility", ugwCount, 16);    
    mps_store_sfp(i++, _("Tech"), tech * 100.0 / MAX_TECH_LEVEL);
    mps_store_sfp(i++, _("busy"), (float)food_last_month / 100.0);
    mps_store_sd(i++, _("Output"), max_foodprod);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/organic_farm.cpp */

