-- This is an example for getting all fields in a certain distance of a given coordinate.
-- The script will create a circle of road objects with a radius of 3 around the 
-- cursor position
--
-- Available in: mapeditor

map = asc.getActiveMap();
centerpos = asc.getCursorPosition( map )
road = asc.getObjectType( 1 )

-- get all fields with a distance of 3
fields = asc.getFieldsInDistance( map, centerpos, 3 )

-- note that this is a 1 based array, as is normal in Lua
for i = 1, fields:size()  do
   pos = fields:getItem(i)
   asc.placeObject( map, pos, road )
end
