/* ---------------------------------------------------------------------- *
 * market.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include <cstdlib>
#include "modules.h"
#include "market.h"
#include "lincity-ng/ErrorInterface.hpp"
#include "../range.h"
#include "../transport.h"

MarketConstructionGroup marketConstructionGroup(
    "Market",
     FALSE,                     /* need credit? */
     GROUP_MARKET,
     2,                         /* size */
     GROUP_MARKET_COLOUR,
     GROUP_MARKET_COST_MUL,
     GROUP_MARKET_BUL_COST,
     GROUP_MARKET_FIREC,
     GROUP_MARKET_COST,
     GROUP_MARKET_TECH
);

Construction *MarketConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new Market(x, y, type);
}


void Market::update()
{
    int tmp, xx, yy, pears, n;
    int ratio;
    int market_lvl, market_cap;
    int market_ratio = 0;
    Commodities stuff_ID;
    std::map<Commodities, int>::iterator stuff_it;
    n = 0;
    for(stuff_it = commodityCount.begin() ; stuff_it != commodityCount.end() ; stuff_it++ )
    {
        stuff_ID = stuff_it->first;
        //dont handle stuff if neither give nor take
        if (!commodityRuleCount[stuff_ID].give 
         && !commodityRuleCount[stuff_ID].take)
        {
            continue;
        }
        //dont handle anything else than jobs if to little jobs
        if ((stuff_ID != STUFF_JOBS)
         && (commodityCount[STUFF_JOBS] < jobs)
           )
        {
            continue;
        }
        market_lvl = stuff_it->second;
        market_cap = constructionGroup->commodityRuleCount[stuff_ID].maxload;
        pears = 1;
        ratio = market_lvl * TRANSPORT_QUANTA / market_cap;
        market_ratio += ratio;
        n++;      
        for(yy = ys; yy < ye; yy++)
        {
            for(xx = xs; xx < xe; xx++)
            {
                //Count Constructions only once
                //Never count other markets transport or power lines
                if ( !world(xx,yy)->construction
                  || (world(xx,yy)->getGroup() == GROUP_MARKET)
                  || (world(xx,yy)->getGroup() == GROUP_POWER_LINE)
                  || world(xx,yy)->is_transport()
                   )
                {
                    continue;
                }
                tmp = collect_transport_info( xx, yy, stuff_ID, market_ratio);
                if (tmp != -1)
                {
                    ratio += tmp;                
                    pears++;
                }
            }//endfor xx
        }//endfor yy
        // not weighted average filling for stuff_ID
        ratio /= pears;
        for(yy = ys; yy < ye; yy++)
        {
            for(xx = xs; xx < xe; xx++)
            {
                //Deal with constructions only once
                //Never deal with markets power lines or transportTiles
                if ( !world(xx,yy)->construction
                  || (world(xx,yy)->getGroup() == GROUP_MARKET)
                  || (world(xx,yy)->getGroup() == GROUP_POWER_LINE) 
                  || world(xx,yy)->is_transport()
                   )
                {
                    continue;
                }             
                int old_lvl = market_lvl;
                //do the normal flow
                //if (collect_transport_info( xx, yy, stuff_ID, market_ratio)!=-1)
                //{
equilibrate_transport_stuff(xx, yy, &market_lvl, market_cap, ratio,stuff_ID);                
                //}
                int flow = market_lvl - old_lvl;
                //revert flow if it conflicts with local rules
                if((!commodityRuleCount[stuff_ID].give && flow < 0)
                 ||(!commodityRuleCount[stuff_ID].take && flow > 0))
                {
                    market_lvl -= flow;
                    world(xx,yy)->construction->commodityCount[stuff_ID] += flow;                
                }                   
            }//endfor xx
        }//endfor yy
        stuff_it->second = market_lvl;
    }//endfor stuff_it
    
    if (commodityCount[STUFF_JOBS] >= jobs)
    {
        commodityCount[STUFF_JOBS] -= jobs;
        //Have to collect taxes here since transport does not consider the market a consumer but rather as another transport        
        income_tax += jobs;
    }
   
    if (total_time % 25 == 17) 
    {
        //average filling of the market    
        market_ratio /= (100 * n);
        if (market_ratio < 10)
        {      
            type = CST_MARKET_LOW;
            jobs = JOBS_MARKET_LOW;
        }
        else if (market_ratio < 50)
        {
            type = CST_MARKET_MED;
            jobs = JOBS_MARKET_MED;
        }
        else
        {
            type = CST_MARKET_FULL;
            jobs = JOBS_MARKET_FULL;
        }
    }
    if (commodityCount[STUFF_WASTE] >= (95 * MAX_WASTE_IN_MARKET / 100) && !burning_waste)
    {
        old_type = type;        
        type = CST_FIRE_1;
        anim = real_time + WASTE_BURN_TIME;
        burning_waste = true;        
        world(x,y)->pollution += 3000;       
        commodityCount[STUFF_WASTE] -= (7 * MAX_WASTE_IN_MARKET) / 10;     
    }
    else if (burning_waste && real_time > anim)
    {
        type = old_type;
        burning_waste = false;
    }
}

void Market::cover() //do this for showing range in minimap
{
    for(int yy = ys; yy < ye; yy++)
    {
        for(int xx = xs; xx < xe; xx++)
        {
            world(xx,yy)->flags |= FLAG_MARKET_COVER;
        }
    }
}


void Market::report()
{
    int i = 0;

    mps_store_sd(i++,constructionGroup->name,ID);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/market.cpp */




