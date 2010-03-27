-- This is an example for a Lua brush for the mapeditor
-- Brushs are a subset of the available scripts for the mapeditor
-- They are run for each field that the user 'draws' on with a pressed mouse button
-- This means the the scripts should not make any kind of user interaction 
-- (like dialogs, messages boxes, ...)
--
-- Available in: mapeditor


map = asc.getActiveMap()
      
-- get the tank blocker object      
blocker = asc.getObjectType( 998 )
      
-- get the ditch object      
ditch = asc.getObjectType( 10 )

-- get the current cursor position...
cursorpos = asc.getCursorPosition( map )
-- .. and the field for ot
field = map:getField( cursorpos )


-- the cursorposition shall be a ditch
-- if there is already a blocker, remove it
field:removeObject( blocker )

asc.placeObject( map, cursorpos, ditch )

-- now we iterate over all neighbouring fields
for i = 0,5 do
   -- getting the field next to cursorpos in direction i
   p = asc.getNeighbouringFieldCoordinate( cursorpos, i )
   field = map:getField(p)

   -- if cursorpos was at the edge of map, the coordinate p may not exist
   -- we check if the field really exists
   if field then

      -- only place a block if there is not already a ditch
      if not field:checkForObject( ditch ) then
         asc.placeObject( map, p, blocker )
      end
   end
end