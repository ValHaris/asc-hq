-- This is an example for a Lua brush for the mapeditor
-- Brushs are a subset of the available scripts for the mapeditor
-- They are run for each field that the user 'draws' on with a pressed mouse button
-- This means the the scripts should not make any kind of user interaction 
-- (like dialogs, messages boxes, ...)
--
-- This scripts places cliff objects on land fields which have a sea-field nearby.
-- It also does some additional nonsense just to show off ;-)
--
-- Available in: mapeditor


function isWater ( position )
   -- we are creating our own local map variable, as functions should ideally not
   -- depend on global variables, only on parameters
   local map = asc.getActiveMap()


   local field = map:getField(position)

   -- maybe there is no field for that coordinate
   -- because the coordinate is outside the map
   if not field then
      return false
   end


   -- an array holding the names of terrain properties   
   local waterBits = { 'Water', 'Shallow_water', 'Deep_water', 'Very_shallow_water' }

   -- iterating over the array
   --   pos will contain the index of the array, which we don't care about 
   --   value will contain the value, like 'Water', etc.
   for pos,value in pairs(waterBits) do
      if field:hasProperty( value ) then
         return true
      end
   end

   -- we didn't find any water in the loop
   return false
end


map = asc.getActiveMap()

      
-- get the cliff object      
cliff = asc.getObjectType( 2401 )

runway = asc.getObjectType( 45 )

rock = asc.getTerrainType( 2910 )
      
-- get the current cursor position...
cursorpos = asc.getCursorPosition( map )

-- we only do something if the field is not a water field
if isWater( cursorpos ) == false then

   waterFoundOnNextField = false

   -- now we iterate over all neighbouring fields
   for i = 0,5 do

      -- getting the field next to cursorpos in direction i
      p = asc.getNeighbouringFieldCoordinate( cursorpos, i )

      if isWater( p ) then
         waterFoundOnNextField = true
      end

   end

   -- get the field beneath the cursor
   field = map:getField( cursorpos )

   if waterFoundOnNextField == true then


      -- ok, now we want to build the cliff

      -- But cliffs cannot be build on fields that contain runways, so we check if there is a runway
      -- and remove it
      -- This is just for LUA demonstration purposes  
      if field:checkForObject( runway ) then
         field:removeObject( runway )
      end
      asc.placeObject( map, cursorpos, cliff )
   else
      -- if we can't build a cliff, because there is no surrounding water, then let's change the terrain!
      -- but only if the id is below 100
      if  field:getTerrainType():getID() < 100 then
         field:changeTerrainType( rock )  
      end
   end

end

