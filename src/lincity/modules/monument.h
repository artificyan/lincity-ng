#define GROUP_MONUMENT_COLOUR (white(15))
#define GROUP_MONUMENT_COST   10000
#define GROUP_MONUMENT_COST_MUL 25
#define GROUP_MONUMENT_BUL_COST   1000000
#define GROUP_MONUMENT_TECH   0
#define GROUP_MONUMENT_FIREC  0

#define BUILD_MONUMENT_JOBS     350000
#define MONUMENT_GET_JOBS       100
#define MAX_JOBS_AT_MONUMENT    (MONUMENT_GET_JOBS*20)
#define MONUMENT_DAYS_PER_TECH  3
#define MONUMENT_TECH_EXPIRE    400
#define MONUMENT_ANIM_SPEED     300 //actually used?

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"

class MonumentConstructionGroup: public ConstructionGroup {
public:
    MonumentConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance, int cost, int tech
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech
    ) {
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_MONUMENT;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        
    }
    // overriding method that creates a monument
    virtual Construction *createConstruction(int x, int y, unsigned short type);
};

extern MonumentConstructionGroup monumentConstructionGroup;

class Monument: public CountedConstruction<Monument> { // Monument inherits from is own CountedConstruction
public:
	Monument(int x, int y, unsigned short type): CountedConstruction<Monument>(x, y, type) 
    {
        constructionGroup = &monumentConstructionGroup;
        this->tech_made = 0;
        setMemberSaved(&this->tech_made, "tech_made");  
        this->tail_off = 0;
        setMemberSaved(&this->tail_off, "tail_off");
        this->completion = 0;
        setMemberSaved(&this->completion, "completion");
        this->jobs_consumed = 0;
        setMemberSaved(&this->jobs_consumed, "jobs_consumed");
        initialize_commodities();           
        
    }

	virtual ~Monument() { }
	virtual void update();
	virtual void report();

    int  tech_made;
    int  tail_off;    
    int  completion;
    int  jobs_consumed;    
};

/** @file lincity/modules/monument.h */

