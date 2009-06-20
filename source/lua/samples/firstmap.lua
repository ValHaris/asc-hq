-- This is an example for a script that runs in ASC and plays on a 
-- map.
-- Please load the first campaign map (asc001.map) and then run this script.
-- it will move some units around
-- 
-- This script was created by playing the first round and then saving
-- the actions with Action / Save Script


-- get handle to active map 
map = asc.getActiveMap() 

--Move unit SHOCK TROOPER to #coord(6/4)#
r = asc.unitMovement ( map, 414, asc.MapCoordinate( 6, 4), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Move unit SHOCK TROOPER to #coord(6/6)#
r = asc.unitMovement ( map, 414, asc.MapCoordinate( 6, 6), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Move unit SHOCK TROOPER to #coord(2/8)#
r = asc.unitMovement ( map, 411, asc.MapCoordinate( 2, 8), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Move unit SHOCK TROOPER to #coord(3/8)#
r = asc.unitMovement ( map, 379, asc.MapCoordinate( 3, 8), 3 )
if r:successful()==false then asc.displayActionError(r) end 

--Move unit SHOCK TROOPER to #coord(1/11)#
r = asc.unitMovement ( map, 415, asc.MapCoordinate( 1, 11), 3 )
if r:successful()==false then asc.displayActionError(r) end 
