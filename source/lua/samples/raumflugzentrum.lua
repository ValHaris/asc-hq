
map = asc.getActiveMap()
centerpos = asc.getCursorPosition( map )
beton = asc.getObjectType( 12012 )
isgzone = asc.getObjectType( 12015 )
hq = asc.getBuildingType( 12025 )
sockel = asc.getObjectType( 12013 )
jammer = asc.getUnitType( 12010 )

owner = asc.selectPlayer( map )
if owner < 0 then
  return
end

pos = centerpos
asc.clearField( map, pos )
asc.placeObject( map, pos, beton )
asc.placeObject( map, pos, isgzone )
asc.placeBuilding( map, pos, hq, owner )

for  dir = 0, 5  do
  local pos = asc.getNeighbouringFieldCoordinate( centerpos, dir ) -- 1 Feld in die richtung
  asc.clearField( map, pos )
  asc.placeObject( map, pos, beton )
  asc.placeObject( map, pos, isgzone )
end


function betonUmgebung ( position )
   for dir = 0, 5 do
      local p  = asc.getNeighbouringFieldCoordinate( position, dir ) 
      asc.placeObject( map, p, beton ) 
   end
end

function baueQuadrant( dir1, dir2 )

  local pos = asc.getNeighbouringFieldCoordinate( centerpos, dir1 ) 
  pos = asc.getNeighbouringFieldCoordinate( pos, dir1 )      

  asc.clearField( map, pos )
  asc.placeObject( map, pos, beton )

  pos = asc.getNeighbouringFieldCoordinate( pos, dir2 )       
  betonUmgebung( pos )
  asc.clearField( map, pos )
  asc.placeObject( map, pos, beton )
  asc.placeObject( map, pos, sockel, true )

  pos = asc.getNeighbouringFieldCoordinate( pos, dir2 )      
  betonUmgebung( pos )
  asc.clearField( map, pos )
  asc.placeObject( map, pos, beton )
  asc.placeObject( map, pos, sockel, true )
  asc.placeUnit( map, pos, jammer, owner )
end

baueQuadrant( 0, 2 )
baueQuadrant( 0, 4 )
baueQuadrant( 3, 1 )
baueQuadrant( 3, 5 )

