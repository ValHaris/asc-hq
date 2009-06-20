-- This script will count the number of red units on the map
-- It does not count units that are loaded into transports or buildings
-- 
-- Available in: mapeditor


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
           counter = counter + 1
        end
     end
  end
end
asc.infoMessage( counter .. ' units on map' )
