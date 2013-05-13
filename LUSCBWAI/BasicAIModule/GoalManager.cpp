/*
* This is Zeus (the goal manager). The role of this class is to maintain the high level goal at
* any given point in time in order to keep all other components of the bot in sync. For example,
* if the bot is working towards a slow push goal, then the Macro Manager will need to build the 
* appropriate units to accomplish the goal (i.e. a lot of siege tanks and some anti-air units) and
* the Strategy Manager will need to create a big army from the Army manager, which will maintain
* multiple squads that will be used in the slow push (i.e. each squad is contains a sufficient 
* number of tanks and squads fight alongside each other maintaining a slowly moving wall towards the
* enemy base.
*/