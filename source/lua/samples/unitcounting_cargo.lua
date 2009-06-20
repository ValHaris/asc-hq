-- This script will count the number of red units on the map
-- This is an extension to unitcounting.lua and DOES count cargo
-- It also demonstrates the usage of Lua functions
-- 
-- Available in: mapeditor



function countCargo ( carrier )
   local sum = 1
   local c
   -- the cargo array is 0 indexed
   for c = 0, carrier:getCargoCount()-1 do
       local unit = carrier:getCargo(c)
             
       -- the cargo may have empty slots. This is deliberately done so that
       -- the units in the cargo dialog don't shuffle around when the players 
       -- moves them out one by one
       if unit then
           sum = sum + countCargo( unit )
       end
   end
   return sum
end


counter = 0


-- get the active map
map = asc.getActiveMap()

-- iterate over all fields of the map
for y = 0,map:height()-1 do
  for x = 0,map:width()-1 do
     fld = map:getField(x,y)

     -- check if there is a unit on the field
     if fld:getVehicle() then

        -- check if the units belongs to red player (who is number 0)
        if fld:getVehicle():getOwner() == 0 then

           -- increase our counter 
           counter = counter + countCargo( fld:getVehicle() )
        end
     end
           
     -- the same for buildings
     -- we are only checking for the building entrance, to make sure
     -- that buildings covering several fields are only counted once
     building = fld:getBuildingEntrance()
     if building then
        if building:getOwner() == 0 then
           counter = counter + countCargo( building ) -1
           -- we are subtracting - 1, because we don't want to have the building being counted too
        end
     end
           
  end
end
asc.infoMessage( counter .. ' units on map' )
